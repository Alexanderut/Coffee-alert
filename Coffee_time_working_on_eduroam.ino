
/* 
 *  This is my first shareworthy esp32 projects. I wanted to make an alert system for when the coffee was ready. I used an esp32 and a KY 013 heat sensor.
 *  Rundown:
 *  1. connect to WPA2 enterprise network. I'm on the eduroam university network. Here I was able to copy some code from Jeron Beemster that worked perfectly. WPA2 code example I copied: https://esp32.com/viewtopic.php?t=3108
 *  2. Getting the heat sensor to work and send a signal when resistance was above threshold. 
 *  3. Sending email. I did not manage to send an email from the ESP32, si instead I followed a guide that used ifttt. guide I followed: https://www.youtube.com/watch?v=ydqHbY7IPgg
 *  
 *  
 *  I'm a total novice to arduino and esp32 and writing code in general, so any feedback or advice is gladly welcome=)

 */

 ///////////////////////////////////  wifi and WPA setup

#include "esp_wpa2.h"
#include <WiFi.h>

const char* ssid = "eduroam-2.4GHz"; // your ssid
#define EAP_ID "Xxxx"
#define EAP_USERNAME "xxxxx"
#define EAP_PASSWORD "xxxxx"


///////////////////////////////////////     Email and ifttt setup
WiFiClient client;


String MakerIFTTT_Key ;
;String MakerIFTTT_Event;
char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;char *content_length_here;char *json_start;int compi;




/////////////////////////////////////////     Heat sensor setup
int sensorPin = 35;
#define LED 2                         //      Also wanted internal led to light up when running email code.


void setup() {
    Serial.begin(115200);
    delay(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    // WPA2 enterprise magic starts here
    WiFi.disconnect(true);      
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ID, strlen(EAP_ID));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); 
    esp_wifi_sta_wpa2_ent_enable(&config);
    // WPA2 enterprise magic ends here


    WiFi.begin(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

pinMode(LED,OUTPUT);


}
void loop()
{
 //getting the voltage reading from the temperature sensor
 int reading = analogRead(sensorPin);  
 
 // converting that reading to voltage
 float voltage = reading * 5.0;
 voltage /= 1024.0; 
 
 // print out the voltage
 Serial.print(voltage); Serial.println(" volts");
 
 // now print out the temperature ///////////////////////// This convertion is total BS and makes no sense, but leaving it for future tinkering. 
 float temperatureC = (voltage - 0.5) * 3.5 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
 Serial.print(temperatureC); Serial.println(" degrees C");

 if (voltage > 6.2 ){               ///////////////////////// when heat sensor is close to the coffee pot the voltage drops to about 5.5-6.5 depending on how full the pot is and placement of sensor.
  digitalWrite(LED, LOW);
 }
else 
 {
    digitalWrite(LED, HIGH);         /////////// Turning on internal LED for 10 sek when sending email core runs. 
    
    Serial.println("Sending Email, shutting down for 60min");

                                                      //////////////////////////////// The below code is spit our from www.easycoding.tn
    client.connect("maker.ifttt.com",80);
    MakerIFTTT_Key ="ekTDtehoEXGQYkukzDejCB8Mtlo5fGmZpqueO-Gyzws";    //////////////// trigger key from IFTTT
    MakerIFTTT_Event ="mail";
    p = post_rqst;
    p = append_str(p, "POST /trigger/");
    p = append_str(p, MakerIFTTT_Event);
    p = append_str(p, "/with/key/");
    p = append_str(p, MakerIFTTT_Key);
    p = append_str(p, " HTTP/1.1\r\n");
    p = append_str(p, "Host: maker.ifttt.com\r\n");
    p = append_str(p, "Content-Type: application/json\r\n");
    p = append_str(p, "Content-Length: ");
    content_length_here = p;
    p = append_str(p, "NN\r\n");
    p = append_str(p, "\r\n");
    json_start = p;
    p = append_str(p, "{\"value1\":\"");
    p = append_str(p, "coffee");
    p = append_str(p, "\",\"value2\":\"");
    p = append_str(p, "kaffen er klar");
    p = append_str(p, "\",\"value3\":\"");
    p = append_str(p, "... mvh, esp32");
    p = append_str(p, "\"}");

    compi= strlen(json_start);
    content_length_here[0] = '0' + (compi/10);
    content_length_here[1] = '0' + (compi%10);
    client.print(post_rqst); 

    delay(10000);
    digitalWrite(LED, LOW);
    delay(3600000);                                // w8ing 60min before restart, one cup of coffee per hour should be more than enough=)
    }
 delay(2000);                                     //waiting 2 seconds
    

  }


