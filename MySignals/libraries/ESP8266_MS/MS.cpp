#include "MS.h"

bool ESP8266_MS::startup(void)
{
  serial_empty();
  Serial.println("AT");
  return recvFind("OK");
}

bool ESP8266_MS::firmware(void)
{
  String response;

  serial_empty();
  Serial.println("AT+GMR");
  response = recvString("OK");
  //Serial.println(response);
  return (response.indexOf("OK") != -1);
}

bool ESP8266_MS::restart(void) 
{
  unsigned long start;
  serial_empty();
  Serial.println("AT+RST");
  if (recvFind("OK")) {
    delay(2000);
    start = millis();
    while (millis() - start < 3000) {
      if (startup()) {
        delay(1500); /* Waiting for stable */
        return true;
      }
      delay(100);
    }
  }
  return false;
}

bool ESP8266_MS::setUart(uint32_t baud)
{
  serial_empty();
  Serial.print("AT+UART_CUR=");
  Serial.print(baud);
  Serial.println(",8,1,0,1");
  return recvFind("OK");
}

bool ESP8266_MS::setOprToStation(void)
{    
    String response;
    
    serial_empty();
    Serial.println("AT+CWMODE_CUR?");
    response = recvString("OK");
    if (response.indexOf("1") != -1 && 
        response.indexOf("OK") != -1) {
      return true;
    } else if (response.indexOf("OK") == -1){
      return false;
    }

    serial_empty();
    Serial.println("AT+CWMODE_CUR=1");    
    response = recvString("OK");
    if (response.indexOf("OK") != -1){
        return restart();
    }
    return false;
}


bool ESP8266_MS::joinAP(String ssid, String pwd)
{
    String data;
    serial_empty();
    Serial.print("AT+CWJAP_CUR=\"");
    Serial.print(ssid);
    Serial.print("\",\"");
    Serial.print(pwd);
    Serial.println("\"");
    
    data = recvString("OK", "FAIL", 10000);

//Serial.println();
//Serial.println(data);

    if (data.indexOf("OK") != -1) {
        return true;
    }
    return false;
}

bool ESP8266_MS::getAP(void)
{
    String data;
    serial_empty();
    Serial.println("AT+CWJAP_CUR?");

    
    data = recvString("OK", 10000);
//Serial.println();
//Serial.println(data);
    if (data.indexOf("OK") != -1) {
        return true;
    }
    return false;
}


bool ESP8266_MS::singleConnection(void)
{
    String data;
    serial_empty();
    Serial.print("AT+CIPMUX=0");

    
    data = recvString("OK", "Link is builded");
    //Serial.println();
    //Serial.println(data);
    if (data.indexOf("OK") != -1) {
        return true;
    }
    return false;
}

bool ESP8266_MS::startTCP(String type, String addr, uint32_t port)
{
    String data;
    serial_empty();
    Serial.print("AT+CIPSTART=\"");
    Serial.print(type);
    Serial.print("\",\"");
    Serial.print(addr);
    Serial.print("\",");
    Serial.println(port);
    
    data = recvString("OK", "ERROR", "ALREADY CONNECT", 10000);
    //Serial.println();
    //Serial.println(data);
    if (data.indexOf("OK") != -1 || data.indexOf("ALREADY CONNECT") != -1) {
        return true;
    }
    return false;
}

bool ESP8266_MS::tcpStatus(void)
{
    String data;
    delay(100);
    serial_empty();
    Serial.println("AT+CIPSTATUS");
    data = recvString("OK");
    Serial.println(data);
    return true;
}

bool ESP8266_MS::tcpSend(String msg, uint32_t len)
{
    serial_empty();
    Serial.print("AT+CIPSEND=");
    Serial.println(len);
    if (recvFind(">", 5000)) {
        serial_empty();
        Serial.println(msg);
        return recvFind("SEND OK", 10000);
    }
    return false;
}

bool ESP8266_MS::tcpSend(const uint8_t *buffer, uint32_t len)
{
    serial_empty();
    Serial.print("AT+CIPSEND=");
    Serial.println(len);
    if (recvFind(">", 5000)) {
        serial_empty();
        for (uint32_t i = 0; i < len; i++) {
            Serial.write(buffer[i]);
        }
        return recvFind("SEND OK", 10000);
    }
    return false;
}



void ESP8266_MS::serial_empty(void) 
{
  while(Serial.available() > 0) {
    Serial.read();
  }
}

bool ESP8266_MS::recvFind(String target, uint32_t timeout)
{
  String data_tmp;
  data_tmp = recvString(target, timeout);
  if (data_tmp.indexOf(target) != -1) {
    return true;
  }
  return false;
}

String ESP8266_MS::recvString(String target, uint32_t timeout)
{
  String data;
  char a;
  unsigned long start = millis();
  while (millis() - start < timeout) {
    while(Serial.available() > 0) {
      a = Serial.read();
      if(a == '\0'){
        continue;
      }
      data += a;
    }
    if (data.indexOf(target) != -1) {
      break;
    }
  }
  return data;
}

String ESP8266_MS::recvString(String target1, String target2, uint32_t timeout)
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(Serial.available() > 0) {
            a = Serial.read();
			if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            break;
        } else if (data.indexOf(target2) != -1) {
            break;
        }
    }
    return data;
}

String ESP8266_MS::recvString(String target1, String target2, String target3, uint32_t timeout)
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(Serial.available() > 0) {
            a = Serial.read();
			if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target1) != -1) {
            break;
        } else if (data.indexOf(target2) != -1) {
            break;
        } else if (data.indexOf(target3) != -1) {
            break;
        }
    }
    return data;
}

