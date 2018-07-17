//      Display.cpp
//
//      Copyright (c) 2018 by William A. Thomason.      http://arduinoalternatorregulator.blogspot.com/
//                                                                  http://smartdcgenerator.blogspot.com/
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
//              aint32_t with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//


                                               
#include "OSE_Dash.h"
#include "Display.h"

#include <U8g2lib.h>



//---  Internal prototypes and constructors
void show_battery(unsigned index, bool extended);
void show_charger(unsigned index, bool extended);
void show_charger_fault(unsigned index, bool extended);
void dummy_IRQ(void);

U8G2_SH1106_128X64_WINSTAR_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 11, /* dc=*/ 12, U8X8_PIN_NONE);



bool showingError = false;                              // Is there an error or Fault message on the display?  
                                                        // If so, do nothing more until user presses a button to clear it.
                                                        





//------------------------------------------------------------------------------------------------------
// Initialize Display
//      Call once during startup.
//
//
//
//------------------------------------------------------------------------------------------------------

bool init_display(void) {

                 


    u8g2.begin(KP_CENTER_PORT, KP_RIGHT_PORT, KP_LEFT_PORT, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE);
    u8g2.setFlipMode(0);                                    // making sure it rotate the right way.

}







//------------------------------------------------------------------------------------------------------
//
//  Display Sleep
//      Controls power mode of display subsystem.  
//
//
//
//------------------------------------------------------------------------------------------------------

void    display_sleep(bool sleep) {

    if (sleep)                              //-- Turn off the OLED display
        u8g2.setPowerSave(1);
    else
        u8g2.setPowerSave(0);

               
}









//------------------------------------------------------------------------------------------------------
//
//  Handle Keypads
//      This function will look to see if the user has pressed a keypad, and if so make changes to what handle_display() should look at.
//
//
//------------------------------------------------------------------------------------------------------

void handle_keypad(void) {

    int     i;
    int8_t  key;


    key = u8g2.getMenuEvent();                                                      // See if a key wass pressed.

    #ifdef DEBUG
        char c;
        
        if (Serial.available()) {
            c = Serial.read();
            if (c == 'l')   key = U8X8_MSG_GPIO_MENU_PREV;
            if (c == 'r')   key = U8X8_MSG_GPIO_MENU_NEXT;
            }
        #endif
        

        
    if (key != 0) {  
        
        last_USER_activity = millis();                                              // We have a key!                                    
        display_sleep(false);                                                       // If display is powered down, turn it on.


        #ifdef DEBUG
            Serial.print(" Keypad recevied: ");
            Serial.println(key,HEX);
            #endif

        showingError = false;                                                       // user pressed a key, release the screen.   
        
        switch (key) {
    
            case U8X8_MSG_GPIO_MENU_NEXT:                                           // Right keypad - User wants to see next battery
                                chargerFocus = -1;                                  // Moving to next (if one is present) battery, stop showing chargers (if we are..)
    
                                for (i = batFocus; i < (batFocus + MAX_BATTERIES); i++)                
                                    if ((batteries[(i%MAX_BATTERIES)].batID != 0xff) &&  (batteries[(i%MAX_BATTERIES)].batID != batteries[batFocus].batID)) {
                    
                                        batFocus = (i%MAX_BATTERIES);               // Found an active entery that is a different battey
                                        break;                                      // All done, we will now display this new battery.
                                    }
    
                                break;
    
                                
    
    
    
    
            case U8X8_MSG_GPIO_MENU_PREV:                                           // Left keypad - User want to see details of chargers.
                                i = chargerFocus+1;                                 // OK, a little tricky here.   IF this is the 1st time we are looking at chargers, 
                                                                                    // i will be set to 0  (-1 ++ --> 0).  However, if we are already looking at chargers
                                                                                    // (chargerFocus != -1), lets look at the next charger in the datastore to see if it
                                                                                    // is a match.
                                            
                                for (; i < MAX_CHARGERS; i++)
                                    if ((chargers[i].devID != 0xff) && (chargers[i].batID == batteries[batFocus].batID)) {
                                        chargerFocus = i;                           // Found a new charger to show
                                        break;
                                        }
           
                               if (chargerFocus != i)                               // Did we find a new charger to look at?
                                    chargerFocus = -1;                              // Nope. Go back to showing BAT info.
                                    
                                break;                                        
    
    
    
    
            case U8X8_MSG_GPIO_MENU_SELECT:                                         // Center keypad
            case U8X8_MSG_GPIO_MENU_HOME:
            case U8X8_MSG_GPIO_MENU_UP:
            case U8X8_MSG_GPIO_MENU_DOWN:
            default:                     
                                break;
    

        }
    }
}





