/*
  Sixfab_NBIoT.cpp 
  -
  Library for Sixfab Arduino NBIoT Shield.
  -
  Created by Yasin Kaya (selengalp), September 17, 2018.
*/

#include "Sixfab_NBIoT.h"

Sixfab_HDC1080 hdc1080;
MMA8452Q accel; // I2C Address : 0x1C

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  SoftwareSerial DEBUG(10,11); // RX, TX - 9600 baud rate
#endif 

// default
SixfabNBIoT::SixfabNBIoT()
{

}

/******************************************************************************************
 *** Base Functions : Set or Clear Hardwares - Status Controls - Helper AT Functions  *****
 ******************************************************************************************/

// function for initializing BC95 module.
void SixfabNBIoT::init()
{
  // setting pin directions
  pinMode(USER_LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(USER_BUTTON, INPUT);
  pinMode(ALS_PT19_PIN, INPUT);
  
  enable();

  // setting serials
  BC95_AT.begin(9600);
  DEBUG.begin(9600);

  DEBUG.println("Module initializing");
  delay(500); // wait until module ready.

  // HDC1080 begin
  hdc1080.begin(0x40);
  // mma8452q init 
  accel.init();

  setAutoConnectConf(AUTO_ON);
  setScrambleConf(SCRAMBLE_ON);

  sendATComm("ATE1","OK\r\n"); 
  sendATComm("ATE1","OK\r\n");
  sendATComm("AT","OK\r\n");
}

// power up BC95 module and all peripherals from voltage regulator 
void SixfabNBIoT::enable()
{
  pinMode(BC95_ENABLE, OUTPUT);
  digitalWrite(BC95_ENABLE,HIGH);
}

// power down BC95 module and all peripherals from voltage regulator 
void SixfabNBIoT::disable()
{
  digitalWrite(BC95_ENABLE,LOW);
}

// send at comamand to module
void SixfabNBIoT::sendATCommOnce(const char *comm)
{
  BC95_AT.print(comm);
  BC95_AT.print("\r");
  //DEBUG.println(comm);
}

// function for sending at command to BC95_AT.
const char* SixfabNBIoT::sendATComm(const char *command, const char *desired_reponse)
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
        return response;
        memset(response, 0 , strlen(response));
        break;
      }    
  }
}

// function for sending data to BC95_AT.
const char* SixfabNBIoT::sendDataComm(const char *command, const char *desired_reponse)
{
  uint32_t timer;
  char response[AT_RESPONSE_LEN]; // module response for AT commands. 

  memset(response, 0 , AT_RESPONSE_LEN);
  BC95_AT.flush();

  BC95_AT.print(command);
 
  timer = millis();
   while(true){
    if(millis()-timer > timeout){
      BC95_AT.print(command);
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
        return response;
        memset(response, 0 , strlen(response));
        break;
      }    
  }
}

// function for reset BC95_AT module
void SixfabNBIoT::resetModule()
{
  saveConfigurations();
  delay(200);

  digitalWrite(BC95_ENABLE,LOW);
  delay(200);
  digitalWrite(BC95_ENABLE,HIGH);
  delay(200);
}

// Function for save configurations that be done in current session. 
void SixfabNBIoT::saveConfigurations()
{
  sendATComm("AT&W","OK\r\n");
}

// Function for getting IMEI number
const char* SixfabNBIoT::getIMEI()
{
  return sendATComm("AT+CGSN","OK\r\n");
}

// Function for getting firmware info
const char* SixfabNBIoT::getFirmwareInfo()
{
  return sendATComm("AT+CGMR","OK\r\n");
}

//Function for getting hardware info
const char* SixfabNBIoT::getHardwareInfo()
{
  return sendATComm("AT+CGMM","OK\r\n");
}

// Function for setting autoconnect feature configuration 
void SixfabNBIoT::setAutoConnectConf(const char *autoconnect)
{
  strcpy(compose, "AUTOCONNECT,");
  strcat(compose, autoconnect);
  sendATComm(compose,"OK\r\n");
  clear_compose();
}

// Function for setting scramble feature configuration 
void SixfabNBIoT::setScrambleConf(const char *scramble)
{
  strcpy(compose, "AT+NCONFIG=CR_0354_0338_SCRAMBLING,");
  strcat(compose, scramble);
  sendATComm(compose,"OK\r\n");
  clear_compose();
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


/******************************************************************************************
 *** Network Service Functions ************************************************************
 ******************************************************************************************/ 

// function for getting signal quality
const char* SixfabNBIoT::getSignalQuality()
{
  return sendATComm("AT+CSQ","OK\r\n");
}

// connect to base station of operator
void SixfabNBIoT::connectToOperator()
{
  DEBUG.println("\nTrying to connect base station of operator...");
  sendATComm("AT+CGATT=1","OK\r\n");
  delay(500);
  sendATComm("AT+CGATT?","+CGATT:1\r\n");
  
  getSignalQuality(); 
}

/******************************************************************************************
 *** TCP & UDP Protocols Functions ********************************************************
 ******************************************************************************************/

// function for connecting to server via UDP
void SixfabNBIoT::startUDPService()
{
  char *port = "3005";

  strcpy(compose, "AT+NSOCR=DGRAM,17,");
  strcat(compose, port);
  strcat(compose, ",0");
  
  sendATComm(compose,"OK\r\n");
  clear_compose();
}

// fuction for sending data via udp.
void SixfabNBIoT::sendDataUDP(const char *data)
{
  char data_len[5];
  sprintf(data_len, "%d", strlen(data));

  convert_data_str_to_hex(data);

  strcpy(compose, "AT+NSOST=0,");
  strcat(compose, ip_address);
  strcat(compose, ",");
  strcat(compose, port_number);
  strcat(compose, ",");
  strcat(compose, data_len);
  strcat(compose, ",");
  strcat(compose, data_hex);

  sendATComm(compose,"\r\n");
  clear_compose();
  clear_data_hex();
}

// function for closing server connection
void SixfabNBIoT::closeConnection()
{
  sendATComm("AT+NSOCL=0","\r\n");
}

/******************************************************************************************
 *** Peripheral Devices' Functions : Read sensors - Set Relay and LEDs ********************
 ******************************************************************************************/  

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

