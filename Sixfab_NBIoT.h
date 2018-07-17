/*
  arduinoiot.h - Library for Sixfab Arduino NBIoT Shield.
  Created by Yasin Kaya (selengalp), july 5, 2018.
*/

#ifndef _SIXFAB_NBIOT_H
#define _SIXFAB_NBIOT_H

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
    #include "pins.h"    
#endif

#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Sixfab_HDC1080.h>
#include <Sixfab_MMA8452Q.h>

// determine board type
// Arduino Geniuno / Uno or Mega
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define BC95_AT Serial
  extern SoftwareSerial DEBUG; // RX, TX

// Tinylab, Arduino Leonardo or Micro  
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega16U4__)
  #define BC95_AT Serial1 
  #define DEBUG Serial

#endif

// Peripheral Pin Definations
#define USER_BUTTON 8
#define USER_LED 6
#define BC95_ENABLE 4
#define ALS_PT19_PIN A1
#define RELAY 5

// Constants  
#define TIMEOUT 1000
#define IP_ADDRESS_LEN 30
#define DOMAIN_NAME_LEN 50
#define PORT_NUMBER_LEN 8  
#define AT_COMM_LEN 200
#define AT_RESPONSE_LEN 400
#define DATA_COMPOSE_LEN 200
#define DATA_LEN_LEN 5  



class SixfabNBIoT
{
  public:

    /*
    Default constructer with no parameter

    [no-return]
    ---
    [no-param]
    */
    SixfabNBIoT();

    /*
    Function for initializing module and peripherals
    This function do things below:
    * enables bc95 module
    * sets pin directions


    [no-return]
    ---
    [no-param]
    */
    void init(); // initialize

    /*
    Function for powering up bc95 module

    [no-return]
    ---
    [no-param]
    */
    void enable();
    
    /*
    Function for powering down bc95 module

    [no-return]
    ---
    [no-param]
    */
    void disable();

    /*
    Function for connecting to base station of operator

    [no-return]
    ---
    [no-param]
    */
    void connectToOperator();

    /*
    Function for sending AT [param #1] command to bc95.
    
    [no-return]
    ---
    [param #1] : const char* AT command word
    */
    void sendATCommOnce(const char *);

    /*
    Function for sending AT [param #1] command to bc95. If the desired [param #2] 
    response isn't recevived, function resend the AT command wait a time as [timeout].
    
    [no-return]
    ---
    [param #1] : const char* AT command word
    [param #2] : const char* AT desired_response word
    [param #3] : const char* AT response word
    */
    void sendATComm(const char *, const char *); 

    /*
    Function for resetting b595 module

    [no-return]
    ---
    [no-param]
    */
    void resetModule();

    /*
    Function for getting described ip address

    [return] : const char* IP Address
    ---
    [no-param]
    */
    const char* getIPAddress();

    /*
    Function for setting desired ip address

    [no-return]
    ---
    [param #1] : char * desired ip address
    */
    void setIPAddress(char *);

    /*
    Function for getting described domain name

    [return] : const char* domain name
    ---
    [no-param]
    */
    const char* getDomainName();

    /*
    Function for setting desired domain name

    [no-return]
    ---
    [param #1] : char * desired domain name
    */
    void setDomainName(char *);

    /*
    Function for getting described port number

    [return] : const char* port number
    ---
    [no-param]
    */
    const char* getPort();

    /*
    Function for setting desired port number

    [no-return]
    ---
    [param #1] : char * desired port number
    */
    void setPort(char *);

    /*
    Function for getting described timeout

    [return] : uint16_t described timeout in ms
    ---
    [no-param]
    */
    uint16_t getTimeout();

    /*
    Function for setting desired timeot

    [no-return]
    ---
    [param #1] : uint16_t desired timeout in ms
    */
    void setTimeout(uint16_t);

    /*
    Function for sending data via UDP protocol. 
    First use setIPAddress and setPort functions before 
    try to send data with this function.  

    [param #1] : const char* data word
    */
    void sendDataUDP(const char *);

    void readAccel(double*, double*, double*);
    double readTemp();
    double readHum();
    double readLux();
    void turnOnRelay();
    void turnOffRelay();
    uint8_t readUserButton();
    void turnOnUserLED();
    void turnOffUserLED();

  private:
    char ip_address[IP_ADDRESS_LEN]; //ip address       
    char domain_name[DOMAIN_NAME_LEN]; // domain name   
    char port_number[PORT_NUMBER_LEN]; // port number 
    uint16_t timeout = TIMEOUT; // default timeout for function and methods on this library.
};
#endif