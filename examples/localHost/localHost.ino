/*
  localHost.ino - This is basic local host example.
  Created by Yasin Kaya (selengalp), September 17, 2018.
*/

#include "Sixfab_NBIoT.h"

SixfabNBIoT node;

char your_ip[] = "xx.xx.xx.xx";
char your_port[] = "xxxx";

double ax, ay, az;
// ------------------------------------------------------------------
// ------------------------- SETUP ----------------------------------
// ------------------------------------------------------------------
void setup() {
  
  node.init();
  node.turnOnUserLED();
  delay(TIMEOUT);
  node.turnOffUserLED();
  delay(TIMEOUT);
  node.turnOnRelay();
  delay(TIMEOUT);
  node.turnOffRelay();
  delay(TIMEOUT);

  node.readAccel(&ax, &ay, &az);
  DEBUG.print("ax: "); DEBUG.println(ax);
  DEBUG.print("ay: "); DEBUG.println(ay);
  DEBUG.print("az: "); DEBUG.println(az);
  
  DEBUG.print("Temperature: ");DEBUG.print(node.readTemp());
  delay(TIMEOUT);
  DEBUG.print("Humidity: ");DEBUG.print(node.readHum());
  delay(TIMEOUT);
  DEBUG.print("Light: ");DEBUG.print(node.readLux());
  delay(TIMEOUT);
  node.connectToOperator();

  node.setIPAddress(your_ip);
  node.setPort(your_port);

  node.startUDPService();
  node.sendDataUDP("Hello World from Sixfab Crew");
}
// ------------------------------------------------------------------
// --------------------------- LOOP ---------------------------------
// ------------------------------------------------------------------
void loop() {

}