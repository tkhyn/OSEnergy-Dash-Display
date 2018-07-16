//      CAN_handler.cpp
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
#include "CAN_handler.h"




#include <avr_can.h>                                        // https://github.com/thomasonw
#include <RVCMessages.h>                                    // https://github.com/thomasonw



    typedef struct {
        uint32_t PGN;
        void         (*Receiver)   (const tN2kMsg &N2kMsg);
        } tCANHandlers;

    void RVCDCStatus1_handler  (const tN2kMsg &N2kMsg);
    void RVCDCStatus2_handler  (const tN2kMsg &N2kMsg);
    void RVCDCStatus3_handler  (const tN2kMsg &N2kMsg);
    void RVCDCStatus4_handler  (const tN2kMsg &N2kMsg);
    void RVCDCStatus5_handler  (const tN2kMsg &N2kMsg);
    void RVCDCStatus6_handler  (const tN2kMsg &N2kMsg);
    void RVCDCDisconnectStatus_handler(const tN2kMsg &N2kMsg);
    void RVCChrgStat_handler   (const tN2kMsg &N2kMsg);
    void RVCChrgStat2_handler  (const tN2kMsg &N2kMsg);
    void ISODiagnostics_handler(const tN2kMsg &N2kMsg);

            
    tCANHandlers CANHandlers[]={
        {0x1FFFD,&RVCDCStatus1_handler},
        {0x1FFFC,&RVCDCStatus2_handler},
        {0x1FFFB,&RVCDCStatus3_handler},
        {0x1FEC9,&RVCDCStatus4_handler},
        {0x1FEC8,&RVCDCStatus5_handler},
        {0x1FEC7,&RVCDCStatus6_handler},
        {0x1FED0,&RVCDCDisconnectStatus_handler},
        {0x1FFC7,&RVCChrgStat_handler},
        {0x1FEA3,&RVCChrgStat2_handler},            // Official PGN number
        {0x1FF9D,&RVCChrgStat2_handler},            // Old 'Temp' one used until we were assigned the real number...  (For backwars compatability with existing field devices.)
        {0x1FECA,&ISODiagnostics_handler},
        
        {0,NULL}                                                                               // ----PGN of 0 indicates end of table----
        };
            








//------------------------------------------------------------------------------------------------------
//
//  Initialize CAN 
//              This function will startup the CAN subsystem.
//
//
//
//
//
//
//
//------------------------------------------------------------------------------------------------------

bool init_CAN(void) {
    
    int i;
    


    //------  Setup the CAN to receive messages
  

    CAN_sleep(false);                                           // Make sure CAN sybsystem is awake
    
    Can0.begin(CAN_BPS_250K);                                   // Initialize CAN0, Set the proper baud rates here
    
    for (i = 0; i < 4; i++)                                     //Set up 4 of the 5x boxes for extended, no filtering
        Can0.setRXFilter(i, 0, 0, true);
    
    
    while (Can0.setRXFilter(0, 0, false) > 0) ;                 // Set up the remaining MOB for standard messages.  
                                                                // (Just in case, but not sure any std messages are ever used.
                                                                
    init_datastores();                                          // Clear out the datastores, getting them ready to recevie data.
    CAN_status_request();                                       // Send out requests for data, in case we need to wake up the CAN bus!
        
}







//------------------------------------------------------------------------------------------------------
//
//  CAN Sleep
//      Controls power mode of the CAN subsystem.   Will place CAN subsystem into Standby mode
//      as well as the transeiver.   While in this mode the CAN subsystem will not be able to 
//      acatualy receive a CAN message, but will execute a packet-noted IRQ when CAN bus traffic
//      is detected.  That in tern can be used to wake up the CPU which will then wake-up the CAN
//      subsystem.

//
//
//
//------------------------------------------------------------------------------------------------------