//------------------------------------------------------------------------------------------------------
//
//  Update Diplay
//      Formates and updates the display with approperate information.  
//      Responds to uses requests for which page they with to see.  This function will also 
//      monitor for a fault condition, and show that informatin if one is detected (as well as 
//      flash the LED).
//      
//      Finaly, this fucntion will monitor the ambient light level and adjsut the display accordenly.
//
//
//
//------------------------------------------------------------------------------------------------------




void  update_display(void) {
    int i;
    

#ifdef SIMULATION
    void stuff_datapools(void);
    stuff_datapools();
    #endif




        
    //---   Adjust OLED display brightness level for ambient lighting level.
    //
    
    u8g2.setContrast(255);                                  // For now, just FULL BEAMS ON!

    


    if (showingError == true)    return;                                // If there is some type of error or fault message on the screen, do no
                                                                        // updates until the user presses a key to release the screen.


    //---  Is there a new FAULT condition somewhere in the system?
    //      update_LED() will take care of lighting the RED LED for us.

     for (i=0; i<MAX_CHARGERS; i++) {
        if (chargers[i].SPN.value != 0) {

            //--- A Charger is at fault!  This takes the cake, show it 1st!
            show_charger_fault(i, false);
            return;
       }
     }


    //---   Has what we were looking at expired?
    if ((batFocus != -1) && ((millis() - batteries[batFocus].volts.timestamp) >= BAT_STATUS_TIMEOUT))                   // Battery expired?
        batFocus = -1;

    if ((chargerFocus != -1) && ((millis() - chargers[chargerFocus].Vdc.timestamp) >= CHARGER_STATUS_TIMEOUT))                    // Charger expired?
        chargerFocus = -1;
            

    
    if (batFocus == -1) {                                                   // Display is just starting up. 
        chargerFocus = -1;                                                  // (We will we want to see the battery, not the charger.)
            
        batFocus = 0;                                                       // Assume it will be the 1st entery.
        for (i = 0; i < MAX_BATTERIES; i++)
            if (batteries[i].batID < batteries[batFocus].batID)
                batFocus = i;                                               // Found an active entery with a lower batID.  

        if (batteries[batFocus].batID == 0xff)                              // No active batteries were found...
            batFocus = -1;

        
    }





    //---   Which battery / charger to show?
    //


    if (batFocus == -1) {                                                   // After looking, is there no battery to dsiplay? (Batfocus is STILL == -1!)
        u8g2.clearBuffer();           
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(23,10, "OSEnergy Dash");
        //u8g2.drawStr(50,20, "Display");
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.drawStr(25,40, "Searching");
        u8g2.sendBuffer();                    
        
        return;   
        }



    //----  OK, time to actualy display something!
    //      (Assuming we are not showign a FAULT condition)
    
    if (chargerFocus == -1)                                                 // Does user want to see a charger detail?                                         
        show_battery(batFocus,false);
    else
        show_charger(chargerFocus,false);                                       



}


    


 





//------------------------------------------------------------------------------------------------------
//
//  Show Battery
//      This function will construct and display the battery informaion from the Batteru[] datastore
//      The battery entery shown is INDEX.
//      If 'Extended' is true, an alternative display with extended battery informaion will be shown.
//
//
//------------------------------------------------------------------------------------------------------


