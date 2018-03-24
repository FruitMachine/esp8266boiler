/*
 Udp NTP Client
 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket
 For more on NTP time servers and the messages needed to communicate with them,
 see http://en.wikipedia.org/wiki/Network_Time_Protocol
 created 4 Sep 2010
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe
 updated for the ESP8266 12 Apr 2015 
 by Ivan Grokhotkov
 This code is in the public domain.
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <TimeLib.h>

const char* ssid = "WIFI SSID";
const char* pass = "WIFI PASSWORD";


int hourCheck = 0;

unsigned int localPort = 2390;      // local port to listen for UDP packets

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */
IPAddress timeServerIP(192, 168, 5, 102); // time.nist.gov NTP server
//IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
MDNSResponder mdns;
ESP8266WebServer server(80);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;


String webPage = "";
String webPageF = "";

String webPageOn = "";
String webPageOff = "";
String jsonSuccess = "";
String jsonFail = "";
String jsonOn = "";
String jsonOff = "";
String jsonStateOn = "";
String jsonStateOff = "";
String switchOff = "";
String switchOn = "";
String timeString = "";

bool state = false ;
int gpio0_pin = D6;
int gpio2_pin = D7;
int gpio0_in = 0;
int gpio2_in = 0;
int state0;
int state2;
int timemod = 0;
int start = int(millis());
int mills = 0;
long onsince = 0;
long nowepoch = 0;

