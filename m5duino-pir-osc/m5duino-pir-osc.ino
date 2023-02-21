/*
Sensor TOF to osc broadcast using microOSC
*/
byte ID = 1;
#define SAMPLING 100
#include <Ethernet2.h>
EthernetUDP udp;

#define PIRPIN 32


IPAddress broadcastIp(192, 168, 1, 255);
IPAddress sendIp(192, 168, 1, 255);
unsigned int sendPort = 50501;
unsigned int receivePort = 50500;

unsigned long myChronoStart = 0;


#include <MicroOscUdp.h>
// The number 1024 between the < > below  is the maximum number of bytes reserved for incomming messages.
// Outgoing messages are written directly to the output and do not need more reserved bytes.
MicroOscUdp<1024> oscUdp(&udp, sendIp, sendPort);


//  OSC MESSAGE
//Changer les derniers "chiffres de la mac addresse"
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xAE, ID };

//mac[5]=ID;
//
// ethPOE
#include <SPI.h>



#define SCK 22
#define MISO 23
#define MOSI 33
#define CS 19




// m5 LED
#include "M5Atom.h"

// couleur du led
#define RED 0xff, 0x00, 0x00
#define YELLOW 0xff, 0xff, 0x00
#define GREEN 0x00, 0xff, 0x00
#define CYAN 0x00, 0xff, 0xff
#define BLUE 0x00, 0x00, 0xff
#define PINK 0xff, 0x00, 0xff
#define OFF 0x00, 0x00, 0x00



uint8_t DisBuff[2 + 5 * 5 * 3];
void set_m5_led(uint8_t Rdata, uint8_t Gdata, uint8_t Bdata) {
  DisBuff[0] = 0x05;
  DisBuff[1] = 0x05;
  for (int i = 0; i < 25; i++) {
    DisBuff[2 + i * 3 + 1] = Rdata;  // -> fix : grb? weird?
    DisBuff[2 + i * 3 + 0] = Gdata;  //->  fix : grb? weird?
    DisBuff[2 + i * 3 + 2] = Bdata;
  }
  M5.dis.displaybuff(DisBuff);
}

void cycle_m5_color(int d_time) {
  delay(d_time);
  set_m5_led(RED);
  delay(d_time);
  set_m5_led(YELLOW);
  delay(d_time);
  set_m5_led(GREEN);
  delay(d_time);
  set_m5_led(CYAN);
  delay(d_time);
  set_m5_led(BLUE);
  delay(d_time);
  set_m5_led(PINK);
}


// \m5 LED




void setup() {
  // put your setup code here, to run once:
  M5.begin(true, false, true);
  Wire.begin(26, 32);  // join i2c bus (address optional for master)
  set_m5_led(RED);


  SPI.begin(SCK, MISO, MOSI, -1);
  Ethernet.init(CS);

  // init DHCP
  Ethernet.begin(mac);
  udp.begin(8888);
  set_m5_led(PINK);  // IF here; ethernet initialize


  // Print init state
  Serial.print("Button ip => ");
  Serial.print(Ethernet.localIP());
  Serial.print(" broadcasting => ");
  //Serial.println(ip_broadcast);


}


void loop() {
  // put your main code here, to run repeatedly:
  // listen for incoming clients
  M5.update();
  unsigned long myChronoElapsed = millis() - myChronoStart;
  unsigned long myChronoInterval = SAMPLING;
  if ( myChronoElapsed >= myChronoInterval) {
    myChronoStart = millis();
     oscUdp.sendMessage("/pir", "i" "i" "s", digitalRead(PIRPIN), ID, (Ethernet.localIP().toString()));
  }
  

  

  if (M5.Btn.wasPressed()) {
    Serial.println("BTN 1");
    oscUdp.sendMessage("/btn", "i" "i" "s", 1,ID, (Ethernet.localIP().toString()) );
    cycle_m5_color(10);
  }
  if (M5.Btn.wasReleased()) {
    Serial.println("BTN 0");
    oscUdp.sendMessage("/btn", "i" "i" "s", 0,ID, (Ethernet.localIP().toString()));
    cycle_m5_color(10);
  }
  
}
