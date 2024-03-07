#include "DHT.h"
#include <WiFi.h>
#include "ESPAsyncWebSrv.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include "SPIFFS.h"
#include <NTPClient.h>
#include <WiFiUdp.h>


const char* ssid = "pokoj123";
const char* password = "pokoj123";

#define DHTPIN 4  
#define TEMT6000 34
#define DHTTYPE DHT22 

/*WiFiUDP ntpUDP;

#define NTP_OFFSET  3600  

#define NTP_INTERVAL 60 * 1000   

#define NTP_ADDRESS  "tempus1.gum.gov.pl"

NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);*/



int inputPin = 5;
int inputaudio = 35; 
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;

const int output26=26;
const int output27=25;

int stanauto=0;
int stanauto2=0;

float h;
float t;
float t_bmp;
float pre_bmp;
float lux;
String ruch;
String audio;
String output26State = "off";
String output27State = "off";
String formattedTime;
int godzina;
int czaszapalania;


AsyncWebServer server(80);
AsyncEventSource events("/events");

unsigned long lastTime = 0;  
unsigned long timerDelay = 3000;

void DHT_mes(){

  h = dht.readHumidity();
  t = dht.readTemperature();
 
  if (isnan(h) || isnan(t) ) {
    Serial.println(F("blad pomiaru!"));
    return;
}

}

void bmp_mes(){

t_bmp=bmp.readTemperature();
pre_bmp=bmp.readPressure()/100.0F;
  
}
void ruch_mes(){

 int val = digitalRead(inputPin);
  if (val == HIGH) {
    ruch="wykryto ruch";
    }
  else {
    ruch="brak ruchu";
    }
    

}
/*void czas(){
  timeClient.update();
  formattedTime = timeClient.getFormattedTime();
  godzina = timeClient.getHours();
}*/


void audio_mes(){

 int val = digitalRead(inputaudio);
  if (val == HIGH) {
    audio="wykryto dzwiek";
    }
  else {
    audio="brak dzwieku";
    }
    
}


void autom(){
  lig_mes();
  ruch_mes();
  if(lux<=100&&ruch=="wykryto ruch"){
    digitalWrite(output26, LOW);
}
else{
  digitalWrite(output26, HIGH);
}


}
/*void autom2(){
  czaszapalania=20;
  czas();
  if(godzina==czaszapalania)
  {
    digitalWrite(output27, LOW);
}
else{
  digitalWrite(output27, HIGH);
}*/


//}

void lig_mes(){
    analogReadResolution(10);
    
    float volts =  analogRead(TEMT6000) * 5 / 1024.0;
    float VoltPercent = analogRead(TEMT6000) / 1024.0 * 100;
    
    float amps = volts / 10000.0;
    float microamps = amps * 1000000; 
    lux = microamps * 2.0 + 80; 
    
}