void    show_battery(unsigned index, bool extended) {



    if (index >= MAX_BATTERIES)          return;                   // Tisk-tisk, out of range.
    if (batteries[index].batID == 0xff)  return;                   // This is an EMPTY spot!
         

    u8g2.clearBuffer();                    // clear the internal memory  //!! HEY!!  SHOULD I BE USING A 2nd BUFFER TO CONSTRUCT DATA INTO, TO PREVENT FLICKERING?
    u8g2.setFont(u8g2_font_ncenB08_tr);   // choose a suitable font

    u8g2.setCursor(0, 10);                                      // Hum,  maybe later print putup some type of battery icon.
    switch (batteries[index].batID) {

        case 1:     u8g2.print("House Battery");
                    break;

        case 2:     u8g2.print("Starter Battery");
                    break;

        case 3:     u8g2.print("House #2 Battery");
                    break;


        default:     u8g2.print("Bat: ");
                     u8g2.print(batteries[index].batID);
                     break;
        }




    if ((millis() - batteries[index].volts.timestamp) < BAT_STATUS_TIMEOUT) {               // Valid timestamp?   
        u8g2.setFont(u8g2_font_ncenB24_tr);
        u8g2.setCursor(19,45);
        u8g2.print(batteries[index].volts.value,1);
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.print("V");
        }


        
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(8,64);     
    if ((millis() - batteries[index].amps.timestamp) < BAT_STATUS_TIMEOUT)
        u8g2.print(batteries[index].amps.value,1);
    else
        u8g2.print("---");
    u8g2.setFont(u8g2_font_5x7_tr);   
    u8g2.print("A");


    
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(80,64);
    if ((millis() - batteries[index].temp.timestamp) < BAT_STATUS_TIMEOUT) 
        u8g2.print(batteries[index].temp.value);
    else
        u8g2.print("--- ");
    u8g2.setFont(u8g2_font_5x7_tr);  
    u8g2.print("C");


  
    
   if ((millis() - batteries[index].SOC.timestamp) < BAT_STATUS_TIMEOUT) {             
        u8g2.drawFrame(115, 5,10,50);
        u8g2.drawBox  (115, (5 +(50-(batteries[index].SOC.value*50/100))),
                        10,         (batteries[index].SOC.value*50/100));
        u8g2.setFont(u8g2_font_5x7_tr);   
        u8g2.drawStr(113,64,"soc");
        }
   


        
    u8g2.sendBuffer();                    // transfer internal memory to the display
  
}





//------------------------------------------------------------------------------------------------------
//
//  Show Charger
//      This function will construct and display the charger information from the Charger[] datastore
//      The charger entery shown is at INDEX.
//      If 'Extended' is true, an alternative display with extended informaion will be shown.
//
//
//------------------------------------------------------------------------------------------------------


