//
//      CAN_handler.h
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
//              aint32_t with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//


#ifndef _OSE_DASH_H_
#define _OSE_DASH_H_


#include <stdint.h>
#include <RVCMessages.h>                                    // https://github.com/thomasonw




//#define  SIMULATION
//
#define  DEBUG




                //----  The following timeout values are used to invalidate data stored on the datastore arrays.
                //      They are also used to automatically decide what should (and should not) be displayed
                //
                
#define BAT_STATUS_TIMEOUT      1000UL                  // Battery Status info (Volts, Amps, Temp, etc)  should arrive every 500mS
#define CHARGER_CONFIG_TIMEOUT 10000UL                  // Charger config (including CHARGE_STATUS) comes every 5s, or on a change.
#define CHARGER_STATUS_TIMEOUT  1000UL                  // Charger status (e.g. CHARGE_STATUS2) comes every 500mS
#define CHARGER_FAULT_TIMEOUT   2000UL                  // Charger fault messaged come out every 1000mS while in fault condition.



#define DISPLAY_TIMEOUT         30000UL                 // If nothing happens (Charger CAN update received, user not pressed button) after 30 seconds,
                                                        // place display into low-power mode and go dark.                                                          
#define SLEEP_TIMEOUT         (10*60000UL)              // If no CAN or USER activity for 10 minutes, shut down the monitor and go into low-power SLEEP mode.
                                                        




            //---   Arduino 'Ports' and pin usages

#define CAN_STBY_PORT   A0
#define LED_RED_PORT    A3            
#define LED_GREEN_PORT   7
#define LED_BLUE_PORT   A2
#define KP_LEFT_PORT    A7
#define KP_CENTER_PORT  A6
#define KP_RIGHT_PORT   10

            // -----  If attached, the following IRQs are used to wake-up the display from a deep-sleep power-saving mode.
//#define KP_LEFT_IRQ     INT2
#define KP_CENTER_IRQ   INT2                        // Hardware at present only has CENTER key connected to INT capable port.
//#define KP_RIGHT_IRQ    INT2
//#define CAN_Rx_IRQ      INT2               






//---   Datastore / Data Stash tables.
//      As CAN informaiton is recevied, it will be parsed and placed into these tables.
//      The rest of the programm will scan the tables and update the display appropertly.


#define MAX_BATTERIES   4                                   // We will only track up to 4 batteries, or battery IDs
#define MAX_CHARGERS    8                                   // We will only track up to 8 chargers

//    META = common statstructure which is added to recevied values to trach who from, and when it arrived.  Allows for timeouts os stale data.
#define  META        uint8_t devID;    \                                      
                     uint8_t devPri;   \   
                     uint32_t timestamp;                                 
                                 

                      // CAN address (J1939 device address) that sent this data
                      // Priority of device that sent this informaiton
                      // Time this message was recevied.



                         
typedef struct {
    META
    float  value;  
    } tFloat;

typedef struct {
    META
    int  value;  
    } tInt;

typedef struct {
    META
    uint16_t  value;  
    } tUInt;


typedef struct {
    META
    uint32_t  value;  
    } tULong;


typedef struct {
    META
    uint8_t  value;  
    } tUByte;







typedef struct {   
    uint8_t batID;                                          // Which battery Number is this data assoicated with?  (OxFF = unused position)

    tFloat  volts;                               
    tFloat  amps;
    tInt    temp;                                           // in Degrees C
    tUByte  SOC;                                            // State Of Charge, % value from 0..100    
    tUInt   TR;                                             // Time Remaing till full discharge, in Minutes.
    tUByte  SOH;                                            // State of Health
    tUInt   CapRem;                                         // Capacity Remaining in Ahs
    tUByte  CapRemPer;                                      // % capacity Remaining.
    
    } tBatteries;






typedef struct {   
    uint8_t     devID;                                      // CAN address (J1939 device address) of this charger, this is used as the 'index key'  0xFF means space is not used.
    uint8_t     batID;                                      // Which battery is this associated with?
    tRVCBatChrgMode state;
    tRVCChrgType    chrgType;
    uint8_t         instance;
    char            deviceName[19];

    
    tFloat      Vdc;                                        // Voltage at charging device
    tFloat      Adc;                                        // Amperhage charging device is delivering
    tInt        temp;                                       // In degree C
    tUByte      PerMax;                                     // % of charger capaity currently being uses.  (How hard is it driving)

    tULong      SPN;                                        // Is the cahrger in FAULT confition of some type?  (0= no fault, else containes fault number)
    bool        errorRed;
    bool        errorYellow;
    
    } tChargers;





extern tBatteries  batteries[MAX_BATTERIES];
extern tChargers   chargers[MAX_CHARGERS];
extern int batFocus;   
extern int chargerFocus;

extern unsigned long last_CAN_activity;  
extern unsigned long last_USER_activity;  
        



#endif   /* _OSE_DASH_H_ */