void    CAN_sleep(bool sleep) {


   pinMode(CAN_STBY_PORT, OUTPUT);

 
    if (sleep) {

        //-- Put the CAN subsystem into standby mode  (Making sure IRQs are enabled)
 
         
        digitalWrite(CAN_STBY_PORT, HIGH);                              // Put transeiver into standby mode.

                                                        // Make sure the CAN IT interupt is enabled (To allow an IRQ upon CAN traffic being noted, to wake up the CPU)
                                                        // Is there someting in the existing AVR_CAN lib that addresses power mode and state?  We do nto want to interfeer
                                                        // wiht IRQ handing in that lib...

                                                        
        
               
    } else {


 
        digitalWrite(CAN_STBY_PORT, LOW);                               // Put transeiver into full-power mode.
        
        delay(100);                                                     // Give some time for the sybsystem to wake up.  
                                                    //!! HEY!!  DO I NEED THIS DELAY???  IS IT THE CORRECT VALUE???
                                                    
    }
    

}






//------------------------------------------------------------------------------------------------------
//
//  Initialize Datastore arrays
//      This fucntion will clear out any existing data contained in the two datastores / datastashes
//      (Chargers and Batteries).  preparing them to be refreshed as new data arrives via the CAN
//
//
//------------------------------------------------------------------------------------------------------

void init_datastores(void) {                                         
    
    int i;

    for (i=0; i<MAX_BATTERIES; i++)                             // Battery ID in the battery data-store array = 0 --> No in use.
        batteries[i].batID = 0xff;                 
   
    for (i=0; i<MAX_CHARGERS; i++) {                            // CAN node address (ID) charger data-store array = 0xFF --> Not in use.
        chargers[i].devID = 0xff;   
        chargers[i].errorRed = false;
        chargers[i].errorYellow = false;
        chargers[i].deviceName[0] = NULL;
        }

    CAN_status_request();                                       // Broadcast a request for status, see if we need to wake someone up.

    batFocus = -1;
    chargerFocus = -1;                                          // Reset indexes of what to show.

}







//------------------------------------------------------------------------------------------------------
//
//  Purge Datastash
//      Scans teh battery and charger datastash looking for enteries with stale / timesout data.
//      Those found to have expired will have that entery released.
//
//      For Battereis, critical data is VBat, we need AT LEAST that to talk about a battery.
//      For Chargers, the critical data is  Vdc  (Chargers voltage)
//
//
//------------------------------------------------------------------------------------------------------

void purge_datastash(void) {
    
    int i;


    for (i=0; i<MAX_BATTERIES; i++) {
        if ((millis() - batteries[i].volts.timestamp) >= BAT_STATUS_TIMEOUT)
            batteries[i].batID = 0xff;                                          // Stale battery, release this entery
    }




   for (i=0; i<MAX_CHARGERS; i++) {
      if (((millis() - chargers[i].Vdc.timestamp) >= CHARGER_STATUS_TIMEOUT) &&
          ((millis() - chargers[i].SPN.timestamp) >= CHARGER_STATUS_TIMEOUT)) {
          chargers[i].devID = 0xff;                                             // Come on, any charger in OSEnergy should at least knows its own voltage!
          chargers[i].SPN.value = 0;                                            //  (Unless it is in a fault condition)
          }
     }


}





//------------------------------------------------------------------------------------------------------
//
//  CAN Status Request
//      This fucntion will send out on the CAN bus requests for Battery and Charger Status.
//      Idea is if we have just woken up and want to see what is happening we might have to wake up
//      other devices on the CAN bus.
//
//
//
//
//------------------------------------------------------------------------------------------------------

void CAN_status_request(void) {


     CAN_FRAME packet;

    //-- Dummy up a Request packet.  Note that we do not really have an addess, so will use '0xff', but ask for global reply and hope nodes respond.
    //
    
    packet.extended = true;
    packet.id             = 0xFF;                                                   // We do not have an offical J1939 address, so just using 0xFF as this will be a global request.
    packet.id            |=  (0xEAFF << 8);                                         // Request for DGN, broadcast reply to all.
    packet.id            |=  (6      << 23);                                        // Prioity = 6 (By definition)
    packet.length         =  3;
    packet.data.bytes[0]  =  0xFD;                                                  // Send out request for DC Status 1 (0x1FFFD), that should get things rolling.
    packet.data.bytes[1]  =  0xFF;
    packet.data.bytes[2]  =  0x01;

    Can0.sendFrame(packet);            

}




