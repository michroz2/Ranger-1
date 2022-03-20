void sendMessage(String outgoing) {
//  digitalWrite(LED_BUILTIN, HIGH);
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
//  digitalWrite(LED_BUILTIN, LOW);
}
