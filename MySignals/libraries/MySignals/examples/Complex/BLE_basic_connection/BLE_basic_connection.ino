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


#include <Adafruit_GFX_AS.h>
#include <Adafruit_ILI9341_AS.h>
#include <MySignals.h>
#include <MySignals_BLE.h>
#include <Wire.h>
#include <SPI.h>

char buffer_tft[30];

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);

unsigned long previous;


// Sensor list
bool selected_airflow;
bool selected_ecg;
bool selected_emg;
bool selected_gsr;
bool selected_position;
bool selected_snore;
bool selected_temp;
bool selected_spiro;
bool selected_eeg;
bool selected_spo2_uart;
bool selected_bp_uart;
bool selected_gluco_uart;
bool selected_scale_ble;
bool selected_spo2_ble;
bool selected_bp_ble;
bool selected_gluco_ble;

void setup()
{
  MySignals.begin();

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  //TFT message: Welcome to MySignals
  strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[0])));
  tft.drawString(buffer_tft, 0, 0, 2);

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

  //Clean the input serial buffer
  while (Serial.available() > 0 )
  {
    Serial.read();
  }



  if (MySignals_BLE.initModule() == 1)
  {

    if (MySignals_BLE.sayHello() == 1)
    {
      //TFT message: "BLE init ok";
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[1])));
      tft.drawString(buffer_tft, 0, 15, 2);
    }
    else
    {
      //TFT message:"BLE init fail"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[2])));
      tft.drawString(buffer_tft, 0, 15, 2);


      while (1)
      {
      };
    }
  }
  else
  {
    //TFT message: "BLE init fail"
    strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[2])));
    tft.drawString(buffer_tft, 0, 15, 2);

    while (1)
    {
    };
  }

}


void loop()
{

  //1. SET MODE: SLAVE (VISIBLE TO APP)
  while ((MySignals_BLE.ble_mode_flag == master_mode))
  {
    if (MySignals_BLE.setMode(slave_mode) == 0)
    {
      //TFT message:  "Slave mode ok";
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[3])));
      tft.drawString(buffer_tft, 0, 30, 2);


      MySignals_BLE.ble_mode_flag = slave_mode;
    }
    else
    {
      //TFT message:  "Slave mode fail"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[4])));
      tft.drawString(buffer_tft, 0, 30, 2);


      MySignals_BLE.hardwareReset();
      delay(100);
      MySignals_BLE.initialize_BLE_values();
    }
  }



  //2. SET BONDABLE MODE
  if (MySignals_BLE.bond_mode_and_mitm == 0)
  {
    if (MySignals_BLE.setBondableMode(BLE_ENABLE_BONDING) == 0)
    {

      //TFT message:  "Bondable mode ok"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[5])));
      tft.drawString(buffer_tft, 0, 45, 2);



      //3. SET SM PARAMETERS
      if (MySignals_BLE.setSMParameters(BLE_ENABLE_MITM) == 0)
      {
        //TFT message:  "SM parameters ok"
        strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[7])));
        tft.drawString(buffer_tft, 0, 60, 2);


        MySignals_BLE.bond_mode_and_mitm = 1;

      }
      else
      {
        //TFT message:  "SM parameters fail"
        strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[8])));
        tft.drawString(buffer_tft, 0, 60, 2);

        MySignals_BLE.hardwareReset();
        delay(100);
        MySignals_BLE.initialize_BLE_values();
      }
    }
    else
    {
      //TFT message:  "Bondable mode fail"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[6])));
      tft.drawString(buffer_tft, 0, 45, 2);

      MySignals_BLE.hardwareReset();
      delay(100);
      MySignals_BLE.initialize_BLE_values();
    }
  }



}