//------------------------------------------------------------------------------------------------------
//
//  Process CAN Messages
//              This function will check to see if there are any incomming CAN messages, and if do will
//              recevie them and place the data into the datapool array.
//
//
//
//
//!! HEY!!  Add capability to request (and recevie) NEMA2000 PGN126996 (Product Informaiton).
//          In there is the device name string suer has set!
//          But do note, this is a multi-packet message, so some handler will need to be added....
//!! HEY!!  Maybe a BETTER alterantive is to fix the regulator so taht it can send multi-packet J1939 messages,
//          then send out a proper RV-C 65259 (FEEBh) "Product identification message", using the MODEL as
//          the device name.
//
//------------------------------------------------------------------------------------------------------

void process_CAN_Messages(void) {

    CAN_FRAME packet;
    tN2kMsg   N2kMsg;
    int i;

  
                
    if (Can0.rx_avail()) {                                                                          // See if there are any CAN messages to receive.  
        if ((Can0.read(packet)) && (packet.extended)){                                              // New message, now see where it needs to go. 

            //--  Need to transfer the raw CAN packet into a N2K packet, for the rest of the parsers to be able to work.
            
            N2kMsg.Source   = (uint8_t)    packet.id;                                               // Lower 8 bits (0..7)= source ID of this message
            N2kMsg.PGN      = (uint32_t) ((packet.id >> 8) & 0x1FFFF);                              // Next 23 bits are the PGN
            N2kMsg.DataLen  = packet.length;
            for (i=0; i<8; i++) 
                N2kMsg.Data[i] = packet.data.bytes[i];
                


            //---  Now look for a handler for this message and pass it on to the approperate handler.
                     
            for (i=0; CANHandlers[i].PGN!=0 && !(N2kMsg.PGN==CANHandlers[i].PGN); i++);             // Scan the vector table, looking for a matching PGN # (Or the last entery stopper value of 0)

            if ((CANHandlers[i].PGN!=0) &&   (CANHandlers[i].Receiver != NULL)) {                   // If a matching PGN was found, (Not at end of table), and there is a function for it..
                CANHandlers[i].Receiver(N2kMsg);                                                    // Well, looks like we can support this incoming message.  Call the handler.
                last_CAN_activity = millis();                                                       // And note when this was, to help manage sleep-timeouts. 
 
                #ifdef DEBUG
                    Serial.print("Proccessing CAN message PGN: ");
                    Serial.print(N2kMsg.PGN,HEX);
                    Serial.print("  Priority:" );
                    Serial.print(N2kMsg.Priority);
                    Serial.println();
                    
                    #endif
                                   
            }
        }
    }      
}








//***************************************************
//  Scan the Battery Stash table, looking for a matching battery ID (or instance).
//  Returns index of found entery, even if 1st time used.
//  Returns -1 if table is full..


int findBatStash(uint8_t batID) {

    int j;
    for (j=0; j<MAX_BATTERIES; j++) {                                                           // Look for a matching battery ID in the battery data-store array.
        if (batteries[j].batID == batID)      break;                                            // Found a match!
        if (batteries[j].batID == 0xff)       break;                                            // Found an empty table entery.
        }
        
    if (j >= MAX_BATTERIES) 
        return(-1);                                                                             // Battery datastore array is out of room, too many batteries!

        
    batteries[j].batID = batID;                                                                 // Stash the battery instance, is this might be a 1st time this table entery is used.
    return (j);

}




//***************************************************
//  Scan the Charger Stash table, looking for a matching device ID.
//  Returns index of found entery, even if 1st time used.
//  Returns -1 if table is full..


