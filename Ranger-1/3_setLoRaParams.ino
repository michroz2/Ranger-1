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

}////void setLoRaParams()
