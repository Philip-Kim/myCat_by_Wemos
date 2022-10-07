#ifndef STASSID
#define STASSID "SSID"
#define STAPSK  "PASS"
#endif

const char * ssid = STASSID; // your network SSID (name)
const char * pass = STAPSK;  // your network password

void setup_WiFi() {
  // We start by connecting to a WiFi network
  delay(500);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
  
}

void check_WiFi_connected() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost, trying re-connect...");
    WiFi.disconnect();
    setup_WiFi();
  }
}