void    show_charger(unsigned index, bool extended) {

    int   i;
    
    if (index >= MAX_CHARGERS)          return;                     // Tisk-tisk, out of range.
    if (chargers[index].devID == 0xff)  return;                     // This is an EMPTY spot, or some other error!

    
    u8g2.clearBuffer();                   
    u8g2.setFont(u8g2_font_ncenB08_tr);  

    u8g2.setCursor(0, 12);                                   // Hum,  maybe later print putup some type of charger icon (Spinning wheel??).
                                                             // try and see if we can pull out the NMEA2000 name being sent out!  (Ala, "Port Alt");

    switch (chargers[index].chrgType) {
        case RVCDCct_Alternator:        u8g2.print("Alt ");
                                        break;

        case RVCDCct_Solar:             u8g2.print("Solar ");
                                        break;

        case RVCDCct_Wind:               u8g2.print("Wind ");
                                        break;

        case RVCDCct_FuelCell:          u8g2.print("Fuel ");
                                        break;

        case RVCDCct_Water:             u8g2.print("Water ");
                                        break;

        case RVCDCct_Generator:         u8g2.print("Gen ");
                                        break;

         case RVCDCct_ACSourced: 
         default:                       u8g2.print("Charger ");  
                                        break;
         }

    u8g2.print(chargers[index].instance);  

        

    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.setCursor(5,28);
    u8g2.print("Volts:   ");
    u8g2.print(chargers[index].Vdc.value,1);
    
    u8g2.setCursor(5,46);
    u8g2.print("Amps:   ");


    
    if ((millis() - chargers[index].Adc.timestamp) < CHARGER_STATUS_TIMEOUT) 
        u8g2.print(chargers[index].Adc.value,1);
    else
        u8g2.print("   ---");





   
    if ((millis() - chargers[index].PerMax.timestamp) < CHARGER_CONFIG_TIMEOUT) {
        u8g2.drawFrame(115, 5,10,50);
        u8g2.drawBox  (115, (5 +(50-(chargers[index].PerMax.value*50/100))),
                        10,         (chargers[index].PerMax.value*50/100));
        u8g2.setFont(u8g2_font_5x7_tr);   
        u8g2.drawStr(118,64,"%");
        }
   
/*
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(6,64);  
    if ((millis() - chargers[index].PerMax.timestamp) < CHARGER_CONFIG_TIMEOUT) {
        u8g2.print(chargers[index].PerMax.value);                           //  % util is partof charger_status1, and only comes every 5 seconds..
        u8g2.print("%");
        }

*/
    

    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(10,64);
    switch (chargers[index].state) {
        case RVCDCbcm_Bulk:         u8g2.print("Bulk");
                                    break;
                         
        
        case RVCDCbcm_Absorption:   u8g2.print("Abs");
                                    break;
                         
           
        case RVCDCbcm_Overcharge:   u8g2.print("OvrCh");
                                    break;
                         
        case RVCDCbcm_Float:        u8g2.print("Float");
                                    break;
                         
        case  RVCDCbcm_Equalize:    u8g2.print("Equal");
                                    break;
                         
        
        case RVCDCbcm_CVCC:         u8g2.print("CVCC");
                                    break;
                         
        case RVCDCbcm_Disabled: 
        default:                    u8g2.print("Off");
                                    break;             
    }
   



    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setCursor(78,64);   
    if ((millis() - chargers[index].temp.timestamp) < CHARGER_STATUS_TIMEOUT)   
        u8g2.print(chargers[index].temp.value);
    else 
        u8g2.print("-- ");
  
    u8g2.setFont(u8g2_font_5x7_tr);  
    u8g2.print("C");

    




    
    u8g2.sendBuffer();                    // transfer internal memory to the display
  
}




//------------------------------------------------------------------------------------------------------
//
//  Show Charger at Fault
//      This function will construct and display the charger FAULT information from the Charger[] datastore
//      The charger entery shown is at INDEX.
//      If 'Extended' is true, an alternative display with extended informaion will be shown.
//
//
//------------------------------------------------------------------------------------------------------


void show_charger_fault(unsigned index, bool extended) {

    if (index >= MAX_CHARGERS)          return;                   // Tisk-tisk, out of range.
    if (chargers[index].devID == 0xff)  return;                   // This is an EMPTY spot, or some other error!
         

    u8g2.clearBuffer();                   
    u8g2.setFont(u8g2_font_ncenB08_tr);  

    u8g2.setCursor(0, 10);                                   // Hum,  maybe later print putup some type of charger icon (Spinning wheel??).
                                                             // try and see if we can pull out the NMEA2000 name being sent out!  (Ala, "Port Alt");

  switch (chargers[index].chrgType) {
        case RVCDCct_Alternator:        u8g2.print("Alt ");
                                        break;

        case RVCDCct_Solar:             u8g2.print("Solar ");
                                        break;

        case RVCDCct_Wind:               u8g2.print("Wind ");
                                        break;

        case RVCDCct_FuelCell:          u8g2.print("Fuel ");
                                        break;

        case RVCDCct_Water:             u8g2.print("Water ");
                                        break;

        case RVCDCct_Generator:         u8g2.print("Gen ");
                                        break;

         case RVCDCct_ACSourced: 
         default:                       u8g2.print("Charger ");  
                                        break;
         }

    if (chargers[index].deviceName[0] != NULL)
       u8g2.print(chargers[index].deviceName);  
    else
        u8g2.print(chargers[index].instance);  

                           

    u8g2.setFont(u8g2_font_ncenB18_tr);
    u8g2.drawStr(10,40,"FAULT!");

    u8g2.setFont(u8g2_font_ncenB12_tr);
    u8g2.setCursor(40,64);
    u8g2.print("# ");
    u8g2.print(chargers[index].SPN.value);
    
    
    u8g2.sendBuffer();  

    showingError = true;         
  
}







    
   

