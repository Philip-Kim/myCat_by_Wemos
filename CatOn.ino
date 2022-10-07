#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

void(* resetFunc) (void) = 0;

unsigned long epoch;
unsigned long millis_at_epoch = 0;
unsigned long time_now_in_sec = 0;

unsigned long time_to_sync = 20; //every 5 o'clock (29HR - 9HR)

unsigned long temp_day = 0;
unsigned long temp_day_before = 0;

unsigned long temp_hour = 0;

unsigned long temp_minute = 0;

unsigned long temp_second = 0;

bool is_light_on = false;

const int pin_Light = D3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  pinMode(pin_Light, OUTPUT);
  digitalWrite(pin_Light, LOW);
  
  setup_WiFi();

  sync_NTP();
  
  temp_day_before = temp_day - 1;
}

void loop() {
  // put your main code here, to run repeatedly:

  //retry on not wl_connected
  check_WiFi_connected();

  remote_CAT();

  time_now_in_sec = epoch + ( (millis() - millis_at_epoch) / 1000 );
  
  temp_hour = (time_now_in_sec % 86400L) / 3600;
  temp_minute = (time_now_in_sec % 3600) / 60;
  temp_second = time_now_in_sec % 60;

  temp_day = time_now_in_sec / 86400L;
 
  if ( (temp_hour == time_to_sync) && (temp_day != temp_day_before) ){
    temp_day_before = temp_day;
    Serial.println("05:00LT, start sync_NTP()...");
    sync_NTP();
  }

  //14 days from boot, restart
  if ( (millis()/1000) / 86400L == 14) {
    Serial.println("Scheduled System Reset...");
    resetFunc();    
  }

  // at 21:00LT(==12:00UTC), light on
  if ( ( (12 <= temp_hour) && (temp_hour < 16) ) && !is_light_on ) {
    digitalWrite(pin_Light, HIGH);
    is_light_on=true;
    Serial.println("21:00LT~00:59LT, light is On");
  }

  // at 01:00LT(==16:00UTC), light off
  if ( temp_hour == 16 && temp_minute == 0 && temp_second == 0 && is_light_on) {
    actionUnlit();
    is_light_on = false;
    Serial.println("01:00LT, light is Off");
  }
}

void actionUnlit() {
  for ( int i = 0 ; i < 3 ; i++ ) {
    for ( int y = 0 ; y < 4 ; y++ ) {
      digitalWrite( pin_Light, (y+2) % 2 );
      delay( 1000 / (2^i) );
    }
  }
  
  digitalWrite(pin_Light, LOW);
}
