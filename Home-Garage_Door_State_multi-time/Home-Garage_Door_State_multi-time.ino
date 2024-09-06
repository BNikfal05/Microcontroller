#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <DHT.h>    // For Temperature and Humidity DHT madule
#include "time.h"

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;   //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 3600;  //Replace with your daylight offset (seconds)

#define INPUT_PIN 12  // Define Pin GPIO12 (D6) as input pin
#define wifiLed   16   //D0
#define DHTPIN 14     // Digital pin D5 connected to the DHT sensor     

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);
 float tc = 99; // dht.readTemperature();
 float hc = 99; //dht.readHumidity();  

// Bolean variable for digital input reads
bool inp = 0;

int m = 0;
int n = 0;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

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

void setup() {
  Serial.begin(115200);
  
  // Initialize the output variables as outputs
    pinMode(wifiLed, OUTPUT);
    pinMode(INPUT_PIN, INPUT); // Configure pin INPUT_PIN as input
  // Set outputs to LOW
  digitalWrite(wifiLed, HIGH); // Initial WiFi LED off before connecting
        
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
   
   //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  server.begin();
}  
    
void loop(){
delay(300);
  inp=digitalRead(INPUT_PIN);
  WiFiClient client = server.available();   // Listen for incoming clients

// inp text definition
if (inp <= 0) {
  output12State = "Closed";
  state = "<FONT COLOR=green><h1>CLOSED</h1></FONT COLOR>";
  } else if (inp > 0) {
    output12State = "Open";
    state = "<FONT COLOR=red><h1>OPEN</h1></FONT COLOR>";
  }
delay(1500);
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
  
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client    
currentTime = millis();
     previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
   currentTime = millis(); 
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
           if (header.indexOf("GET /RESET") >= 0) {
                WiFiManager wifiManager;
              wifiManager.resetSettings();
              void sendReset() ;
  Serial.println("Sending Reset command");
  Serial.println(1/0);
            } else if (header.indexOf("GET /REBOOT") >= 0) {
              void sendReset() ;
  Serial.println("Sending Reboot command");
  Serial.println(1/0);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta http-equiv=\"refresh\" content=\"5\">"); //Refresh the page automatically every t secs
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><FONT COLOR=blue><h2>GARAGE DOOR</h2></FONT COLOR>");
    
                      client.println("<p>" + state + "</p>");
                      client.println("<p><h3><FONT COLOR=purple>" + Time + "</h3></FONT COLOR>");
                      
// print the state and time when state changes
   client.println("<hr><small>" + output12StateQ + " @ " + TimeC + "</small>");
    client.println("<br><small>" + output12StateR + " @ " + TimeE + "</small>");
      client.println("<br><small>" + output12StateS + " @ " + TimeF + "</small>"); 
            client.print("<small><br>Door opened "); 
            client.print(n/2);
             client.print(" times since " + TimeS + "</small>");
            client.println("<hr>"); 
            
                      // Display Temperature value
//read temperature and humidity
float tc = dht.readTemperature(); //current temp
  float hc = dht.readHumidity();  //current humidity

  client.print("Garage Temperature: <strong>");
  if (isnan(hc) || isnan(tc)) {
    Serial.println("Failed to read from DHT sensor!");
     Serial.println(tc);
    client.print(tc); 
  }                                                              
                     else {
                       Serial.println(tc);
                      client.print(tc);
                     } 
                      client.print(" 'C</strong>");
                          client.println("<br>");

                             // Display Humidity value
                     client.print("Garage Humidity: <strong>");
                      if (isnan(hc) || isnan(tc)) {
                         client.print(hc); 
                      }
                        else {
                      client.print(hc);
                        }
                      client.print(" %</strong>");
                            
        client.println("<hr>");
        
                             // Display remote IP
                          client.print("<small>Remote IP Address:");
                      client.print(client.remoteIP());
                        client.println("<br>"); 
                              Serial.print("Remote client with IP: ");
                              Serial.print(client.remoteIP());
                              Serial.println("connected");
                                // Display local IP
                          client.print("Local IP Address:");
                      client.print(WiFi.localIP());
                        client.println("</small><br>"); 
                              
            // Display RSSI Value              
                                  if (WiFi.RSSI() < -78) { 
                                 client.println("WiFi Signal level <progress value=\"10\" max=\"90\"> </progress>"); 
                          } else if (WiFi.RSSI() < -70) {
                            client.println("WiFi Signal level <progress value=\"20\" max=\"90\"> </progress>");
                          } else if (WiFi.RSSI() < -60) { 
                              client.println("WiFi Signal level <progress value=\"40\" max=\"90\"> </progress>");
                          } else if (WiFi.RSSI() < -50) {
                                client.println("WiFi Signal level <progress value=\"50\" max=\"90\"> </progress>");
                          } else if (WiFi.RSSI() < -40) {
                               client.println("WiFi Signal level <progress value=\"70\" max=\"90\"> </progress>"); 
                          } else if (WiFi.RSSI() < -30) {
                                    client.println("WiFi Signal level <progress value=\"80\" max=\"90\"> </progress>"); 
                         } else if (WiFi.RSSI() < -20) {
                         client.println("WiFi Signal level <progress value=\"90\" max=\"90\"> </progress>");
                          } else if (WiFi.RSSI() < 0) {
                          client.println("WiFi Signal level <progress value=\"100\" max=\"90\"> </progress>");
                         }
                                client.println("<hr>");

  // Display reset if the remote IP and WiFi IP are the same range (they are at the same network, or close)
IPAddress WiFiIP = WiFi.localIP();
IPAddress RemoteIP = client.remoteIP();
              if (WiFiIP[0]==RemoteIP[0] && WiFiIP[1]==RemoteIP[1]) {  
                        
                                     // Display REBOOT, button  
        // client.println("<p><small>Push REBOOT to Restart <a href=\"/REBOOT\"><button class=\"btn-sm\">REBOOT</button></a></small></p>");
        }
              else {
               // client.println("<p><small>Reboot button has been disabled. The remote client is not at the same network of the Device</small></p>");
              }
               
  // Display reset if the remote IP and WiFi IP are the same range (they are at the same network, or close)
              if (WiFiIP[0]==RemoteIP[0] && WiFiIP[1]==RemoteIP[1]) {  
                        
                                     // Display RESET, button  
         client.println("<p><small>Push RESET to Reconfigure, (attention: You should be close to the device for reset) <a href=\"/RESET\"><button class=\"btn-sm\">RESET</button></a></small></p>");
        }
              else {
                client.println("<p><small>RESET button has been disabled. The remote client is not at the same network of the Device</small>");
              }
              client.println("<hr>");
              client.println("<small>Designed by Babak & Borna Nikfal, nikfal@gmail.com</small></p>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }     
      }      
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
