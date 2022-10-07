const int PortRemoteCat = 59595; // Port number for remoteCat is 59595;

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp_cat;

void remote_CAT() {
  if (udp_cat == NULL) {
    Serial.println("Starting remote_Cat..");
    udp_cat.begin(PortRemoteCat);
  }

  if( udp_cat.parsePacket() ){
    char packetBuffer_cat[255];
    int len = udp_cat.read(packetBuffer_cat, 255);
    if (len > 0){
      packetBuffer_cat[len] = 0;
    }
    Serial.print("Data Received : ");
    Serial.println(packetBuffer_cat);
    if (packetBuffer_cat[0] == '1') {
      digitalWrite(pin_Light, HIGH);
      Serial.println("Cat is on by remote");
    }
    else if (packetBuffer_cat[0] == '0') {
      digitalWrite(pin_Light, LOW);
      Serial.println("Cat is off by remote");
    }
  }
}
