/* Тестовая программа для отработки меню системы Ranger-1
  Соединения:
  1.8" 128X160 RGB_TFT Дисплей
    GND - GND
    VCC - 5V
    SCL - D13
    SDA - D11
    Res - D9
    DC - D8
    CS - D10
  Энкодер:
    A - D5
    B - D6
    But1 - D7
    Центр.энк., But2 - GND
  Laser
    "+" - A5
    "-" - GND
*/

//DISPLAY:
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735S
#include <SPI.h>             // Arduino SPI library (already included with lidar!)

//display connections:
#define DISP_CS     10
#define DISP_RST    9  // or set to -1 and connect to Arduino RESET pin
#define DISP_DC     8  // data/command pin

// Initialize Adafruit display library
Adafruit_ST7735 display = Adafruit_ST7735(DISP_CS, DISP_DC, DISP_RST);

#define BLANK 0xDA
//MR> 0xDA is the "full square" char to erase whatever was written before on this text position,
//however it is better to write on the same spot the previous char
//in background color (less flicker).
#define LINEBREAK 5 //space between the lines
#define SCREEN_WIDTH 160
#define CHR_WIDTH 12

int16_t distCursorX, distCursorY;
int16_t fluxCursorX, fluxCursorY;
int16_t tempCursorX, tempCursorY;

//ENCODER:
#include <EncoderButton.h>

/**
   Instatiate an EncoderButton.
   For Arduino Uno, the hardware interrupts are pins 2 & 3
   Encoder+button:
   EncoderButton(byte encoderPin1, byte encoderPin2, byte switchPin);
   Encoder only:
   EncoderButton(byte encoderPin1, byte encoderPin2);
   Button only:
   EncoderButton(byte switchPin);
*/
EncoderButton encoder(3, 4, 7); //5 or 6 didn't work on one Nano

//MENU:
bool menuON;
bool settingON;
bool updateMenu;
int curMenu;
#define NUM_MENUS 5
static String menuItem[NUM_MENUS] = {
  "ЛАЗЕР",
  "КАЛИБР",
  "МИН",
  "МАКС",
  "ЧУВСТВ",
};
static char blank = (0xDA);
String menuData[NUM_MENUS];
uint16_t menuDataPosX[NUM_MENUS];
uint16_t menuPosY[NUM_MENUS];

bool laserON;     //default false;
float calibrDist; //default = 0
float minRange;   //default = 0
float maxRange = 20; //meters; considered an infinity
int sensitivity = 1;
#define MINSENS = 1;
#define MAXSENS = 8;
static float calibrDistIncrement = 0.01;
static float rangeIncrement = 0.1;
static int sensitivityIncrement = 1;

//LASER:
#define PIN_LASER A5

//LIDAR:
#define MIN_RANGE 0.3
#define MAX_RANGE 30

void  displaySomething() {
  uint16_t screenHeight = 128;
  uint16_t screenWidth = 160;
  display.fillScreen(ST77XX_BLACK);   //Clear screen
  uint16_t x0 = 80;
  uint16_t y0 = 64;
  uint16_t x1;
  uint16_t y1;
  uint16_t color;
  for (byte i = 0; i < 64; i++) {
    x1 = random(0, screenWidth);
    y1 = random(0, screenHeight);
    color = random(0, 65536);
    display.drawLine (x0, y0, x1, y1, color);
    x0 = x1;
    y0 = y1;
  }
}///void  displaySomething()

void setup() {
  Serial.begin( 9600);   // Intialize terminal serial port for debugging
  delay(20);               // Give port time to initalize

  initEncoder();
  initDisplay();
  pinMode(PIN_LASER, OUTPUT);

  displaySomething();   //placeholder screen draw
}

void loop() {
  // put your main code here, to run repeatedly:
  encoder.update();
  if (menuON) {
    //TODO
  }
  else {
    //update working screen
  }
}

void initEncoder() {
  //Link the event(s) to your function
  encoder.setDoubleClickHandler(onDoubleClicked);
  encoder.setClickHandler(onClicked);
  encoder.setEncoderHandler(onEncoder);
}///void initEncoder()

void onDoubleClicked(EncoderButton& eb) {
  Serial.println("Doubleclicked");
  menuON = !menuON;
  if (menuON)
    printMenu();
  else
    displaySomething();
}///void onDoubleClicked(EncoderButton& eb)

void onClicked(EncoderButton& eb) {
  Serial.println("Clicked");
  if (menuON) {
    clearMenuSelection();
    settingON = !settingON;
    printMenuSelection();
  }
  else { //switch the Laser on/off
    laserON = !laserON;
    switchLaser();
  }
}///void onClicked(EncoderButton& eb)

void onEncoder(EncoderButton& eb) {
  Serial.println("Encoder");
  if (menuON) {
    Serial.println(encoder.increment());
    if (settingON) {

      /*
        bool laserON;     //default false;
        float calibrDist; //default = 0
        float minRange;   //default = 0
        float maxRange = 20; //meters; considered an infinity
        int sensitivity;
      */
      int increment = encoder.increment();
      clearMenuDataItem(curMenu);
      Serial.println("curMenu: " + String(curMenu));
      if (curMenu == 0) {
        laserON = !laserON;
        switchLaser();
      }
      else if (curMenu == 1) {
        calibrDist += calibrDistIncrement * increment;
      }
      else if (curMenu == 2) {
        float newMinRange = minRange + rangeIncrement * increment;
        minRange = constrain(newMinRange, MIN_RANGE-calibrDist, maxRange);
        Serial.println("MinRange: " + String(minRange));
      }
      else if (curMenu == 3) {
        Serial.println("MaxRange: " + String(maxRange));
        float newMaxRange = maxRange + rangeIncrement * increment;
        maxRange = constrain(newMaxRange, minRange, MAX_RANGE);
        Serial.println("MaxRange: " + String(maxRange));
      }
      else if (curMenu == 4) {
        Serial.println("sensitivity: " + String(sensitivity));
        int newSensitivity = sensitivity + sensitivityIncrement * increment;
        sensitivity = constrain(newSensitivity, MINSENS, MAXSENS);
        Serial.println("sensitivity: " + String(sensitivity));
      }
      updateMenuDataItem(curMenu);
      printMenuDataItem(curMenu);

    }
    else {
      clearMenuSelection();
      curMenu = (curMenu + encoder.increment() + NUM_MENUS) % NUM_MENUS;
      printMenuSelection();
    }
  }
}///void onEncoder(EncoderButton& eb)

