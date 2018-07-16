//      LED.cpp
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
#include "LED.h"
#include "Display.h"







void update_LED(void) {
   
    pinMode(LED_RED_PORT, OUTPUT);

    //---    Are there ANY chargers at Fault?  If so, we need to light the RED LED
    //          (Note how this LED will still be lit even if thee display is turned off and 
    //           the CPU put into sleep mode.  To prompt user to press a button and see what is
    //           up)
    //
    
    if (showingError == true)  
        digitalWrite(LED_RED_PORT, HIGH);  
    else
        digitalWrite(LED_RED_PORT, LOW);  

}