void setup()
{
  
  switchOff += "<img width=\"60\" height=\"60\" src=\"http://192.168.5.102/zone/switchOff.jpg\" />";
  switchOn += "<img width=\"60\" height=\"60\" src=\"http://192.168.5.102/zone/switchOn.jpg\" />";
  
  webPage += "<!doctype html><html><meta charset=\"utf-8\"><title>Heating Control</title>";
  webPage += "<script type=\"text/javascript\" src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>";
  webPage += "<style media=\"screen\" type=\"text/css\">";
  webPage += "#header { text-align: center;  background:#162; color:#FFF; font-family: arial; font-weight: bold;  }";
  webPage += "#main { width:100%; background:#FFF; color:#000; font-family: arial; padding: 40px; } body { text-align: center; }";
  webPage += "#corner { text-align: right; bottom:0;right:0; background:#FFF; font-family: arial; font-size: 8px; }"; 
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
  webPage += ".button:active span { -webkit-transform: translate(0, 4px); -moz-transform: translate(0, 4px); -o-transform: translate(0, 4px); transform: translate(0, 4px);}</style><body>";

  webPage += "<script>";
  webPage += "$(document).ready(function(){";
  /**
  webPage += "$.ajax({";
  webPage += "        type:'GET',";
  webPage += "        dataType:'JSON',";
  webPage += "        url:'state',";
  webPage += "        success:function(response)";
  webPage += "        {";
  webPage += "    console.log(response);";
  webPage += "          var response=eval(response);";
  webPage += "          theres=response.state;";
  webPage += "    if (theres == \"on\") {";
  webPage += "      $('#toggle').prop('checked', false) ; ";
  webPage += "    } else {";
  webPage += "      $('#toggle').prop('checked', true) ; ";
  webPage += "        }";
  webPage += "        },";
  webPage += "       error: function(error) { console.log(error); } } );";

*/
  webPage += "if ($('#toggle').prop('checked') == \"true\") {";
  webPage += "$('#buttonOn').hide(); ";
  webPage += "$('#imgOff').hide();";
  webPage += "$('#buttonOff').show(); ";
  webPage += "$('#imgOn').show();"; 
  webPage += "    } else {"; 
  webPage += "$('#buttonOn').show(); ";
  webPage += "$('#imgOff').show();";
  webPage += "$('#buttonOff').hide(); ";
  webPage += "$('#imgOn').hide();";   
  webPage += "        }"; 
  webPage += "  $('#buttonOn,#buttonOff').click(function() {";
  webPage += "  var mode=$('#toggle').prop('checked');";
  webPage += "  $('#toggle').prop('checked', ! mode);";
  webPage += "$.ajax({";
  webPage += "        type:'POST',";
  webPage += "        dataType:'JSON',";
  webPage += "        url:'switch',";
  webPage += "        data:'mode='+mode,";
  webPage += "        success:function(response)";
  webPage += "        {";
  webPage += "    console.log(response);";
  webPage += "          var response=eval(response);";
  webPage += "          themode=response.mode;";
  webPage += "    if (themode == \"true\") {";
  webPage += "      $('#buttonOn').hide(); ";
  webPage += "      $('#buttonOff').show(); ";
  webPage += "      $('#buttonFor').hide(); ";
  webPage += "      $('#imgOn').show();";
  webPage += "       $('#imgOff').hide();";
  webPage += "    } else {";
  webPage += "      $('#buttonOn').show(); ";
  webPage += "      $('#buttonOff').hide(); ";
  webPage += "      $('#buttonFor').show(); ";  
  webPage += "      $('#imgOn').hide();";
  webPage += "      $('#imgOff').show();";
  webPage += "        }";
  webPage += "        },";
  webPage += "       error: function(error) {";
  webPage += "              console.log(error);";
  webPage += "        }";
  webPage += "      });";
  webPage += "      });";
  webPage += "  $('#buttonFor').click(function() {";
  webPage += "  $('#toggle').prop('checked', false);";
  webPage += "$.ajax({";
  webPage += "        type:'POST',";
  webPage += "        dataType:'JSON',";
  webPage += "        url:'Onfor',";
  webPage += "        success:function(response)";
  webPage += "        {";
  webPage += "    console.log(response);";
  webPage += "          var response=eval(response);";
  webPage += "          themode=response.mode;";
  webPage += "    if (themode == \"true\") {";
  webPage += "      $('#buttonOn').hide(); ";
  webPage += "      $('#buttonOff').show(); ";
  webPage += "      $('#buttonFor').hide(); ";
  webPage += "      $('#imgOn').show();";
  webPage += "       $('#imgOff').hide();";
  webPage += "    } else {";
  webPage += "      $('#buttonOn').show(); ";
  webPage += "      $('#buttonOff').hide(); ";
  webPage += "      $('#buttonFor').show(); ";  
  webPage += "      $('#imgOn').hide();";
  webPage += "      $('#imgOff').show();";
  webPage += "        }";
  webPage += "        },";
  webPage += "       error: function(error) {";
  webPage += "              console.log(error);";
  webPage += "        }";
  webPage += "      });";
  webPage += "      });";
  webPage += "}); </script>";
  
  webPage += "<div id=\"header\"><H1>THE HEATING</H1></div>";
  webPage += "<div id=\"imgOff\">";
  webPage += switchOff;
  webPage += "</div><div id=\"imgOn\">";
  webPage += switchOn;
  webPage += "</div>";
  webPage += "<form id=\"myForm\" name=\"myForm\" action=\"switch\" method=\"post\">";
  webPage += "<div id=\"On\"><a href=\"#\" class=\"button\" id=\"buttonOn\"/>";
  webPage += "<span>Turn On</span>";
  webPage += "</a></div>";
  webPage += "<div id=\"Off\"><a href=\"#\" class=\"button\" id=\"buttonOff\"/>";
  webPage += "<span>Turn Off</span>";
  webPage += "</a></div>";
  webPage += "<div id=\"Onfor\"><a href=\"#\" class=\"button\" id=\"buttonFor\"/>";
  webPage += "<span>On for an hour</span>";
  webPage += "</a></div></div>"; 
  webPage += "<input type=\"checkbox\" name=\"toggle\" id=\"toggle\" data-toggle=\"toggle\" data-off=\"Off\" data-on=\"On\" checked >";
  webPage += "<div id=\"corner\" class=\"corner\"><a href=\"/\">Reset</a></div></form>";
  webPage += "</body></html>";


  
  jsonSuccess += "{ \"success\": true }";
  jsonFail += "{ \"success\": false }";
  jsonOn += "{ \"success\": \"true\", \"mode\": \"true\" }";
  jsonOff += "{ \"success\": \"true\", \"mode\": \"false\" }";
  jsonStateOn += "{ \"state\": \"on\" }";
  jsonStateOff += "{ \"state\": \"off\" }";
  
  // preparing GPIOs
  pinMode(gpio0_pin, OUTPUT);
  digitalWrite(gpio0_pin, LOW);
  pinMode(gpio2_pin, OUTPUT);
  digitalWrite(gpio2_pin, LOW);

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  unsigned long epoch = 0;
  while (epoch == 0) {
    epoch = getTime();
    Serial.println(epoch);
    delay(2000);
  }
  Serial.print("Found time. Epoch is = " );
  Serial.println(epoch);
  setTime(epoch);

  

    
  server.on("/", [](){
    server.send(200, "text/html", webPage);
    
  if (timeStatus()!= timeNotSet) { digitalClockDisplay(); } else { Serial.println("time unknown" ); }
  });

  
  server.on("/switch", [](){
    
    //server.sendHeader("Access-Control-Allow-Origin", "*");

    // if (server.arg("mode") == "true" ){
    if (state == false ){
      Serial.println("Request to toggle on");
      digitalWrite(gpio0_pin, HIGH);
      delay(300);
      digitalWrite(gpio2_pin, HIGH);
      server.send(200, "application/json", jsonOn);
    } else 
    {
      Serial.println("Request to toggle off");
      digitalWrite(gpio0_pin, LOW);
      delay(300);
      digitalWrite(gpio2_pin, LOW);
      server.send(200, "application/json", jsonOff);
    }
    // toggle state
    state = !state;
    
  });

  
  server.on("/On", [](){
    Serial.println("Request to turn on");
    digitalWrite(gpio0_pin, HIGH);
    delay(1100);
    digitalWrite(gpio2_pin, HIGH);
    state = true ;
    server.send(200, "application/json", jsonOn);
  });

  
  server.on("/Off", [](){
    Serial.println("Request to turn off");
    digitalWrite(gpio0_pin, LOW);
    delay(1100);
    digitalWrite(gpio2_pin, LOW);
    state = false ;
    server.send(200, "application/json", jsonOff);
  });

  server.on("/CHoff", [](){
    Serial.println("Request to turn p0 off");
    digitalWrite(gpio0_pin, LOW);
    server.send(200, "text/plain", "CH/pin0 off\n");
  });

  server.on("/HWoff", [](){
    Serial.println("Request to turn p2 off");
    digitalWrite(gpio2_pin, LOW);
    server.send(200, "text/plain", "HW/pin2 off\n");
  });
        
  server.on("/CHon", [](){
    Serial.println("Request to turn p0 on");
    digitalWrite(gpio0_pin, HIGH);
    server.send(200, "text/plain", "CH/pin0 on\n");
  });

  server.on("/HWon", [](){
    Serial.println("Request to turn p2 on");
    digitalWrite(gpio2_pin, HIGH);
    server.send(200, "text/plain", "HW/pin2 on\n");
  });

  server.on("/state", [](){
    String bobster = "";
    Serial.print("Request for state: ");
    Serial.println(state);
    if ( state == true ) {
        bobster = jsonStateOn;
    } else {
      bobster = jsonStateOff;
    }
    server.send(200, "application/json", bobster);
  });
  
  server.on("/Onfor", [](){
    Serial.println("Request to turn on for an hour");
    onsince = now(); // record epoch of now, then lookup later if an hour ago
    digitalWrite(gpio0_pin, HIGH);
    delay(200);
    digitalWrite(gpio2_pin, HIGH);
    state = true ;
    server.send(200, "application/json", jsonOn);
  });  

  server.on("/time", [](){
    Serial.println("Request for current time");
    jsonTime += "{ \"time\": \"";
    jsonTime += clockString();
    jsonTime += "\" } "";
    server.send(200, "application/json", jsonTime);
  });  

  server.on("/timeupd", [](){
    Serial.println("Request for ntp update");
    unsigned long epoch = 0;
  while (epoch == 0) {
    epoch = getTime();
    Serial.println(epoch);
    delay(2000);
  }
  Serial.print("Found time. Epoch is = " );
  Serial.println(epoch);
  setTime(epoch);
  jsonTime += "{ \"time\": \"";
  jsonTime += clockString();
  jsonTime += "\" } "";
  server.send(200, "application/json", jsonTime);
  });  

  server.begin();
  Serial.println("Heating controller started");
  if (timeStatus()!= timeNotSet) { digitalClockDisplay(); } else { Serial.println("time unknown" ); }
}

void loop()
{

server.handleClient();
//timeString = clockString();
//Serial.println(timeString);
if (onsince != 0) {
  nowepoch = now();
  if ((nowepoch - onsince) > 3600 ) {
    Serial.println("On since has passed, turning off");
    digitalWrite(gpio0_pin, LOW);
    delay(900);
    digitalWrite(gpio2_pin, LOW);
    state = false ;
    onsince = 0;
  }
}
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
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
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

long getTime()
{
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  while (!cb) {
    Serial.println("no packet yet");
    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(1000);
    cb = udp.parsePacket();
  }
  Serial.print("packet received, length=");
  Serial.println(cb);
  // We've received a packet, read the data from it
  udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, esxtract the two words:
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
  Serial.print("Seconds since Jan 1 1900 = " );
  Serial.println(secsSince1900);
  Serial.print("Unix time = ");
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long newepoch = secsSince1900 - seventyYears;
  // print Unix time:
  //Serial.println(newepoch);
  return newepoch;
}
void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

String clockString(){
  // string of the time
  String timeString;
  timeString += hour() + ":";
  timeString += minute() + ":";
  timeString += second() + " ";
  timeString += day() + "/";
  timeString += month() + "/";
  timeString += year();
  return timeString;
}


void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

