//------------------------------------------------------------------------------------------------------
//
//
//
// OSEnergy Dash Display
//
//      Copyright (c) 2018 by William A. Thomason.      http://arduinoalternatorregulator.blogspot.com/
//
//
//
//              This program is free software: you can redistribute it and/or modify
//              it under the terms of the GNU General Public License as published by
//              the Free Software Foundation, either version 3 of the License, or
//              (at your option) any later version.
//      
//              This program is distributed in the hope that it will be useful,
//              but WITHOUT ANY WARRANTY; without even the implied warranty of
//              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//              GNU General Public License for more details.
//      
//              You should have received a copy of the GNU General Public License
//              along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
//
//
//
//    This software is to be used in conjunction with the 2" dash display hardware by _______________________________________
//      It will monitor the CAN communicaitons network, logging OSEnergy message packets and display the status on the dash
//
//      Future versions may also allow limited configuration changes.
//
//
//      07/16/2018  v0.1.0      1st workable release
//      07/02/2018  vx.x.x      1st edits,
//
//
//
//
//
//------------------------------------------------------------------------------------------------------
//
//

                                               
#include "OSE_Dash.h"
#include "CAN_handler.h"
#include "Display.h"
#include "LED.h"






tBatteries  batteries[MAX_BATTERIES];                       // Datastach for Batteries
tChargers   chargers[MAX_CHARGERS];                         // And chargers.

int batFocus;                                               // Which battery in the battery datastash is being displayed?  (-1 = uninitialized)
int chargerFocus;                                           // Or are we to display a charger for that battery instead?  If so, this is the charger to show.
                                                            //  -1 = show battery details, not charger.                                                           
unsigned long last_CAN_activity;                                                    
unsigned long last_USER_activity; 


#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>


void setup() {

      
#ifdef DEBUG
    Serial.begin(115200);                                   // Just for testing, will not use Serial in production...
    Serial.println("Ready - OSEnergy Dash Display");
    #endif
    
  

    init_CAN();
    init_display();


    //--  Power off subsystems we are not going to use.
    #ifndef DEBUG
        power_lin_disable();
        #endif
        
    power_psc_disable();
    WDTCSR  = (uint8_t) 0;                                              // Disable Watchdog timer and its clock
    DACON  &= (uint8_t) ~(1<<DAEN);                                     // Disable DAC subsystem


      

}





//------------------------------------------------------------------------------------------------------
//
//  Handle Sleep States
//      This fucntion monitors for activity on the CAN as well as user activity via the push-buttons.
//      If there is no activity for a period of time, it will cause the display to be blanked, and if there is
//      no acvity on the CAN for a period of time will send the unit into SLEEP mode.
//
//      When actiity is noted, it will wake up the device.
//
//      Note that the ATmega64M1 chip will only allow wakeups from external IRQs, not internal ones.
//          As such, only the user pressing the CENTER key will wake us up.  CAN activity will not..
//
//
//
//
//------------------------------------------------------------------------------------------------------


