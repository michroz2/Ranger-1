int loopnum;
int sensitivity = 3;
int16_t avrDist;

void loop()
{
  if ( lidar.getData( curDist, curFlux, curTemp)) // Get data from the device.
  {
    /*
          String message;
          message += "Dist=";
          message += curDist;    // display distance,
          message += "cm\t";
          message += "Flux=";
          message += curFlux;   // display signal strength/quality,
          message += "\t";
          message += "Temp=";
          message += curTemp;   // display temperature,
          message += "°C";
          message += "\r\n";                   // end-of-line.
    */
    //      Serial.print(message);

//    Serial.print(millis());
//    Serial.print("\t");
    Serial.print(curDist);
    Serial.print(",");
    avrDist += (uint32_t)(((uint32_t)curDist << 4) - avrDist) >> sensitivity;
//    Serial.print(double(curDist-avrDist));
//    Serial.print(",");
//    Serial.print(sqrt(double(curDist-avrDist)));
//    Serial.print(",");
//    Serial.print(sqrt(double(curDist-avrDist)) / numAverage);
//    Serial.print(",");
    Serial.println(avrDist>>4);


    loopnum++;
    EVERY_MS(100) {
//Convert distance into string: "?X.XX" for 7-segment 4-digit monitor
      String sendDist = (curDist < 1000) ? " " : ""; //leading space if needed
      float curDistMeters = float(curDist) / 100; //convert to meters
      sendDist += curDistMeters; //add space if distance < 10 meters 
    //Send data over LoRa
      sendMessage(sendDist);
//      sendMessage(curDist);
//      Serial.print(millis());
//      Serial.print("\t");
//      Serial.println("Loops: " + String(loopnum));
      loopnum = 0;
      //Display data on TFT:
      //Erase previous values by writing previous values in black,
      //write new values with white:
      display.setCursor(distCursorX, distCursorY);
      display.setTextColor(ST77XX_BLACK);
      display.print(prevDist);
      display.setCursor(distCursorX, distCursorY);
      display.setTextColor(ST77XX_WHITE);
      display.print(curDist);

      display.setCursor(fluxCursorX, fluxCursorY);
      display.setTextColor(ST77XX_BLACK);
      display.print(prevFlux);
      display.setCursor(fluxCursorX, fluxCursorY);
      display.setTextColor(ST77XX_WHITE);
      display.print(curFlux);

      //      display.setCursor(tempCursorX, tempCursorY);
      //      display.setTextColor(ST77XX_BLACK);
      //      display.println(prevTemp);
      //      display.setCursor(tempCursorX, tempCursorY);
      //      display.setTextColor(ST77XX_WHITE);
      //      display.println(curTemp);

      prevDist = curDist;
      prevFlux = curFlux;
//      prevTemp = curTemp;
    }
  }
  else                  // If the command fails...
  {
    //    lidar.Serial.printrame();  // display the error and HEX data
  }
  //  }
}//////loop()
