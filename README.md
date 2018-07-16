# OSEnergy Dash Display

#####A 2" Dash Display for the VSR Alternator Regulator, and other OSEnergy complaint devices.

The OSEnergy Dashboard Display is a combination of hardware and software and designed to provide  information  of your battery and its associated charging sources.  Usable with any OSEnergy compliant device (such as the [VSR Alternator Regulator](http://arduinoalternatorregulator.blogspot.com/) ), this simple gauge will monitor the communications bus providing you with real-time updates of any active devices.



<br><br>
###How to Purchase and Install
The OSEnergy-Dash-Display source code has been proofed on the PUT NAME HERE AS WELL AS LINK open xxxx.  You can purchase a preassembled version complete with firmware installed here: link-here.

Insert Photo of finished device



<br><br>
###Usage:

Once installed and connected, the Dash Display will monitor the CAN communications bus looking for battery information as well as any chargers associated with that battery.  Up to 4x different batteries may be monitored at one time, as well as up to 10x different charging sources.  When 1st powered up the display will show the 'No batteries Found' message; as battery information is found it will change the display to reflect the battery as well as any charging source associated with that battery.



The PUT NAME HERE display hardware has 3 touch-pad buttons along the lower edge of the OLED display.  Pressing the LEFT button will cycle between battery information and its associated charging sources.  Pressing the RIGHT button will cycle between different batteries in your system.  After a period of inactivity the display may darken.  You can awaken the dash display by pressing the CENTER button

<br>
###Information displayed:
#####The following battery information may be displayed:
- Battery Name / ID number
- Battery Voltage, Current, Temperature
- Battery SOC

 ----- PUT INSOME SAMPLE PHOTOS

<br>
#####The following Charging Source information may be displayed:
- Voltage at charging source
- Current being delivered by charging source
- Temperature of charging source
- % capability of charging source presently be utilized.
- Charging mode

 ----- PUT INSOME SAMPLE PHOTOS

Note that not all information may be available, depending on the capabilities of the associated device(s) installed in your system.  For example, if you only have a VSR Alternator Regulator installed battery SOC will not be shown.  To see Battery SOC you will need to install something which actually tracks SOC over the long term, such as a proper Battery Monitor or BMS.

<br><br>
####VSR Alternator Regulator Configuration Hints and notes:
- The  [VSR Alternator Regulator](http://arduinoalternatorregulator.blogspot.com/) needs to be configured to explicitly tell it its current shunt is located AT the battery by using the $CCN: command (Providing it is located at the battery).  Otherwise current will be recognized on the Alternator screen instead of the Battery screen.
- Generation 3 of the VSR Alternator Regulator does not supply power via the CAT-5 cable.  You will need to connect the external power wires on the display.  Generation 3B and beyond have the ability to provide power to the CAN CAT-5 cable directly, and no addition power wiring is needed.
- Update the VSR Alternator Regulator firmware  to version 1.3.1 or greater to maximize the capability of the Dash Display.


  



