/* TFMPlus library https://github.com/budryerson/TFMini-Plus
   Distance in centimeters,
   Use the 'sendCommand()' to send commands and return a status code.
   Commands are selected from the library's list of defined commands.
   Parameters can be entered directly (115200, 250, etc) but for
   safety, they should be chosen from the library's defined lists.
MR> Works with TF02-Pro "as is"!!!
MR> Added TFT display
MR> Connections (NANO):
  LIDAR (TF):
    White - D6
    Green - D5
    Red (5v) - 5V - 12V
    Black (Gnd) - Gnd
  LoRa (Ra-01 AI Thinker)
    GND - Gnd   //земля
    3V3 - 3V3   //Питание
    RST - D7
    NSS - D4   //AKA SS, CS
    MOSI - D11
    MISO - D12
    SCK - D13   //Clock
    DIO0 - D2   //Interrupt - maybe not needed (no recieve)
  1.8" 128X160 RGB_TFT
    GND - GND
    VCC - 5V
    SCL - D13
    SDA - D11
    Res - D9
    DC - D8
    CS - D10
*/

//LIDAR:
#include <TFMPlus.h>  // Include TFMini Plus Library v1.4.1
TFMPlus lidar;         // Create a TFMini Plus object

// The Software Serial library is an alternative for devices that
// have only one hardware serial port.
#include <SoftwareSerial.h>
SoftwareSerial mySerial( 5, 6);

// Initialize lidar data variables:
int16_t curDist;    // Distance to object in centimeters
int16_t curFlux;    // Strength or quality of return signal
int16_t curTemp;    // Internal temperature of Lidar sensor chip
int16_t prevDist;    // To erase previous data on display
int16_t prevFlux;    // To erase previous data on display
int16_t prevTemp;    // To erase previous data on display

//LORA:
#include <SPI.h>              // include libraries
#include <LoRa.h>
//(Settings for Nano + AI Thinker)
#define LoRa_CS 4        //CS/SS/NSS LoRa radio chip/slave select
#define LoRa_RST 7       //RST LoRa radio reset 
#define LoRa_IRQ 2       //IRQ a hardware interrupt pin

#define WORK_FREQUENCY 434E6
#define SYNC_WORD 3

//DISPLAY:
#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735S
//#include <SPI.h>             // Arduino SPI library (already included with lidar!)

//display connections
#define DISP_CS     10   //There is no CS on available 240x240 IPS 1.3" board!!!!! 
#define DISP_RST    9  // define reset pin, or set to -1 and connect to Arduino RESET pin
#define DISP_DC     8  // define data/command pin

// Initialize Adafruit display library
//Adafruit_ST7789 display = Adafruit_ST7789(DISP_CS, DISP_DC, DISP_RST);
Adafruit_ST7735 display = Adafruit_ST7735(DISP_CS, DISP_DC, DISP_RST);

//For display:
char blank[] = {(0xDA), (0xDA), (0xDA), (0xDA), (0xDA), (0xDA), 0}; 
//MR> 0xDA is the "full square" char to erase whatever was written before on this text position,
//however it is better to write on the same spot the previous char in black coor (less flicker). 
int16_t distCursorX, distCursorY;
int16_t fluxCursorX, fluxCursorY;
int16_t tempCursorX, tempCursorY;

//==== MILLIS TIMER MACRO ====
// performs the {subsequent} code once and then again after each x ms
#define EVERY_MS(x) \
  static uint32_t tmr;\
  bool flg = millis() - tmr > (x);\
  if (flg) tmr = millis();\
  if (flg)
//===========================