int findChargerStash(const tN2kMsg N2kMsg) {

    int j;
    for (j=0; j<MAX_CHARGERS; j++) {                                                            // Look for a matching battery ID in the battery data-store array.
        if (chargers[j].devID == N2kMsg.Source)  break;                                         // Found a match!
        if (chargers[j].devID == 0xff)           break;                                         // Found an empty table entery.
        }
        
    if (j >= MAX_CHARGERS) 
        return(-1);                                                                             // Chargers datastore array is out of room, too many batteries!

        
    chargers[j].devID   = N2kMsg.Source;                                                        // Stash the Charger CAN ID, is this might be a 1st time this table entery is used.
    
    return (j);
}








//*****************************************************************************
void RVCDCStatus1_handler(const tN2kMsg &N2kMsg){                                           // DC Status 1  (Remote Battery Current)        

    int i;
    
    uint8_t  instance;                                                                      
    uint8_t  devPri;                                                                    
    uint16_t Vdc;
    uint32_t Adc;


    if (ParseRVCDCSourceStatus1(N2kMsg, instance, devPri, Vdc, Adc)) {                      // Received a valid CAN message from someone


        i = findBatStash(instance);
        if (i == -1) return;                                                                // The battery datastash is filled up.
        


        //--        OK, have a place to put this info into, now -  pull out the new data we received and put it into the datastore.

        if ((Adc != N2kUInt32NA)                            &&                              // And do they actually have a current value to tell us?  --AND--
            ((devPri  >= batteries[i].amps.devPri) ||                                       //         Is this message from a like or higher-priority device then we already have stashed away?
             (millis() - batteries[i].amps.timestamp) >= BAT_STATUS_TIMEOUT)) {             //   --OR-- has it been more then 1 second sense we last heard form them?  (This should be send out every 500mS)
            batteries[i].amps.value = (float)((int32_t)(Adc - 0x77359400)) * 0.001;         // Yup - save this info
            batteries[i].amps.timestamp = millis();
            batteries[i].amps.devID = N2kMsg.Source;                                       
            batteries[i].amps.devPri = devPri; 
            }


        if ((Vdc != N2kUInt16NA)                           &&                            
            ((devPri  >= batteries[i].volts.devPri) ||                         
             (millis() - batteries[i].volts.timestamp) >= BAT_STATUS_TIMEOUT)) {          
            batteries[i].volts.value = ((float) Vdc) / 20.0;        
            batteries[i].volts.timestamp = millis();
            batteries[i].volts.devID = N2kMsg.Source;                                   
            batteries[i].volts.devPri = devPri; 
            }
    }
}






//*****************************************************************************
void RVCDCStatus2_handler  (const tN2kMsg &N2kMsg){

    int i;
      
    uint8_t  instance;
    uint8_t  devPri;
    int16_t  sourceTemp;
    uint8_t  SOC;
    uint16_t TR;


    if (ParseRVCDCSourceStatus2(N2kMsg, instance, devPri, sourceTemp, SOC, TR)) {           // Received a valid CAN message from someone
   
        i = findBatStash(instance);
        if (i == -1) return;                                                                // The battery datastash is filled up.


        //--        OK, have a place to put this info into, now -  pull out the new data we received and put it into the datastore.

        if ((sourceTemp != N2kInt16NA)                      &&                              // Do they actually have a battery temperature  value to tell us?  --AND--
            ((devPri  >= batteries[i].temp.devPri) ||                                       //         Is this message from a like or higher-priority device then we already have stashed away?
             (millis() - batteries[i].temp.timestamp) >= BAT_STATUS_TIMEOUT)) {             //   --OR-- has it been more then 1 second sense we last heard form them?  (This should be send out every 500mS)
            batteries[i].temp.value = (int)((float)sourceTemp * 0.03125);                   // Yup - save this info
            batteries[i].temp.timestamp = millis();
            batteries[i].temp.devID = N2kMsg.Source;                                       
            batteries[i].temp.devPri = devPri; 
            }


        if ((SOC != N2kUInt8NA)                             &&                              // Do they actually have a battery SOC (State of Charge)  value to tell us?  --AND--
            ((devPri  >= batteries[i].SOC.devPri) ||                                        //         Is this message from a like or higher-priority device then we already have stashed away?
             (millis() - batteries[i].SOC.timestamp) >= BAT_STATUS_TIMEOUT)) {              //   --OR-- has it been more then 1 second sense we last heard form them?  (This should be send out every 500mS)
            batteries[i].SOC.value = SOC;                                                   // Yup - save this info
            batteries[i].SOC.timestamp = millis();
            batteries[i].SOC.devID = N2kMsg.Source;                                       
            batteries[i].SOC.devPri = devPri; 
            }



   


        if ((TR != N2kUInt16NA)                     &&                                      // Do they actually have a battery TR (Time Remaining till Discharge) value to tell us?  --AND--
            ((devPri  >= batteries[i].TR.devPri) ||                                         //         Is this message from a like or higher-priority device then we already have stashed away?
             (millis() - batteries[i].TR.timestamp) >= BAT_STATUS_TIMEOUT)) {               //   --OR-- has it been more then 1 second sense we last heard form them?  (This should be send out every 500mS)
            batteries[i].TR.value = TR;                               ;                     // Yup - save this info
            batteries[i].TR.timestamp = millis();
            batteries[i].TR.devID = N2kMsg.Source;                                       
            batteries[i].TR.devPri = devPri; 
            }

    }

    
}



