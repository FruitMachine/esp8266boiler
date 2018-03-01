/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

// Replace with your network credentials
const char* ssid = "WIFI SSID";
const char* password = "WIFI PASSWORD";

ESP8266WebServer server(80);

String webPage = "";

int gpio0_pin = D6;
int gpio2_pin = D7;
int gpio0_in = 0;
int gpio2_in = 0;
//const char* gpio0_in = "LOW";
//const char* gpio2_in = "LOW";

void setup(void){
  webPage = "<!doctype html><html><meta charset=\"utf-8\"><title>Heating Control</title>";
  webPage += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script><style media=\"screen\" type=\"text/css\">";
  webPage += "#header { clear:both; float:top; width:100%; background:#162; color:#FFF; font-family: arial; border-bottom:1px solid #000; padding:0.3em 15px 0.3em 15px;";
  webPage += "margin:0; text-align:left;} #main { width:100%; background:#FFF; color:#000; font-family: arial; padding: 40px; } body { text-align: center; }";
  webPage += ".button {display: inline-block; margin: 10px; -webkit-border-radius: 8px; -moz-border-radius: 8px; border-radius: 8px; ";
  webPage += "-webkit-box-shadow: 0 8px 0 #c5376d, 0 15px 20px rgba(0, 0, 0, .35); -moz-box-shadow: 0 8px 0 #c5376d, 0 15px 20px rgba(0, 0, 0, .35);";
  webPage += " box-shadow: 0 8px 0 #c5376d, 0 15px 20px rgba(0, 0, 0, .35); -webkit-transition: -webkit-box-shadow .1s ease-in-out;";
  webPage += " -moz-transition: -moz-box-shadow .1s ease-in-out; -o-transition: -o-box-shadow .1s ease-in-out; transition: box-shadow .1s ease-in-out;";
  webPage += "font-size: 50px; color: #fff; }";
  webPage += ".button span { display: inline-block; padding: 20px 30px; background-color: #ec528d; ";
  webPage += "background-image: -webkit-gradient(linear, 0% 0%, 0% 100%, from(hsla(338, 90%, 80%, .8)), to(hsla(338, 90%, 70%, .2)));";
  webPage += "background-image: -webkit-linear-gradient(hsla(338, 90%, 80%, .8), hsla(338, 90%, 70%, .2));";
  webPage += "background-image: -moz-linear-gradient(hsla(338, 90%, 80%, .8), hsla(338, 90%, 70%, .2));";
  webPage += "background-image: -o-linear-gradient(hsla(338, 90%, 80%, .8), hsla(338, 90%, 70%, .2));";
  webPage += " -webkit-border-radius: 8px; -moz-border-radius: 8px; border-radius: 8px;-webkit-box-shadow: inset 0 -1px 1px rgba(255, 255, 255, .15);";
  webPage += " -moz-box-shadow: inset 0 -1px 1px rgba(255, 255, 255, .15); box-shadow: inset 0 -1px 1px rgba(255, 255, 255, .15);";
  webPage += " font-family: Arial, sans-serif; line-height: 1; text-shadow: 0 -1px 1px rgba(175, 49, 95, .7);";
  webPage += " -webkit-transition: background-color .2s ease-in-out, -webkit-transform .1s ease-in-out;";
  webPage += " -moz-transition: background-color .2s ease-in-out, -moz-transform .1s ease-in-out;";
  webPage += " -o-transition: background-color .2s ease-in-out, -o-transform .1s ease-in-out;";
  webPage += " transition: background-color .2s ease-in-out, transform .1s ease-in-out; }";
  webPage += ".button:hover span { background-color: #ec6a9c;text-shadow: 0 -1px 1px rgba(175, 49, 95, .9), 0 0 5px rgba(255, 255, 255, .8);}";
  webPage += ".button:active, .button:focus { -webkit-box-shadow:    0 8px 0 #c5376d, 0 12px 10px rgba(0, 0, 0, .3);";
  webPage += "    -moz-box-shadow: 0 8px 0 #c5376d, 0 12px 10px rgba(0, 0, 0, .3); box-shadow:    0 8px 0 #c5376d, 0 12px 10px rgba(0, 0, 0, .3); }";
  webPage += ".button:active span { -webkit-transform: translate(0, 4px); -moz-transform: translate(0, 4px); -o-transform: translate(0, 4px); transform: translate(0, 4px);}";
  webPage += "</style><body><P><a href=\"/On\" class=\"button\"><span>Heating On</span></a></P>";
  webPage += "<P><a href=\"/Off\" class=\"button\"><span>Heating Off</span></a></P></body></html>";
  
  // preparing GPIOs
  pinMode(gpio0_pin, OUTPUT);
  digitalWrite(gpio0_pin, LOW);
  pinMode(gpio2_pin, OUTPUT);
  digitalWrite(gpio2_pin, LOW);
  //webPage += "<h1>ESP8266 Web Server</h1><p>Socket #1 <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></p>";
  //webPage += "<p>Socket #2 <a href=\"socket2On\"><button>ON</button></a>&nbsp;<a href=\"socket2Off\"><button>OFF</button></a></p>";
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }


    
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/On", [](){
    server.send(200, "text/html", webPage);
    digitalWrite(gpio0_pin, HIGH);
    delay(400);
    digitalWrite(gpio2_pin, HIGH);
    delay(1000);
  });
  server.on("/Off", [](){
    server.send(200, "text/html", webPage);
    digitalWrite(gpio0_pin, LOW);
    delay(400);
    digitalWrite(gpio2_pin, LOW);
    delay(1000); 
  });
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
} 

