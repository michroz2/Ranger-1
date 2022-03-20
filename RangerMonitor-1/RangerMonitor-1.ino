/* Выносной монитор лидара показывает расстояние в метрах на
   4-дигитальном 7-сегментном дисплее
   Используется BSFrance LoRa32u4
  Соединения дисплея к MPU:
  5V - 5V
  GND - GND
  DIO - 6 (В этом случае надо перерезать на обороте MPU перемычку «DIO1->6»)
  CLK - 5
*/

//LORA:
#include <SPI.h>
#include <LoRa.h>
//(Settings for BSFrance)
#define LoRa_CS 8        //CS/SS/NSS LoRa radio chip/slave select
#define LoRa_RST 4       //RST LoRa radio reset 
#define LoRa_IRQ 7       //IRQ a hardware interrupt pin

#define WORK_FREQUENCY 434E6
#define SYNC_WORD 3

//DISPLAY:

#include <TM1637.h>
// Instantiation and pins configurations
#define DISP_DIO 6
#define DISP_CLK 5
TM1637 tm(DISP_DIO, DISP_CLK);

String dispDist;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  initDisplay();
  initLoRa();
  setLoRaParams();

  LoRa.onReceive(onReceive);
  LoRa.receive(); //Always listen by default

}

void loop() {
  //Nothing here - everything works "onReceive"
}///loop()

void initDisplay() {
  tm.begin();
  tm.setBrightness(4);

}///void initDisplay()

void onReceive(int packetSize) {

  //  Serial.println(F("\n<<<Package Received>>>"));
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.print("RSSI: " + String(LoRa.packetRssi()));
  Serial.print(", Snr: " + String(LoRa.packetSnr()));
  Serial.print(", ΔFreq: " + String(LoRa.packetFrequencyError()));
  Serial.println(", Len: " + String(packetSize));
  char message[packetSize];
  byte chunk;
  for (byte i = 0; i < packetSize; i++) {
    chunk = LoRa.read();
    message[i] = chunk;
    Serial.print(chunk, HEX);
    Serial.print(" ");
  }
  message[packetSize] = 0;
  Serial.println();
  Serial.println(message);
  dispDist = message;

  displayDistance(dispDist);

  digitalWrite(LED_BUILTIN, LOW);

}//void onReceive(int packetSize)

void   displayDistance(String &distance) {
 
    tm.display(distance);

}///void   displayDistance(dispDist)



void initLoRa() {
  Serial.print("LoRa init on\t");
  float freqMHZ = WORK_FREQUENCY / 1000000;
  Serial.print(freqMHZ, 1);
  Serial.println("\tMHz");
  // override the library default CS, reset, and IRQ pins:
  LoRa.setPins(LoRa_CS, LoRa_RST, LoRa_IRQ);
  delay(300);

  if (!LoRa.begin(WORK_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}////void initLoRa()

void setLoRaParams() {
  Serial.println(F("setLoRaParams()"));
  //Set LoRa for Longest Range:
  //  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);    //max
  LoRa.setSignalBandwidth(125E3);                 //..31.25E3, 41.7E3, 62.5E3, (125E3), and 250E3.
  LoRa.setSpreadingFactor(8);                    //default = 7
  LoRa.setPreambleLength(6);                    //min = 6, default = 8
  LoRa.setSyncWord(SYNC_WORD);     //byte value, defaults to 0x12
  //  LoRa.enableCrc();                             //
  //  LoRa.setCodingRate4(5);

  LoRa.setSyncWord(SYNC_WORD);           // ranges from 0-0xFF, default 0x34, see API docs

}// DONE void setLoRaParams()