//*****************************************************************************
void RVCDCStatus3_handler  (const tN2kMsg &N2kMsg){



    int i;
      
    uint8_t   instance;
    uint8_t   devPri; 
    uint8_t   SOH;
    uint16_t  CapRem;
    uint8_t   CapRemPer;
    uint16_t  Ripple;
    
  
    if (ParseRVCDCSourceStatus3(N2kMsg, instance, devPri, SOH, CapRem, CapRemPer, Ripple)) {           // Received a valid CAN message from someone

        
        i = findBatStash(instance);
        if (i == -1) return;                                                                // The battery datastash is filled up.


        //--        OK, have a place to put this info into, now -  pull out the new data we received and put it into the datastore.

        if ((SOH != N2kInt8NA)                      &&                                      // Do they actually have a battery temperature  value to tell us?  --AND--
            ((devPri  >= batteries[i].SOH.devPri) ||                                        //         Is this message from a like or higher-priority device then we already have stashed away?
             (millis() - batteries[i].SOH.timestamp) >= BAT_STATUS_TIMEOUT)) {              //   --OR-- has it been more then 1 second sense we last heard form them?  (This should be send out every 500mS)
            batteries[i].SOH.value  = SOH;                                                  // Yup - save this info
            batteries[i].SOH.timestamp = millis();
            batteries[i].SOH.devID  = N2kMsg.Source;                                       
            batteries[i].SOH.devPri = devPri; 
            }




        if ((CapRem != N2kInt16NA)                         &&                              
            ((devPri  >= batteries[i].CapRem.devPri) ||                                     
             (millis() - batteries[i].CapRem.timestamp) >= BAT_STATUS_TIMEOUT)) {                       
            batteries[i].CapRem.value = CapRem;                                           
            batteries[i].CapRem.timestamp = millis();
            batteries[i].CapRem.devID = N2kMsg.Source;                                       
            batteries[i].CapRem.devPri = devPri; 
            }




        if ((CapRemPer != N2kInt8NA)                         &&                              
            ((devPri  >= batteries[i].CapRemPer.devPri) ||                                     
             (millis() - batteries[i].CapRemPer.timestamp) >= BAT_STATUS_TIMEOUT)) {                       
            batteries[i].CapRemPer.value = CapRemPer;                                           
            batteries[i].CapRemPer.timestamp = millis();
            batteries[i].CapRemPer.devID = N2kMsg.Source;                                       
            batteries[i].CapRemPer.devPri = devPri; 
            }

    }
    
    
}





//*****************************************************************************
void RVCDCStatus4_handler  (const tN2kMsg &N2kMsg){

//  Simple display does not need to show targets for battery, nor details of battery type.

    
}





//*****************************************************************************
void RVCDCStatus5_handler  (const tN2kMsg &N2kMsg){

 //  Simple display does not need to show higher precision of VBat???   
   
}





