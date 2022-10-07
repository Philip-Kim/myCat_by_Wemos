IPAddress timeServerIP; // time.google.com NTP server address
const char* ntpServerName = "time.google.com";

const int PortNTP = 123; // Port number for NTPS is 123

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

void sync_NTP() {
  Serial.println("start sync_NTP");
  udp.begin(PortNTP);

  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  int count_sendPacket_retry = 0;
  for (count_sendPacket_retry = 0; count_sendPacket_retry < 10; count_sendPacket_retry++) {
    check_WiFi_connected();
    sendNTPpacket(timeServerIP);
    Serial.println("Sent packet, waiting for reponse...");
    delay(1000);
    if ( udp.parsePacket() ) break;
  }
  if ( count_sendPacket_retry == 10) { resetFunc(); }

  Serial.println("Received packet!");

  millis_at_epoch = millis(); //check millis at time received
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  //now convert NTP time into everyday time:
  //Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  //subtract seventy years:
  epoch = secsSince1900 - seventyYears;

  temp_hour = (epoch % 86400L) / 3600;
  temp_minute = (epoch % 3600) / 60;
  temp_second = epoch % 60;
  time_now_in_sec = epoch;

  temp_day = epoch / 86400L;

  udp.stop();
  Serial.println("sync finished");
  print_UTC();
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, PortNTP); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void print_UTC(){
  Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
  Serial.print(temp_hour); // print the hour (86400 equals secs per day)
  Serial.print(':');
  if ( temp_minute < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.print(temp_minute); // print the minute (3600 equals secs per minute)
  Serial.print(':');
  if ( temp_second < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.println( temp_second ); // print the second  
}
