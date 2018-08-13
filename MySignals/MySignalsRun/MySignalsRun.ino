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


/*
 * Includes for using tft screen and touch
 */
#include <Adafruit_GFX_AS.h>
#include <Adafruit_ILI9341_AS.h>
#include <UTouch.h>

/*
 * Includes to interact with MySignals HW 2.0
 */
#include <MySignals.h>
#include <Wire.h>
#include <SPI.h>
#include <MSPrint.h>

/*
 * TFT and Touch 
 */
Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC); // Interacts with tft screen
UTouch  myTouch(14,21,15,16,17); // Fetches touch data from screen
bool touch_down = false;         // If the touch screen is touched or not
int touch_x;                     // X coordinate on tft screen
int touch_y;                     // Y coordinate on tft screen

/*
 * Sensor status to displayed on tft screen
 * ON  = ON
 * OFF = OFF
 * DIS = DISABLED
 */
char on[] = "ON ";
char off[] = "OFF";
char dis[] = "DIS";

/*
 * Sensor types to be displayed on tft screen
 */
char spo2[] = "SPO2:";
char temp[] = "Temp:";
char air[] = "Airflow:";
char ECG[] = "ECG:";
char EMC[] = "EMG:";
char GSR[] = "GSR:";
char spo2alarm[] = "SPO2 Alarm:";
char bloodPressure[] = "BP:";

/*
 * Current status of each sensor, starts with all disabled
 */
bool SPO2_S = false;
bool TEMP_S = false;
bool AIR_S = false;
bool ECG_S = false;
bool EMC_S = false;
bool GSR_S = false;
bool SPO2A_S = false;
bool BP_S = false;

MSPrint Sensor;       // Has functions to get each sensors data
uint8_t count = 1;    // Used for calculating delay in sensor data fetching

void setup()
{
  Serial.begin(115200);

  // Start and initialize the MySignals HW 2.0
  MySignals.begin(); 
  MySignals.initSensorUART();

  // Initialize the tft screen
  tft.init();   
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  // Initialize the touch screen
  Wire.begin();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  /*
   * Draw intial 4x2 grid with sensor and sensor status
   */

  // Draw Vertial Lines
  tft.drawFastVLine(0, 0, 319, ILI9341_WHITE);
  tft.drawFastVLine(1, 0, 319, ILI9341_WHITE);
  tft.drawFastVLine(119, 0, 319, ILI9341_WHITE);
  tft.drawFastVLine(120, 0, 319, ILI9341_WHITE);
  tft.drawFastVLine(239, 0, 319, ILI9341_WHITE);
  tft.drawFastVLine(238, 0, 319, ILI9341_WHITE);

  // Draw Horizontal lines
  tft.drawFastHLine(0, 0, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 1, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 318, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 319, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 79, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 80, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 159, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 160, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 239, 239, ILI9341_WHITE);
  tft.drawFastHLine(0, 240, 239, ILI9341_WHITE);

  // Draw Sensor strings
  tft.drawString(spo2, 5, 5, 2);
  tft.drawString(temp, 5, 85, 2);
  tft.drawString(air, 5, 165, 2);
  tft.drawString(ECG, 5, 245, 2);
  tft.drawString(EMC, 126, 5, 2);
  tft.drawString(GSR, 126, 85, 2);
  tft.drawString(spo2alarm, 126, 165, 2);
  tft.drawString(bloodPressure, 126, 245, 2);

  // Draw Sensor status
  tft.drawString(off, 89, 5, 2);
  tft.drawString(off, 89, 85, 2);
  tft.drawString(off, 89, 165, 2);
  tft.drawString(off, 89, 245, 2);
  tft.drawString(off, 209, 5, 2);
  tft.drawString(off, 209, 85, 2);
  tft.drawString(off, 209, 165, 2);
  tft.drawString(dis, 209, 245, 2);

}

void loop()
{
  
  if (myTouch.dataAvailable() && !touch_down)
  {
    touch_down = true;
    touchResponse();
  } else if (myTouch.dataAvailable() && touch_down){
    //ignore
  } else {
    touch_down = false;
  }

  if(count > 50){
    sensorData();
    count = 0;
  }

  delay(10);
  count++;
}

void touchResponse(void){
  myTouch.read();
  touch_x = myTouch.getX();
  touch_y = myTouch.getY();

  if(touch_x <= 319 && touch_x >= 240 &&
    touch_y >= 0 && touch_y <= 119){
      SPO2_S = true;
      tft.drawString(on, 89, 5, 2);
  } else if(touch_x <= 239 && touch_x >= 160 &&
    touch_y >= 0 && touch_y <= 119){
      TEMP_S = true;
      tft.drawString(on, 89, 85, 2);
  } else if(touch_x <= 238 && touch_x >= 80 &&
    touch_y >= 0 && touch_y <= 119){
      if(AIR_S){
        AIR_S = false;
        tft.drawString(off, 89, 165, 2);
      } else{
        AIR_S = true;
        tft.drawString(on, 89, 165, 2);
      }
  } else if(touch_x <= 79 && touch_x >= 0 &&
    touch_y >= 0 && touch_y <= 119){
      if(ECG_S){
        ECG_S = false;
        tft.drawString(off, 89, 245, 2);
      } else{
        ECG_S = true;
        tft.drawString(on, 89, 245, 2);
      }
  } else if(touch_x <= 319 && touch_x >= 240 &&
    touch_y >= 120 && touch_y <= 239){
      if(EMC_S){
        EMC_S = false;
        tft.drawString(off, 209, 5, 2);
      } else{
        EMC_S = true;
        tft.drawString(on, 209, 5, 2);
      }
  } else if(touch_x <= 239 && touch_x >= 160 &&
    touch_y >= 120 && touch_y <= 239){
    if(GSR_S){
      GSR_S = false;
      tft.drawString(off, 209, 85, 2);
      } else{
        GSR_S = true;
        tft.drawString(on, 209, 85, 2);
      }
  } else if(touch_x <= 239 && touch_x >= 80 &&
    touch_y >= 120 && touch_y <= 239){
    if(SPO2A_S){
      SPO2A_S = false;
      tft.drawString(off, 209, 165, 2);
      } else{
        SPO2A_S = true;
        tft.drawString(on, 209, 165, 2);
      }
  } 
}

void sensorData(void){

  if(SPO2_S){
    if(Sensor.pulsioximeter() == false){
      SPO2_S = false;
      tft.drawString(off, 89, 5, 2);
    }
  }
  if(TEMP_S){
    if(Sensor.temperature() == false){
      TEMP_S = false;
      tft.drawString(off, 89, 85, 2);
    }
  }
  if(AIR_S){
    Sensor.airflow();
  }
  if(ECG_S){
    Sensor.ECG();
  }
  if(EMC_S){
    Sensor.EMG();
  }
  if(GSR_S){
    Sensor.GSR();
  }
  if(SPO2A_S){
    Sensor.pulsioximeter_alarm();
  }
}

