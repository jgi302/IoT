/*

    Copyright (C) 2017 Libelium Comunicaciones Distribuidas S.L.
   http://www.libelium.com

    By using it you accept the MySignals Terms and Conditions.
    You can find them at: http://libelium.com/legal

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Version:           2.0
    Design:            David Gascon
    Implementation:    Luis Martin / Victor Boria
*/

#include <MySignals.h>
#include <MySignals_BLE.h>


// Write here the MAC address of BLE device to find
char MAC_BUTTON[14] = "952B00FA58FC";

uint8_t available_button = 0;
uint8_t connected_button = 0;


uint16_t button_times = 0;

#define BUTTON_DETECTION_HANDLE 54
#define BUTTON_ALARM_HANDLE 37


void setup()
{
  MySignals.begin();

  Serial.begin(115200);

  MySignals.initSensorUART();
  MySignals.enableSensorUART(BLE);

  //Enable BLE module power -> bit6: 1
  bitSet(MySignals.expanderState, EXP_BLE_POWER);
  MySignals.expanderWrite(MySignals.expanderState);

  //Enable BLE UART flow control -> bit5: 0
  bitClear(MySignals.expanderState, EXP_BLE_FLOW_CONTROL);
  MySignals.expanderWrite(MySignals.expanderState);


  //Disable BLE module power -> bit6: 0
  bitClear(MySignals.expanderState, EXP_BLE_POWER);
  MySignals.expanderWrite(MySignals.expanderState);

  delay(500);

  //Enable BLE module power -> bit6: 1
  bitSet(MySignals.expanderState, EXP_BLE_POWER);
  MySignals.expanderWrite(MySignals.expanderState);
  delay(1000);
  
  MySignals_BLE.initialize_BLE_values();


  if (MySignals_BLE.initModule() == 1)
  {

    if (MySignals_BLE.sayHello() == 1)
    {
      MySignals.println("BLE init ok");
    }
    else
    {
      MySignals.println("BLE init fail");

      while (1)
      {
      };
    }
  }
  else
  {
    MySignals.println("BLE init fail");

    while (1)
    {
    };
  }

}

void loop()
{
  available_button = MySignals_BLE.scanDevice(MAC_BUTTON, 1000, TX_POWER_MAX);

  if(available_button == 0 || available_button == 1)
  {
    MySignals.disableMuxUART();
    Serial.print(F("Button available:"));
    Serial.println(available_button);
    MySignals.enableMuxUART();
  }

  
  if (available_button == 1)
  {
    if (MySignals_BLE.connectDirect(MAC_BUTTON) == 1)
    {
      connected_button = 1;

      MySignals.disableMuxUART();
      Serial.println(F("Connected"));
      MySignals.enableMuxUART();

      delay(1000);

   

      char attributeData[1] =
      {
        0x01
      };

      if (MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle, BUTTON_DETECTION_HANDLE, attributeData, 1) == 0)
      {
       
        bool stop_alarm_press = 0;
        unsigned long previous = millis();
        do
        {

          
          if (MySignals_BLE.waitEvent(1000) == BLE_EVENT_ATTCLIENT_ATTRIBUTE_VALUE)
          {

            if ((MySignals_BLE.event[9] == 1) && (stop_alarm_press == 0))
            {
              stop_alarm_press = 1;
              
              MySignals.disableMuxUART();
              Serial.print(F("Pressed:"));
              Serial.println(button_times);
              MySignals.enableMuxUART();
              
              button_times++;

              char attributeData2[1] =
              {
                0x02
              };

              if (MySignals_BLE.attributeWrite(MySignals_BLE.connection_handle,  BUTTON_ALARM_HANDLE, attributeData2, 1) == 0)
              {
                MySignals.println("Alarm");
              }
              
            }
            else if ((MySignals_BLE.event[9] == 1) && (stop_alarm_press == 1))
            {
              
               stop_alarm_press = 0;
            }
          }

        }
        while ((connected_button == 1));

        connected_button = 0;

      }
      else
      {
        MySignals.println("Error subscribing");
      }
    }
    else
    {
      connected_button = 0;

      MySignals.println("Not Connected");
    }
  }
  else if (available_button == 0)
  {
    //Do nothing
  }
 
  delay(1000);
}