//*****************************************************************************
void RVCDCStatus6_handler  (const tN2kMsg &N2kMsg){

 //  Simple display does not need to show safety cutoff limits for battery  (High voltage, low voltage limits)

}






//*****************************************************************************
void RVCDCDisconnectStatus_handler(const tN2kMsg &N2kMsg){

// Hum, maybe the simmply display can just time out and stop showing info as a result of devices stopping sending status out!
//!! HEY!!   But maybe also we can show DISCONNECTED on the display.  
//  Note this message is only sent upon a status change, it could be lost if we are asleep.
    
}





//*****************************************************************************
void RVCChrgStat_handler   (const tN2kMsg &N2kMsg){

    int i;
  
    tRVCChrgType ChrgType;
    uint8_t     Instance;
    uint16_t    CVdc;
    uint16_t    CAdc;
    uint8_t     PerMax;    
    tRVCBatChrgMode State; 
    bool        EnableAtPO; 
    bool        AutoRechg;
    tRVCChrgForceChrg ForcedChrg; 




    if (ParseRVCChargerStatus(N2kMsg, ChrgType, Instance, CVdc, CAdc, PerMax,State,EnableAtPO, AutoRechg, ForcedChrg)) {       // Received a valid CAN message from someone


        i = findChargerStash(N2kMsg);
        if (i == -1) return;                                                                // The charger datastash is filled up.
        

 
        //--        OK, have a place to put this info into, now -  pull out the new data we received and put it into the datastore.
        chargers[i].state    = State;
        chargers[i].chrgType = ChrgType;
        chargers[i].instance = Instance;

    
        if (PerMax != N2kUInt8NA) {                                                         //   We do not worry about timestamp on Charger, as each charger has its own entery.
            chargers[i].PerMax.value     = PerMax;
            chargers[i].PerMax.timestamp = millis();
            }
    }
}





//*****************************************************************************
void RVCChrgStat2_handler  (const tN2kMsg &N2kMsg){

    int i;

     tRVCChrgType ChrgType;
     uint8_t    ChrgInst; 
     uint8_t    DCInst; 
     uint8_t    DevPri;
     uint16_t   Vdc; 
     uint16_t   Adc; 
     uint8_t    Temp;

    
     if (ParseRVCChargerStatus2(N2kMsg, ChrgType, ChrgInst, DCInst, DevPri, Vdc, Adc, Temp)) {


        i = findChargerStash(N2kMsg);
        if (i == -1) return;                                                                // The charger datastash is filled up.
        
        chargers[i].batID = DCInst;


        //--        OK, have a place to put this info into, now -  pull out the new data we received and put it into the datastore.

        if (Vdc != N2kUInt16NA) {      
            chargers[i].Vdc.value     = (float)Vdc / 20.0;
            chargers[i].Vdc.timestamp = millis();
            }
            

        if (Adc != N2kUInt16NA) {      
            chargers[i].Adc.value     = (float)((int)(Adc - 0x7D00U)) / 20.0;
            chargers[i].Adc.timestamp = millis();
            }


        if (Temp != N2kInt8NA) {      
            chargers[i].temp.value     = Temp;
            chargers[i].temp.timestamp = millis();
            }

    }


}






//*****************************************************************************
void ISODiagnostics_handler(const tN2kMsg &N2kMsg) {


    int i;

    bool On;
    bool Active; 
    bool Red; 
    bool Yellow; 
    uint8_t DSA;           
    uint32_t SPN;
    tISOFMIType FMI;
    uint8_t Count;
    uint8_t DSA_ext;
    uint8_t Bank;        



    if (ParseISODiagnosticsMessage(N2kMsg, On, Active, Red, Yellow, DSA,SPN, FMI, Count, DSA_ext, Bank)) {

        i = findChargerStash(N2kMsg);
        if (i == -1) return;                                                                // The charger datastash is filled up.
        
        chargers[i].SPN.value = (SPN & 0x7FFF);                                             // Strip off the higher bits to get the basic error code #
        chargers[i].SPN.timestamp = millis();

        chargers[i].errorRed    = Red;
        chargers[i].errorYellow = Yellow;
        }
    
}

    