void stuff_datapools(void) {
#ifdef SIMULATION

    // DUMMY UP AN ENTERY FOR NOW...

    int i;

    
            i = 0;                                                      // House Battery
            batteries[i].batID = 1;
            
            batteries[i].volts.value = 27.4;
            batteries[i].volts.timestamp = millis();
           
            batteries[i].amps.value = -213.75;
            batteries[i].amps.timestamp = millis();
      
            batteries[i].temp.value = 39;
            batteries[i].temp.timestamp = millis();
            
            batteries[i].SOC.value = 82;                                                  
            batteries[i].SOC.timestamp = millis();
            
            batteries[i].TR.value = 5000;                               ;                    
            batteries[i].TR.timestamp = millis();

            batteries[i].SOH.value  = 92;                                    
            batteries[i].SOH.timestamp = millis();
            
            batteries[i].CapRem.value = 780;                                           
            batteries[i].CapRem.timestamp = millis();
            
            batteries[i].CapRemPer.value = 45;                                          
            batteries[i].CapRemPer.timestamp = millis();



            i = 1;                                                      // Starter Battery
            batteries[i].batID = 2;
            
            batteries[i].volts.value = 13.3;
            batteries[i].volts.timestamp = millis();
           
            batteries[i].amps.timestamp = 0;       // No shunt on starter battery
      
            batteries[i].temp.value = 27;
            batteries[i].temp.timestamp = millis();
                                                            
            batteries[i].SOC.timestamp = 0;
            batteries[i].TR.timestamp = 0;                                   
            batteries[i].SOH.timestamp = 0;
            batteries[i].CapRem.timestamp = 0;
            batteries[i].CapRemPer.timestamp = 0;






   
            i = 0;                                                  // Port Alt
            chargers[i].batID = 1;
            chargers[i].devID = 0x20;
            chargers[i].state = RVCDCbcm_Absorption;
            chargers[i].chrgType = RVCDCct_Alternator;
            chargers[i].instance = 1; 
            
            chargers[i].Vdc.value = 29.2;
            chargers[i].Vdc.timestamp = millis();
           
            chargers[i].Adc.value = 78.2;
            chargers[i].Adc.timestamp = millis();
      
            chargers[i].temp.value = 57;
            chargers[i].temp.timestamp = millis();
            
            chargers[i].PerMax.value = 82;                                                  
            chargers[i].PerMax.timestamp = millis();




            i = 1;                                                  // Starboard Alt
            chargers[i].batID = 1;
            chargers[i].devID = 0x21;
            chargers[i].state = RVCDCbcm_Absorption;
            chargers[i].chrgType = RVCDCct_Alternator;
            chargers[i].instance = 2; 
            
            chargers[i].Vdc.value = 29.4;
            chargers[i].Vdc.timestamp = millis();
           
            chargers[i].Adc.value = 94.0;
            chargers[i].Adc.timestamp = millis();
      
            chargers[i].temp.value = 66;
            chargers[i].temp.timestamp = millis();
            
            chargers[i].PerMax.value = 97;                                                  
            chargers[i].PerMax.timestamp = millis();


            i = 2;                                                  // Shore power charger
            chargers[i].batID = 1;
            chargers[i].devID = 0x22;
            chargers[i].state = RVCDCbcm_Absorption;
            chargers[i].chrgType = RVCDCct_ACSourced;
            chargers[i].instance = 1; 
            
            chargers[i].Vdc.value = 29.4;
            chargers[i].Vdc.timestamp = millis();
           
            chargers[i].Adc.value = 52.1;
            chargers[i].Adc.timestamp = millis();
      
            chargers[i].temp.value = 22;
            chargers[i].temp.timestamp = millis();
            
            chargers[i].PerMax.value = 80;                                                  
            chargers[i].PerMax.timestamp = millis();

#endif
}






