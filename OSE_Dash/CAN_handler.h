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


#ifndef _CAN_HANDLER_H_
#define _CAN_HANDLER_H_



//  Prototypes
bool  init_CAN(void);
void  process_CAN_Messages(void);  
void  init_datastores(void);
void  purge_datastash(void);
void  CAN_status_request(void);
void  CAN_sleep(bool sleep);





#endif   /* _CAN_HANDLER_H_ */