void  handle_sleep_states(void) {


//!! HEY!!!  Be smarter about when to go to sleep.  30 second timeout is OK, but maybe if
//      user has pressed into a detail screen stay awake until the move back to the top level.
//      Also, maybe if we are tracking a shore-powered or engien power charing soruce, just leave
//      display active???
//      OR  consdier:  Maybe always leave the dispay active until the user presses the RIGHT key to cycle off
//      the end of the known batteries...
//


    //---   First we check to see if the display should be darkened.
    //
    if (((millis() - last_CAN_activity)  >= DISPLAY_TIMEOUT) &&       // CAN is dead .. 
        ((millis() - last_USER_activity) >= DISPLAY_TIMEOUT))  {      // .. and user has not pressed a button for a while -- so shut down display
            display_sleep(true);
        } else {
            display_sleep(false);                                   // Things are still happening, make sure display is on!
        }





    //----  Next we check to see if all activity has stopped, and if so we shoudl enter lowest-power state!
    //
    if (((millis() - last_CAN_activity)  >= SLEEP_TIMEOUT) &&       // CAN is dead (ahm, no CAN traffic is noted) .. 
        ((millis() - last_USER_activity) >= SLEEP_TIMEOUT))  {      // .. and user has not pressed a button for a while -- so go to sleep

        #ifdef DEBUG
            Serial.println("Good Night");
            delay(1000);
            #endif

        

        //-- Put CPU into a sleep mode, wait for an IRQ from the CAN or the user pressing a button to wake is up.
        //        
      
        //---   Power down the subsystems as they will not be used while sleeping.
        CAN_sleep(true);                                            // Put CAN subsystem into sleep mode;
        display_sleep(true);

        power_adc_disable();
        power_can_disable(); 
        power_spi_disable();
        power_timer0_disable();
        power_timer1_disable();



        //---  Enable IRQs for the touch screen keypads /  CAN Rx activity so that they can be used to awaken the CPU from a SLEEP state
        //         See OSE_dash.h to define which IRQ ports are actualy enabled by hardware.
        
        #ifdef KP_LEFT_IRQ
            attachInterrupt (KP_LEFT_IRQ,   dummy_IRQ, FALLING);                           
            #endif
        #ifdef KP_CENTER_IRQ
            attachInterrupt (KP_CENTER_IRQ, dummy_IRQ, FALLING);                             
            #endif
        #ifdef KP_RIGHT_IRQ
            attachInterrupt (KP_RIGHT_IRQ, dummy_IRQ, FALLING);                                 
            #endif
        #ifdef CAN_Rx_IRQ
            attachInterrupt (CAN_Rx_IRQ,   dummy_IRQ, FALLING);                                
            #endif

        
        //---   This will actualy place the CPU into a sleep state, while assuring IRQs are not missed.
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);      
  
        cli();
        sleep_enable();
        sei();
        sleep_cpu();
        sleep_disable();
        sei();


        //----  CPE has awaken after receving an IRQ   
        //---  Detach external IRQs, so the CPU does not get swamped with requests.  (Most notably, the CAN Rx IRQ)
        //
        #ifdef KP_LEFT_IRQ
            detachInterrupt (KP_LEFT_IRQ);                        
            #endif
        #ifdef KP_CENTER_IRQ
            detachInterrupt (KP_CENTER_IRQ);                             
            #endif
        #ifdef KP_RIGHT_IRQ
            detachInterrupt (KP_RIGHT_IRQ);                                 
            #endif
        #ifdef CAN_Rx_IRQ
            detachInterrupt (CAN_Rx_IRQ);                                
            #endif



        //---   Power-back up needed subsystems.     
        power_adc_enable();
        power_can_enable();                    //!! HEY!!  HUM, CANN I DO THIS, OR DO I NEED TO LEAVE IT RUNNING???  Do I need to reinit the CAN?
        power_spi_enable();
        power_timer0_enable();
        power_timer1_enable();



        last_CAN_activity  = millis();                              // Timer chip has been restarted, need to re-establish baseline timeput values
        last_USER_activity = millis();
        
        init_CAN();                                                 // Wake up  and make sure CAN is properly initalized.  
                                                                    // Will clear datastores and broadcast request RBMs for statuses as well.
        display_sleep(false);                                       // Wake up the display.                                                            
        }
}






//------------------------------------------------------------------------------------------------------
// Dummy IRQ Handler
//      Just a place for the keypad IRQs to attach to.  We do nothing here, as the purpose of the IRQ is to
//      allow the CPU to be takes out of SLEEP mode.  (Plus, the u8g2 lib may well reassign the IRQs itself.
//
//------------------------------------------------------------------------------------------------------

void dummy_IRQ(void) {

}







//------------------------------------------------------------------------------------------------------
//
//  Loop
//      "Main" loop in Arduino world
//
//
//------------------------------------------------------------------------------------------------------



void loop() {


    //!! HEY!!!  NOT SURE I REALLY NEED THIS!!!
    /*
    uint32_t static last_RBM_request = 0;
    

     if (((millis() - last_RBM_request)  >= (45*60*1000UL)) &&      // Incase there are no active chargers present, --AND--
         (display_sleeping() == false)) {                           // we are not in a semi-sleep mode 
        CAN_status_request();                                       //   send out a DC-Status4 requestevery 45 seconds to keep all RBMs from going to sleep.
        last_RBM_request = millis();
        }
        */

        

    process_CAN_Messages();                                         // Fetch and process any incomming CAN messages

    purge_datastash();                                              // Clear out any expired (timesout) enteries in the datastash.

    update_display();                                               // Refersh the display.
    update_LED();
    handle_keypad();

    
    handle_sleep_states();                                          // See if we should blank the display and/or go into sleep mode due to inactivity on CAN bus.


}