String processor(const String& var){
  DHT_mes();
  bmp_mes();
  lig_mes();
  ruch_mes();
  audio_mes();
  //czas();
 
  
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  else if(var == "TEMPBMP"){
    return String(t_bmp);
  }
  else if(var == "PREBMP"){
    return String(pre_bmp);
  }
  else if(var == "LIGHT"){
    return String(lux);
  }
   else if(var == "RUCH"){
    return String(ruch);
  }
  else if(var == "AUDIO"){
    return String(audio);
  }
 

}


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Projekt Domu Inteligentnego Rabczak Piotr</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #4B1D3F; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .card.temperature { color: #0e7c7b; }
    .card.humidity { color: #17bebb; }
    .card.pressure { color: #3fca6b; }
    .card.gas { color: #d62246; }
    .button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
    .button2 {background-color: #555555;}
  </style>
</head>
<body>
  <div class="topnav">
    <h3>Dom Inteligentny</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card temperature">
        <h4> STAN WYJSCIA 1</h4><p><span class="reading">
        <span id="output26"><button id="on">ON</button><button id="off">OFF</button><br><button id="auto">AUTO</button><br></span></span></p>
        
      </div>
      <div class="card temperature">
        <h4>STAN WYJSCIA 2</h4><p><span class="reading">
        <span id="output27"><button id="on2">ON</button><button id="off2">OFF</button><br><button id="auto2">AUTO2</button></span></span></p>
       
      </div>
      <div class="card temperature">
        <h4> TEMPERATURE</h4><p>
        <span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card humidity">
        <h4>HUMIDITY</h4><p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt; </span></p>
      </div>
      <div class="card pressure">
        <h4> PREBMP</h4><p><span class="reading"><span id="pres">%PREBMP%</span> hPa</span></p>
      </div>
      <div class="card temperature">
        <h4>TEMPBMP</h4><p><span class="reading"><span id="tbmp">%TEMPBMP%</span> &deg;C</span></p>
      </div>
       <div class="card temperature">
        <h4> LIGHT</h4><p><span class="reading"><span id="lux">%LIGHT%</span> LUX</span></p>
      </div>
      <div class="card temperature">
        <h4>RUCH</h4><p><span class="reading"><span id="ruch">%RUCH%</span></span></p>
      </div>
      <div class="card temperature">
        <h4> AUDIO</h4><p><span class="reading"><span id="audio">%AUDIO%</span></span></p>
      </div>
      
      
    </div>
  </div>
<script>



if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('humidity', function(e) {
  console.log("humidity", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
  source.addEventListener('t_bmp', function(e) {
  console.log("t_bmp", e.data);
  document.getElementById("t_bmp").innerHTML = e.data;
 }, false);
   source.addEventListener('pre_bmp', function(e) {
  console.log("pre_bmp", e.data);
  document.getElementById("pre_bmp").innerHTML = e.data;
 }, false);
  source.addEventListener('luxx', function(e) {
  console.log("luxx", e.data);
  document.getElementById("lux").innerHTML = e.data;
 }, false);
 source.addEventListener('ruch', function(e) {
  console.log("ruch", e.data);
  document.getElementById("ruch").innerHTML = e.data;
 }, false);
 source.addEventListener('audio', function(e) {
  console.log("audio", e.data);
  document.getElementById("audio").innerHTML = e.data;
 }, false);
  source.addEventListener('output26', function(e) {
  console.log("output26state", e.data);
  document.getElementById("output26").innerHTML = e.data;
 }, false);
 source.addEventListener('output27', function(e) {
  console.log("output27state", e.data);
  document.getElementById("output27").innerHTML = e.data;
 }, false);
  source.addEventListener('czas', function(e) {
  console.log("czas", e.data);
  document.getElementById("czas").innerHTML = e.data;
 }, false);
}

  document.getElementById("on").onclick = function () {  //po nacisinięciu elementu o ID "on"
        const zapytanie = new XMLHttpRequest();            //wyślijmy zapytanie GET, pod adresem /on      
        zapytanie.open("GET", "/on");
        zapytanie.send();                               
    };

    document.getElementById("off").onclick = function () {  //po nacisinięciu elementu o ID "off"
        const zapytanie = new XMLHttpRequest();             //wyślijmy zapytanie GET, pod adresem /off   
        zapytanie.open("GET", "/off");
        zapytanie.send();                               
    };
     document.getElementById("auto").onclick = function () {  //po nacisinięciu elementu o ID "off"
        const zapytanie = new XMLHttpRequest();             //wyślijmy zapytanie GET, pod adresem /off   
        zapytanie.open("GET", "/auto");
        zapytanie.send();                               
    };
    document.getElementById("on2").onclick = function () {  //po nacisinięciu elementu o ID "on"
        const zapytanie = new XMLHttpRequest();            //wyślijmy zapytanie GET, pod adresem /on      
        zapytanie.open("GET", "/on2");
        zapytanie.send();                               
    };
    document.getElementById("auto2").onclick = function () {  //po nacisinięciu elementu o ID "off"
        const zapytanie = new XMLHttpRequest();             //wyślijmy zapytanie GET, pod adresem /off   
        zapytanie.open("GET", "/auto2");
        zapytanie.send();                               
    };

    document.getElementById("off2").onclick = function () {  //po nacisinięciu elementu o ID "off"
        const zapytanie = new XMLHttpRequest();             //wyślijmy zapytanie GET, pod adresem /off   
        zapytanie.open("GET", "/off2");
        zapytanie.send();                               
    };

</script>
</body>
</html>)rawliteral";


void setup() {
  Serial.begin(115200);
   WiFi.mode(WIFI_AP_STA);
   WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(10000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
  
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
    unsigned status;

  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,   
                 Adafruit_BMP280::SAMPLING_X2,    
                 Adafruit_BMP280::SAMPLING_X16,   
                 Adafruit_BMP280::FILTER_X16,     
                 Adafruit_BMP280::STANDBY_MS_500);

  dht.begin();
  pinMode(TEMT6000, INPUT);
  pinMode(inputPin, INPUT);
  pinMode(inputaudio, INPUT);
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  digitalWrite(output26, HIGH);
  digitalWrite(output27, HIGH);

   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ //na otrzymane od klienta zapytania pod adresem "/" typu GET, 
    request->send(SPIFFS, "/index.html", "text/html");         //odpowiedz plikiem index.html z SPIFFS (można to zmienić na kartę SD) 
                                                               //zawierającym naszą stronę będącą plikem tekstowym HTML
  });

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){ 
    digitalWrite(output26, LOW);
    stanauto=0;                                      
                                         
    request->send(200);                                          
  });
  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request){ //na otrzymane od klienta zapytanie pod adresem "/on" typu GET, 
    stanauto=1;                                    //zapal diodę
                                         //zapal diodę
    request->send(200);                                          //odeślij odpowiedź z kodem 200 OK 
  });

   server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){ //na otrzymane od klienta zapytanie pod adresem "/off" typu GET, 
   digitalWrite(output26, HIGH);
   stanauto=0;
                                           //zgaś diodę
   request->send(200);                                             //odeślij odpowiedź z kodem 200 OK          
 }); 
  server.on("/on2", HTTP_GET, [](AsyncWebServerRequest *request){ //na otrzymane od klienta zapytanie pod adresem "/on" typu GET, 
    digitalWrite(output27, LOW); 
    stanauto=0;                                     //zapal diodę
    request->send(200);                                          //odeślij odpowiedź z kodem 200 OK 
  });
   server.on("/auto2", HTTP_GET, [](AsyncWebServerRequest *request){ //na otrzymane od klienta zapytanie pod adresem "/on" typu GET, 
    stanauto2=1;                                    //zapal diodę
                                         //zapal diodę
    request->send(200);                                          //odeślij odpowiedź z kodem 200 OK 
  });

   server.on("/off2", HTTP_GET, [](AsyncWebServerRequest *request){ //na otrzymane od klienta zapytanie pod adresem "/off" typu GET, 
   digitalWrite(output27, HIGH); 
   stanauto2=0;                                       //zgaś diodę
   request->send(200);                                             //odeślij odpowiedź z kodem 200 OK          
  });

  //timeClient.begin();
}



void loop(){
  if ((millis() - lastTime) > timerDelay) {
    lig_mes();
  DHT_mes();
  ruch_mes();
  audio_mes();
  //czas();
  if(stanauto==1){
    autom();
  }
/*if(stanauto2==1){
  autom2();
}*/




 Serial.print(formattedTime);

  

    events.send("ping",NULL,millis());
    events.send(String(t).c_str(),"temperature",millis());
    events.send(String(h).c_str(),"humidity",millis());
    events.send(String(t_bmp).c_str(),"t_bmp",millis());
    events.send(String(pre_bmp).c_str(),"pre_bmp",millis());
    events.send(String(lux).c_str(),"luxx",millis());
    events.send(String(ruch).c_str(),"ruch",millis());
    events.send(String(audio).c_str(),"audio",millis());
    events.send(String(formattedTime).c_str(),"formattedTime",millis());
    
    lastTime = millis();
}}
