#include "MSPrint.h"

bool MSPrint::temperature(){

  static uint8_t prime = 0;
  static uint8_t count = 0;
  static float temp = 0;
  bool status = true;

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  if (prime < 10){
    prime++;
    MySignals.getTemperature();
  } else if (count < 50){
    count++;
    temp += MySignals.getTemperature();
  } else {
    temp = temp / count;
    Serial.println();
    Serial.print("Temp/");
    Serial.println(temp);
    prime = 0;
    temp = 0;
    count = 0;
    status = false;
  }

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

  return status;
}


void MSPrint::airflow(){

  float air = MySignals.getAirflow(VOLTAGE);

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);
   
  Serial.println();
  Serial.print("Airflow/");
  Serial.println(air, 2);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}

void MSPrint::ECG(){

  float ECG = MySignals.getECG(VOLTAGE);

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  Serial.println();
  Serial.print("ECG/");
  Serial.println(ECG, 2);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}


void MSPrint::EMG(){

  float EMG = MySignals.getEMG(VOLTAGE);

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  Serial.println();
  Serial.print("EMG/");
  Serial.println(EMG, 2);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}

void MSPrint::GSR(){

  float conductance = MySignals.getGSR(CONDUCTANCE);
  float resistance = MySignals.getGSR(RESISTANCE);
  float conductanceVol = MySignals.getGSR(VOLTAGE);

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  Serial.println();
  Serial.print("GSR/");
  Serial.print(conductance, 2);
  Serial.print(",");
  Serial.print(resistance, 2);
  Serial.print(",");
  Serial.println(conductanceVol, 4);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}


bool MSPrint::pulsioximeter(){

  bool status = true;

  MySignals.enableSensorUART(PULSIOXIMETER);
  uint8_t statusPulsioximeter = MySignals.getStatusPulsioximeterGeneral();
  String data = "";

  if(MySignals.getStatusPulsioximeterGeneral() == 1){
    MySignals.enableSensorUART(PULSIOXIMETER_MICRO);
    delay(10);
    if(MySignals.getPulsioximeterMicro() == 1){
      data = "SPO2/";
      data = data + String(MySignals.pulsioximeterData.BPM);
      data = data + ",";
      data = data + String(MySignals.pulsioximeterData.O2);
      
      status = false;
    } else if (MySignals.getPulsioximeterMicro() == 2){
      data = "SPO2/FingerOut";
    }
  }

  //MySignals.disableSensorUART();

  Serial.begin(115200);

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  Serial.println();
  Serial.println(data);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

  return status;
}

void MSPrint::pulsioximeter_alarm(){

  MySignals.enableSensorUART(PULSIOXIMETER);
  uint8_t statusPulsioximeter = MySignals.getStatusPulsioximeterGeneral();
  String data = "";

  if(MySignals.getStatusPulsioximeterGeneral() == 1){
    MySignals.enableSensorUART(PULSIOXIMETER_MICRO);
    delay(10);
    if(MySignals.getPulsioximeterMicro() == 1){
      data = "SPO2_Alarm/";
      data = data + String(MySignals.pulsioximeterData.BPM);
      data = data + ",";
      data = data + String(MySignals.pulsioximeterData.O2);
    } else if (MySignals.getPulsioximeterMicro() == 2){
      data = "SPO2_Alarm/FingerOut";
    }
  }

  //MySignals.disableSensorUART();

  Serial.begin(115200);

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  Serial.println();
  Serial.println(data);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}

void MSPrint::snore(){

  MySignals.initSnore();

  float snore = MySignals.getSnore(VOLTAGE);

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  Serial.println();
  Serial.print("Snore/");
  Serial.println(snore, 2);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}


void MSPrint::bodyPosition(){

  MySignals.initBodyPosition();
  uint8_t position = MySignals.getBodyPosition();
 
  delay(100);
   
  MySignals.getAcceleration();
  
  // convert float to strintg
  char bufferAcc[50];
  char x_acc_string[10];  
  char y_acc_string[10];
  char z_acc_string[10];
  dtostrf (MySignals.x_data, 2, 2, x_acc_string); 
  dtostrf (MySignals.y_data, 2, 2, y_acc_string);
  dtostrf (MySignals.z_data, 2, 2, z_acc_string);
			
  // print the X Y Z acceleration
  sprintf (bufferAcc, ",%s,%s,%s",
           x_acc_string, y_acc_string, z_acc_string);

  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  Serial.println();
  Serial.print("BodyPosition/");
  Serial.print(position);
  Serial.println(bufferAcc);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}

void MSPrint::bloodPressure(){

  MySignals.enableSensorUART(BLOODPRESSURE);

  if (MySignals.getStatusBP())
  {
    delay(1000);

    if (MySignals.getBloodPressure() == 1)
    {
      MySignals.disableMuxUART();

      Serial.begin(115200);

      delay(10);
      digitalWrite(ENABLE, HIGH);
      delay(10);

      Serial.println();
      Serial.print("BloodPressure/");
      Serial.print(MySignals.bloodPressureData.diastolic);
      Serial.print(",");
      Serial.print(MySignals.bloodPressureData.systolic);
      Serial.print(",");
      Serial.println(MySignals.bloodPressureData.pulse);

      delay(10);
      digitalWrite(ENABLE, LOW);
      delay(10);

      MySignals.enableMuxUART();

    }
  }

  MySignals.disableSensorUART();

  Serial.begin(115200);

}


/*
  FUNCTIONS DO NOT HAVE CONSTANT DATA, REVISE METHOD

void glucometer(){

}

void MSPrint::spirometer(){
  MySignals.enableSensorUART(SPIROMETER);

  if (MySignals.getStatusSpiro() != 0)
  {
    MySignals.getSpirometer();
    MySignals.disableMuxUART();

    Serial.print(F("Number of measures:"));
    Serial.println(MySignals.spir_measures);
    Serial.println();

    for (int i = 0; i < MySignals.spir_measures; i++)
    {

      Serial.print(MySignals.spirometerData[i].spir_pef);
      Serial.print(F("L/min "));

      Serial.print(MySignals.spirometerData[i].spir_fev);
      Serial.print(F("L "));

      Serial.print(MySignals.spirometerData[i].spir_hour);
      Serial.print(F(":"));
      Serial.print(MySignals.spirometerData[i].spir_minutes);

      Serial.print(F(" "));

      Serial.print(MySignals.spirometerData[i].spir_year);
      Serial.print(F("-"));
      Serial.print(MySignals.spirometerData[i].spir_month);
      Serial.print(F("-"));
      Serial.println(MySignals.spirometerData[i].spir_day);

    }
    MySignals.enableMuxUART();
    MySignals.disableSensorUART();
    Serial.begin(115200);
  }

}

*/
