void initDisplay() {
  //«Для того, чтобы в новой библиотеке символ 0xB0 (=176)
  //отображался (в кодировке Windows-1251 ему соответствует символ градуса)
  //нужно в начале скетча (после инициализации дисплея)
  //применить функцию cp437() с аргументом true»
  display.cp437(true);
  display.initR(INITR_BLACKTAB);    // Init ST7735 display 128x160 pixel
  display.setRotation(1);   // if the screen is rotated/flipped, update this command in range: (0-3)
  display.fillScreen(ST77XX_BLACK);   //Clear screen
  display.setTextWrap(false);
  display.setTextSize(2);
  display.enableDisplay(true);


  //Init screen labels and data cursor positions:
  display.setCursor(0, 0);
  display.setTextColor(ST77XX_WHITE);

}////void initDisplay()

void printMenu() {    //Prints the whole menu with data and selection
  Serial.println("printMenu()");
  Serial.println(curMenu);
  display.fillScreen(ST77XX_BLACK);   //Clear screen
  display.setCursor(0, 0);
  updateMenuDataAll();
  for (byte i = 0; i < NUM_MENUS; i++) {
    menuPosY[i] = display.getCursorY();
    //если это текущее меню и не режим сеттинга, то рисуем ">"
    display.print (((i == curMenu) && (!settingON)) ? ">" : " ");
    display.print(utf8rus(menuItem[i].c_str()));
    printMenuDataItem(i);
    //если происходит настройка, то рисуем "<"
    display.setCursor(SCREEN_WIDTH - CHR_WIDTH, display.getCursorY());
    display.println( ((i == curMenu) && (settingON)) ? "<" : "");
    //next menu line:
    display.setCursor(display.getCursorX(), display.getCursorY() + LINEBREAK);
  }
}///void printMenu()

void printMenuSelection() {
  if (menuON) {
    int16_t selPositionX = (settingON ? SCREEN_WIDTH - CHR_WIDTH : 0);
    display.setCursor(selPositionX, menuPosY[curMenu]);
    display.print((settingON ? "<" : ">"));
  }

}///void printMenuSelection()

void clearMenuSelection() {
  display.setTextColor(ST77XX_BLACK);
  printMenuSelection();
  display.setTextColor(ST77XX_WHITE);
}///void clearMenuSelection()

void printMenuItem(byte k) {
  Serial.print("printMenuItem(): ");
  Serial.println(k);
  display.setCursor(0, 0);
  for (byte i = 0; i < k; i++) {
    display.print( i == curMenu ? ">" : " ");
    display.print(utf8rus(menuItem[i].c_str()));
    menuPosY[i] = display.getCursorY();
  }
}///void printMenuItem()


void updateMenuDataAll() {
  menuData[0] = utf8rus(laserON ? "ВКЛ" : "ВЫКЛ");
  menuData[1] = String(calibrDist, 2);
  menuData[2] = String(minRange, 1);
  menuData[3] = String(maxRange, 1);
  menuData[4] = String(sensitivity);

}///void updateMenuDataAll()

void updateMenuDataItem(byte i) {
  switch (i) {
    case 0:
      menuData[0] = utf8rus(laserON ? "ВКЛ" : "ВЫКЛ");
      break;
    case 1:
      menuData[1] = String(calibrDist, 2);
      break;
    case 2:
      menuData[2] = String(minRange, 1);
      break;
    case 3:
      menuData[3] = String(maxRange, 1);
      break;
    case 4:
      menuData[4] = String(sensitivity);
      break;
  }
}///void updateMenuDataItem(byte i)


void printMenuDataAll() {
  Serial.println("printMenuData()");
  for (byte i = 0; i < NUM_MENUS; i++) {
    clearMenuDataItem(i);
    updateMenuDataItem(i);
    printMenuDataItem(i);
  }
}///void printMenuData()

void printMenuDataItem(byte i) {
  Serial.print("printMenuDataItem(): ");
  Serial.println(i);
  menuDataPosX[i] = int16_t(SCREEN_WIDTH - (menuData[i].length() + 1) * CHR_WIDTH);
  display.setCursor(menuDataPosX[i], menuPosY[i]);
  display.setTextColor(ST77XX_WHITE);
  display.print(menuData[i]);
}///void printMenuDataItem(byte: i)

void clearMenuDataItem(byte i) {
  Serial.print("clearMenuDataItem(): ");
  Serial.println(i);
  display.setCursor(menuDataPosX[i], menuPosY[i]);
  display.setTextColor(ST77XX_BLACK);
  display.print(menuData[i]);   //clear the previous data
}///void clearMenuDataItem(byte: i)

void switchLaser() {
  digitalWrite(PIN_LASER, laserON);
}
