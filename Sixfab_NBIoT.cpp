/*
  arduinoiot.cpp - Library for Sixfab Arduino NBIoT Shield.
  Created by Yasin Kaya (selengalp), july 5, 2018.
*/

#include "Sixfab_NBIoT.h"

// determine board type
// Arduino Geniuno / Uno or Mega
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define BC95_AT Serial
  SoftwareSerial DEBUG(10,11); // RX, TX
#endif

Sixfab_HDC1080 hdc1080;
MMA8452Q accel; // I2C Address : 0x1C

// default
SixfabNBIoT::SixfabNBIoT()
{

}

// function for initializing bc95 module.
void SixfabNBIoT::init()
{
      // setting pin directions
  pinMode(BC95_ENABLE, OUTPUT);
  pinMode(USER_LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(USER_BUTTON, INPUT);
  pinMode(ALS_PT19_PIN, INPUT);

  enable();

  // setting serials
  BC95_AT.begin(9600);
  DEBUG.begin(9600);
  DEBUG.println("Module initializing");

  // HDC1080 begin
  hdc1080.begin(0x40);
  // mma8452q init 
  accel.init();

  sendATComm("ATE1","OK\r\n"); 
  sendATComm("ATE1","OK\r\n"); 
  sendATComm("AT","AT\r\nOK\r\n");
}

// power up bc95 module and peripherals
void SixfabNBIoT::enable()
{
  digitalWrite(BC95_ENABLE,HIGH);
}

// power down bc95 module
void SixfabNBIoT::disable()
{
  digitalWrite(BC95_ENABLE,LOW);
}

// connect to base station of operator
void SixfabNBIoT::connectToOperator()
{
  DEBUG.println("Trying to connect base station of operator...");
  sendATComm("AT+CGATT=1","OK\r\n");
  sendATComm("AT+CGATT?","+CGATT:1\r\n");
  sendATComm("AT+CSQ","OK\r\n"); 
}

// send at comamand to module
void SixfabNBIoT::sendATCommOnce(const char *comm)
{

  BC95_AT.print(comm);
  BC95_AT.print("\r");
  //DEBUG.println(comm);
}

// function for sending at command to BC95_AT.
void SixfabNBIoT::sendATComm(const char *command, const char *desired_reponse)
{
  uint32_t timer;
  char response[AT_RESPONSE_LEN]; // module response for AT commands. 

  memset(response, 0 , AT_RESPONSE_LEN);
  BC95_AT.flush();

  sendATCommOnce(command);
 
  timer = millis();
   while(true){
    if(millis()-timer > timeout){
      sendATCommOnce(command);
      timer = millis();
    }
    char c;
    int i = 0; 

    while(BC95_AT.available()){
      c = BC95_AT.read();
      DEBUG.write(c);
      response[i++]=c;
      delay(2);
      }
      if(strstr(response, desired_reponse)){
        memset(response, 0 , strlen(response));
        break;
      }    
  }
}

// function for reset BC95_AT module
void SixfabNBIoT::resetModule()
{
  sendATCommOnce("AT+NRB");
  DEBUG.println("Rebooting Module...");
  delay(4000);
}

// function for getting ip_address
const char* SixfabNBIoT::getIPAddress()
{
  return ip_address;
}

// function for setting ip_address
void SixfabNBIoT::setIPAddress(char *ip)
{
  strcpy(ip_address, ip);
}

// function for getting domain_name
const char* SixfabNBIoT::getDomainName()
{
  return domain_name;
}

// function for setting domain name
void SixfabNBIoT::setDomainName(char *domain)
{
	strcpy(domain_name, domain);
}

// function for getting domain_name
const char* SixfabNBIoT::getPort()
{
  return port_number;
}

// function for setting port
void SixfabNBIoT::setPort(char *port)
{
	strcpy(port_number, port);
}

// get timout in ms
uint16_t SixfabNBIoT::getTimeout()
{
  return timeout;
}

// set timeout in ms    
void SixfabNBIoT::setTimeout(uint16_t new_timeout)
{
  timeout = new_timeout; 
}

// fuction for sending data via udp.
void SixfabNBIoT::sendDataUDP(const char *data)
{
  char compose[DATA_COMPOSE_LEN]="";
  char data_hex[DATA_COMPOSE_LEN];
  char data_len[DATA_LEN_LEN];
  
  for(int i= 0; i<strlen(data); i++){
    sprintf(data_hex+i*2, "%02X", data[i]);
    }  

  sprintf(data_len, "%d", strlen(data));
  strcat(compose, "AT+NSOST=0,");
  strcat(compose, ip_address);
  strcat(compose, ",");
  strcat(compose, port_number);
  strcat(compose, ",");
  strcat(compose, data_len);
  strcat(compose, ",");
  strcat(compose, data_hex);
  DEBUG.print("\n"); 
  DEBUG.println(compose);
  
  sendATComm(compose,"OK\r\n");
}

// 
void SixfabNBIoT::readAccel(double* ax, double* ay, double* az)
{
  accel.read();
  *ax = accel.cx;
  *ay = accel.cy;
  *az = accel.cz;
}

//
double SixfabNBIoT::readTemp()
{
  return hdc1080.readTemperature();
}

// 
double SixfabNBIoT::readHum()
{
  return hdc1080.readHumidity();
}

//
double SixfabNBIoT::readLux()
{
  return analogRead(ALS_PT19_PIN);
}

//
void SixfabNBIoT::turnOnRelay()
{
  digitalWrite(RELAY, HIGH);
}

//
void SixfabNBIoT::turnOffRelay()
{ 
  digitalWrite(RELAY, LOW);
}

//
uint8_t SixfabNBIoT::readUserButton()
{
  digitalRead(USER_BUTTON);
}

//
void SixfabNBIoT::turnOnUserLED()
{
  digitalWrite(USER_LED, HIGH);
}

//
void SixfabNBIoT::turnOffUserLED()
{
  digitalWrite(USER_LED, LOW);
}