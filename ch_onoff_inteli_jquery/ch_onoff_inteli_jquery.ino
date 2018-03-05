/*********
  Based on
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
String webPageF = "";
String webPageOn = "";
String webPageOff = "";
String jsonSuccess = "";
String jsonFail = "";
String jsonOn = "";
String jsonOff = "";
String switchOff = "";
String switchOn = "";

int state = false ;
int gpio0_pin = D6;
int gpio2_pin = D7;
int gpio0_in = 0;
int gpio2_in = 0;
int state0;
int state2;
int timemod = 0;
int mills = 0;
String flipmode = "";
//const char* gpio0_in = "LOW";
//const char* gpio2_in = "LOW";

void setup(void){

  switchOff += "<img width=\"60\" height=\"60\" src=\"data:image/jpeg;base64,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\" />";
  switchOn += "<img width=\"60\" height=\"60\" src=\"data:image/jpeg;base64,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\"/>";
  
  webPage += "<!doctype html><html><meta charset=\"utf-8\"><title>Heating Control</title>";
  webPage += "<script type=\"text/javascript\" src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>";
  webPage += "<style media=\"screen\" type=\"text/css\">";
  webPage += "#header { text-align: center; clear:both; float:top; width:100%; background:#162; color:#FFF; font-family: arial; font-weight: bold padding:0.3em 15px 0.3em 15px;";
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
  webPage += ".button:active span { -webkit-transform: translate(0, 4px); -moz-transform: translate(0, 4px); -o-transform: translate(0, 4px); transform: translate(0, 4px);}</style><body>";

  webPage += "<script>";
  webPage += "$(document).ready(function(){";
  webPage += "$('#buttonOff').hide(); ";
  webPage += "$('#imgOn').hide();";
  webPage += "  $('.button').click(function() {";
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
  webPage += "      $('#imgOn').show();";
 webPage += "       $('#imgOff').hide();";
  webPage += "    } else {";
  webPage += "      $('#buttonOn').show(); ";
  webPage += "      $('#buttonOff').hide(); ";
  webPage += "      $('#imgOn').hide();";
  webPage += "      $('#imgOff').show();";
  webPage += "        }";
  /**webPage += "          $(\"#message\").html(themode);";*/
  webPage += "        },";
  webPage += "       error: function(error) {";
  webPage += "              console.log(error);";
  webPage += "        }";
  webPage += "      });";
  webPage += "  });";
  webPage += "}); </script>";
  
  webPage += "<div id=\"header\"><H1>THE HEATING</H1></div>";
  webPage += "<div id=\"imgOff\">";
  webPage += switchOff;
  webPage += "</div><div id=\"imgOn\">";
  webPage += switchOn;
  webPage += "</div>";
  webPage += "<form id=\"myForm\" name=\"myForm\" action=\"switch\" method=\"post\">";
  webPage += "<input type=\"checkbox\" name=\"toggle\" id=\"toggle\" data-toggle=\"toggle\" data-off=\"Off\" data-on=\"On\" checked style=\"display:none\">";
  webPage += "<div id=\"buttonOn\"><a href=\"#\" class=\"button\" id=\"buttonOn\"/>";
  webPage += "<span>Turn On</span>";
  webPage += "</a></div>";
  webPage += "<div id=\"buttonOff\"><a href=\"#\" class=\"button\" id=\"buttonOff\"/>";
  webPage += "<span>Turn Off</span>";
  webPage += "</a></div></div>";
  webPage += "<div id=\"message\" class=\"message\"><P>yo</P></div></form>";

  webPage += "</body></html>";
  
  jsonSuccess += "{ \"success\": true }";
  jsonFail += "{ \"success\": false }";
  jsonOn += "{ \"success\": \"true\", \"mode\": \"true\" }";
  jsonOff += "{ \"success\": \"true\", \"mode\": \"false\" }";

  
  // preparing GPIOs
  pinMode(gpio0_pin, OUTPUT);
  digitalWrite(gpio0_pin, LOW);
  pinMode(gpio2_pin, OUTPUT);
  digitalWrite(gpio2_pin, LOW);
  
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

  //pinMode(gpio0_pin, INPUT);
  //pinMode(gpio2_pin, INPUT);
  
  //state0 = digitalRead(gpio0_pin);
  //state2 = digitalRead(gpio2_pin);
  //if (state0 == LOW) 
  //{
  //  Serial.println("i reckon pin0 was low ");
  //} else {
  //  Serial.println("i reckon pin0 was high ");
  //}
  //pinMode(gpio0_pin, OUTPUT);
  //pinMode(gpio2_pin, OUTPUT);
    server.send(200, "text/html", webPage);
  });

  
  server.on("/switch", [](){
    if (server.arg("mode") == "true" ){
      server.sendHeader("Access-Control-Allow-Origin", "*");
      //pinMode(gpio0_pin, OUTPUT);
      //pinMode(gpio2_pin, OUTPUT);
      digitalWrite(gpio0_pin, HIGH);
      delay(500);
      digitalWrite(gpio2_pin, HIGH);
      //pinMode(gpio0_pin, INPUT);
      //pinMode(gpio2_pin, INPUT);
      //if ((digitalRead(gpio0_pin) == HIGH) && (digitalRead(gpio2_pin) == HIGH)) {
      server.send(200, "application/json", jsonOn);
      //} else {
      //  server.send(200, "application/json", jsonFail);
      //}
      //pinMode(gpio0_pin, OUTPUT);
      //pinMode(gpio2_pin, OUTPUT);
    } else 
      {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      //pinMode(gpio0_pin, OUTPUT);
      //pinMode(gpio2_pin, OUTPUT);
      digitalWrite(gpio0_pin, LOW);
      delay(500);
      digitalWrite(gpio2_pin, LOW);
      //pinMode(gpio0_pin, INPUT);
      //pinMode(gpio2_pin, INPUT);
      //if ((digitalRead(gpio0_pin) == LOW) && (digitalRead(gpio2_pin) == LOW)) {
        server.send(200, "application/json", jsonOff);
      //} else {
      //  server.send(200, "application/json", jsonFail);
      //}
      //pinMode(gpio0_pin, OUTPUT);
      //pinMode(gpio2_pin, OUTPUT);
    }
  });

  
   server.on("/On", [](){
    //pinMode(gpio0_pin, OUTPUT);
    //pinMode(gpio2_pin, OUTPUT);
    digitalWrite(gpio0_pin, HIGH);
    delay(1100);
    digitalWrite(gpio2_pin, HIGH);
    state = 1 ;
    //if ((digitalRead(gpio0_pin) == LOW) && (digitalRead(gpio2_pin) == LOW)) {
      server.send(200, "application/json", jsonOn);
    //} else {
    //  server.send(200, "application/json", jsonFail);
    //}
  });

  
  server.on("/Off", [](){
    //pinMode(gpio0_pin, OUTPUT);
    //pinMode(gpio2_pin, OUTPUT);
    digitalWrite(gpio0_pin, LOW);
    delay(1100);
    digitalWrite(gpio2_pin, LOW);
    state = 0 ;
    //if ((digitalRead(gpio0_pin) == LOW) && (digitalRead(gpio2_pin) == LOW)) {
      server.send(200, "application/json", jsonOff);
    //} else {
    //  server.send(200, "application/json", jsonFail);
    //}
  });
  server.begin();
  Serial.println("HTTP REST server started");
}
 
void loop(void){
  server.handleClient();
  /**mills = int(millis());
  timemod = mills % 5000 ;
  if (timemod == 0 ) {
    Serial.println("5 secs passed");
    Serial.print("State is: ");
    Serial.println(state);
  }
  Serial.println(int(millis()));
  
  Serial.println(state);*/
} 

