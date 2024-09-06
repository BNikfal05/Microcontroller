
/*********
 Want to permit only the first remote IP have access
*********/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <DHT.h>    // For Temperature and Humidity DHT madule
#include "time.h"
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;   //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 3600;  //Replace with your daylight offset (seconds)

#include <ESP8266HTTPClient.h>
int a=0;
const char *URL = "http://192.168.68.141/esp/esppost.php";
WiFiClient client;
HTTPClient httpClient;
String temp,hum,door,wsignal,Time;
String data;
String server = "192.168.68.141"; // www.example.com
String uri = "/esp/esppost.php";// our example is /esppost.php

#define INPUT_PIN 12  // Define Pin GPIO12 (D6) as input pin
#define wifiLed   16   //D0
#define DHTPIN 14     // Digital pin D5 connected to the DHT sensor     

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
// #define DHTTYPE    DHT22     // DHT 22 (AM2302)
// #define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
float tc = dht.readTemperature();
float hc = dht.readHumidity();  

// Bolean variable for digital input reads
bool statA = 0;
bool statB = 0;
bool inp = 0;
int m = 0;
int n = 0;


// Auxiliar variables to store the current output state
String output12State = "initial";
String state = "initial";
String output12StateP = "reboot";
String output12StateQ = "reboot";
String output12StateR = "reboot";
String output12StateS = "reboot";
String TimeC = "reboot";
String TimeD = "reboot";
String TimeE = "reboot";
String TimeF = "reboot";
String TimeS = "init";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// Variable to store the HTTP request
String header;


void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
    pinMode(wifiLed, OUTPUT);
    pinMode(INPUT_PIN, INPUT); // Configure pin INPUT_PIN as input
  // Set outputs to LOW
  digitalWrite(wifiLed, HIGH); // Initial WiFi LED off before connecting
      
    // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
   WiFiManager wifiManager;
  
  // Uncomment and run it once, if you want to erase all the stored information
    // wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
   digitalWrite(wifiLed, LOW); // LED is ON when WiFi Connected
}



void loop(){
delay(1000);
  inp=digitalRead(INPUT_PIN);
  
  
 if (a==0) {

//read temperature and humidity
float tp = tc; // Previous temp
float tc = dht.readTemperature(); //current temp

float hp = hc; // Previous Humidity
  float hc = dht.readHumidity();  //current humidity
  
  if (isnan(hc) || isnan(tc)) {
    Serial.println("Failed to read from DHT sensor!");
  }

// inp text definition
if (inp <= 0) {
  output12State = "Closed";
  state = "<FONT COLOR=green><h1>CLOSED</h1></FONT COLOR>";
  } else if (inp > 0) {
    output12State = "Open";
    state = "<FONT COLOR=red><h1>OPEN</h1></FONT COLOR>";
  }
  
delay(4000);
   // Save the door State and Time when state changes                   
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  String Time = asctime(timeinfo);

if (output12StateP != output12State) {
  m = m +1;
  n = n +1;
      TimeC = Time;
  if (m == 1) { 
    TimeF = TimeE;
    TimeE = TimeD; 
    TimeD = TimeC;
  output12StateS = output12StateR;
  output12StateR = output12StateP;
  if (n == 1) {
    output12StateQ = "reboot";
    TimeS = TimeC;
  }
  else {
     output12StateQ = output12State;
  }
  } 
if (m == 2){
  TimeF = TimeE;
  TimeE = TimeD;
  TimeD = TimeC;
  output12StateS = output12StateR;
 if (n == 2) {
    output12StateR = "reboot";
  }
  else {
     output12StateR = output12StateP;
  }
  output12StateQ = output12State;
} 
if (m == 3){
  TimeF = TimeE;
  TimeE = TimeD;
  TimeD = TimeC;
 if (n == 3) {
    output12StateS = "reboot";
  }
  else {
     output12StateS = output12StateR;
  }
  output12StateR = output12StateP;
  output12StateQ = output12State;
  m = 0;
} 
}
output12StateP = output12State; 

 // Display RSSI Value              

                                  if (WiFi.RSSI() < -78) {
                            wsignal = "WiFi Signal: Vey weak";        
                          } else if (WiFi.RSSI() < -70) {
                            wsignal = "WiFi Signal: Weak";
                          } else if (WiFi.RSSI() < -60) { 
                              wsignal = "WiFi Signal: Not so bad";
                          } else if (WiFi.RSSI() < -50) {
                              wsignal = "WiFi Signal: Good";
                          } else if (WiFi.RSSI() < -40) {
                              wsignal = "WiFi Signal: Very good";
                          } else if (WiFi.RSSI() < -30) {
                                    wsignal = "WiFi Signal: Excellent";  
                         } else if (WiFi.RSSI() < -20) {
                          wsignal = "WiFi Signal: Ultra good";
                         }

  temp = String(tc);
  hum = String(hc);
  door = String(output12State);
data = "temperature=" + temp + "&humidity=" + hum +  "&current_doorstate=" + door + "&current_time=" + Time + "&RSSI1=" + wsignal; // data sent must be under this form
 httppost();
      delay(1000); 
  }
     //    light_sleep();         
}

void httppost() {
    httpClient.begin(client, URL);
    httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
    httpClient.POST(data);
    Serial.println(data);
    String content = httpClient.getString();
    httpClient.end();
    Serial.println(content);
    delay(5000);
}

void light_sleep() {
  delay(4000);
  Serial.println("sleep procedure started");
  if (inp == statB) {
    a=1;
    wifi_station_disconnect();
     digitalWrite(wifiLed, HIGH); // LED is OFF when WiFi Sisconnected
    wifi_set_opmode_current(NULL_MODE);
 // wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
   wifi_fpm_open(); // Enables Force sleep
   wifi_fpm_do_sleep(20000000);
  delay(200);
    Serial.println("WiFi is Sleeping"); 
  }
  
if (inp != statB) {
    a=0;
  Serial.println("WiFi woke up");
   wifi_fpm_close();
   wifi_set_opmode(STATION_MODE);
   wifi_station_connect();
 initWifi();
  delay(100);
  Serial.println("end sleep routine commands");
  }
}

void initWifi() {
       digitalWrite(wifiLed, HIGH); // LED is OFF when WiFi Disconnected
    WiFi.mode(WIFI_STA); 
  WiFi.begin(WiFi.SSID(), WiFi.psk());
  wifi_set_sleep_type(LIGHT_SLEEP_T);
 Serial.print("Reconnecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  wifi_set_sleep_type(NONE_SLEEP_T);
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
   digitalWrite(wifiLed, LOW); // LED is ON when WiFi Connected
   statB = inp;
}
