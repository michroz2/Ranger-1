void setup()
{
  Serial.begin( 115200);   // Intialize terminal serial port for debugging
  delay(20);               // Give port time to initalize

  initLidar();
  initLoRa();
  setLoRaParams();
  initDisplay();

}/////setup()

void initLidar() {
  mySerial.begin( 115200);  // Initialize SW serial port for LIDAR
  delay(20);               // Give port time to initalize

  lidar.begin( &mySerial);   // Initialize device library object and...
  // pass device serial port to the object.

  // Send some example commands to the TFMini-Plus
  // - - Perform a system reset - - - - - - - - - - -
  //    Serial.print( "System reset: ");
  if ( lidar.sendCommand( SOFT_RESET, 0))
  {
    //        Serial.print( "LIDAR RESET passed.\r\n");
  }
  else lidar.printReply();
  delay(500);  // added to allow the System Rest enough time to complete

  // - - Display the firmware version - - - - - - - - -
  //    Serial.print( "Firmware version: ");
  //  if ( lidar.sendCommand( GET_FIRMWARE_VERSION, 0))
  //  {
  //        Serial.print( "%1u.", lidar.version[ 0]); // print three single numbers
  //        Serial.print( "%1u.", lidar.version[ 1]); // each separated by a dot
  //        Serial.print( "%1u\r\n", lidar.version[ 2]);
  //  }
  //  else lidar.printReply();
  // - - Set the data frame-rate to 20Hz - - - - - - - -
  //    Serial.print( "Data-Frame rate: ");
  if ( lidar.sendCommand( SET_FRAME_RATE, FRAME_500))
  {
    //lidar.sendCommand( SAVE_SETTINGS, 0)
    //        Serial.print( "%2uHz.\r\n", FRAME_20);
  }
  else lidar.printFrame();
  // - - - - - - - - - - - - - - - - - - - - - - - -
  delay(500);            // And wait for half a second.

}////void initLidar()

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

void initDisplay() {

//Для того, чтобы в новой библиотеке символ 0xB0 (=176)
//отображался (в кодировке Windows-1251 ему соответствует символ градуса)
//нужно в начале скетча (после инициализации дисплея)
//применить функцию cp437() с аргументом true:
  display.cp437(true);
  // if the display has CS pin try with SPI_MODE0:
  //  display.init(240, 240, SPI_MODE2);    // Init ST7789 display 240x240 pixel
  display.initR(INITR_BLACKTAB);    // Init ST7735 display 128x160 pixel
  display.setRotation(1);   // if the screen is rotated/flipped, update this command in range: (0-3)
  display.fillScreen(ST77XX_BLACK);   //Clear screen
  display.setTextWrap(true);
  display.setTextSize(2);
  display.enableDisplay(true);


  //Init screen labels and data cursor positions:
  display.setCursor(0, 0);
  display.setTextColor(ST77XX_RED);
  char dist = "Дистанция(см)";
  display.println(utf8rus(dist));
  distCursorX = display.getCursorX();
  distCursorY = display.getCursorY();
  display.println();
  display.setTextColor(ST77XX_BLUE);
  display.println(utf8rus("Яркость"));
  fluxCursorX = display.getCursorX();
  fluxCursorY = display.getCursorY();
  display.println();
  display.setTextColor(ST77XX_GREEN);
  display.println(utf8rus("Темпер.(°С)"));
  tempCursorX = display.getCursorX();
  tempCursorY = display.getCursorY();

}////void initDisplay()
