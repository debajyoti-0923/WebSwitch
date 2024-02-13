#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <LittleFS.h>
#include <AsyncElegantOTA.h>
//---------------------------------------------------------------------
//define varibales
#define R1 5
#define R2 4
#define R3 14
#define R4 12
#define R5 13
#define FOR for(int i=0;i<5;i++) 
//--------------------------------------------------------------------------
const char* ssid = "TP-Link_C6EB";
const char* password = "Tpsm@181";

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char* ntpServerName = "pool.ntp.org";
const long utcOffsetInSeconds = 19800;
WiFiUDP udp;
NTPClient timeClient(udp, ntpServerName, utcOffsetInSeconds);

PROGMEM char* root=R"(
  <!DOCTYPE html>
  <html lang="en">
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <!-- <link rel="stylesheet" type="text/css" href="style.css"> -->
      <title>Table</title>
  </head>
  <style>
      :root {
          --off: rgb(44, 43, 43);
          --off-shadow: #cadfef;
          --background: #35424e;
          --title:#ADFCF9;
          --btn-color:#979797;
          --btn-active:#5c5c5c;
          --btn-border:#fefedf;
      }


      html,body{
          margin: 0;
          padding: 0;
          border: 0;
      }
      .main-body{
          display: flex;
          background-color: var(--background);
          flex-direction: column;
          height: 100vh;
      }

      .title{
          display: flex;
          background-color: var(--title);
          height: 10vh;
          justify-content: center;
          align-items: center;
      }

      .grid-body{
          display: grid;
          height: 80vh;
          grid-template-columns: repeat(2,1fr);
          grid-template-rows: auto;
          gap: 10px;    
      }

      .btn-div{
          position:relative;
          display: flex;
          justify-content: center;
          align-items: center;
      }

      .btn{
          display: flex;
          justify-content: center;
          align-items: center;
          height: 22vh;
          width: 22vh;
          font-size: 50px;
          color:var(--off);
          background: linear-gradient(145deg, #cacaca, #f0f0f0);
          border-radius: 15%;
          border:5px double var(--off);
          transition: transform 0.2s;
      }

      .btn:active{
          transform: translateY(3px);
          background: rgb(179, 179, 179);
      }

      .btn-active{
          display: flex;
          justify-content: center;
          align-items: center;
          height: 22vh;
          width: 22vh;
          font-size: 50px;
          color:var(--btn-border);
          
          background: var(--btn-active);
          border-radius: 15%;
          border:5px double var(--btn-border);
          box-shadow: 0px 0px 30px var(--btn-border),
          0px 0px 30px #121212 inset;
          transition: transform 0.2s;
      }

      .btn-active:active{
          transform: translateY(3px);
          background-color: rgb(82, 82, 82);
      }

      .timer{
          position: absolute;
          font-size: 35px;
          background: transparent;
          border: none;
          color:#6f6f6f;
          z-index: 100;
          top:12%;
          right:5%;
      }

      .timer-active{
          position: absolute;
          font-size: 35px;
          background: transparent;
          border: none;
          color:var(--btn-border);
          z-index: 100;
          top:12%;
          right:5%;
      }


      @media(min-width:700px){
          .btn,.btn-active{
              width: 90%;
              height: 90%;
              border-radius: 20px;
              font-size: 80px;
          }
      }
  </style>
  <body>
      <div class="main-body">
          <div class="title">
              <h1>AMY</h1>
          </div>
          <div class="grid-body">
              <div class="btn-div">
                  <button class="btn" id="btn1"><ion-icon name="laptop"></ion-icon></button>
                  <button class="timer" id="timer1"><ion-icon name="timer"></ion-icon></button>
              </div>
              <div class="btn-div">
                  <button class="btn" id="btn2"><ion-icon name="desktop"></ion-icon></button>
                  <button class="timer" id="timer2"><ion-icon name="timer"></ion-icon></button>
              </div>
              <div class="btn-div">
                  <button class="btn" id="btn3"><ion-icon name="phone-portrait-outline"></ion-icon></button>
                  <button class="timer" id="timer3"><ion-icon name="timer"></ion-icon></button>
              </div>
              <div class="btn-div">
                  <button class="btn-active" id="btn4"><ion-icon name="watch"></ion-icon></button>
                  <button class="timer" id="timer4"><ion-icon name="timer"></ion-icon></button>
              </div>
              <div class="btn-div">
                  <button class="btn" id="btn5" style=""><ion-icon name="headset"></ion-icon></button>
                  <button class="timer" id="timer5"><ion-icon name="timer"></ion-icon></button>
              </div>
              <div class="btn-div">
                  <button class="btn" id="btn6"><ion-icon name="cog"></ion-icon></button>
              </div>
          </div>

      </div>
  </body>
  <script type="module" src="https://unpkg.com/ionicons@5.5.2/dist/ionicons/ionicons.esm.js"></script>
  <!-- <script src="./logic.js"></script> -->
  <script>
      let status=[0,0,0,0,0];

      function update_status(){
          for(var i=0;i<5;i++){
              var btn=document.getElementById('btn'+(i+1));
              var timer=document.getElementById('timer'+(i+1));

              if(status[i]===0){
                  btn.classList.add('btn');
                  btn.classList.remove('btn-active');
              }else{
                  btn.classList.add('btn-active');
                  btn.classList.remove('btn');
              }

              if(status[i]==2){
                  timer.classList.add('timer-active');
                  timer.classList.remove('timer');
              }else{
                  timer.classList.add('timer');
                  timer.classList.remove('timer-active');
              }

          }
      }

      function toggle(id){
          if(status[id]!=0){
            status[id]=0;
          }else{
            status[id]=1;
          }
          // update_status();
          data={'P1':status[0],'P2':status[1],'P3':status[2],'P4':status[3],'P5':status[4]};
          ws.send(JSON.stringify(data));
      }

      function toggle2(id){
          if(status[id]!=2){
            status[id]=2;
            data={'P1':status[0],'P2':status[1],'P3':status[2],'P4':status[3],'P5':status[4]};
            ws.send(JSON.stringify(data));
          }else{
            fetch(window.location.href+'getrem?pin='+(id+1))
            .then(response => response.json())
            .then(data => {
              alert("Remaining Time: "+data.response+" minutes!\nDefault Time: "+data.def+" minutes");
            })
          }
      }

      function updateFromJSON(data){
        const object=JSON.parse(data);
        for(var i=0;i<5;i++){
          status[i]=object['status'][i];
        }
        update_status();
      }

      let btn1=document.getElementById('btn1');
      let btn2=document.getElementById('btn2');
      let btn3=document.getElementById('btn3');
      let btn4=document.getElementById('btn4');
      let btn5=document.getElementById('btn5');
      let btn6=document.getElementById('btn6');

      let timer1=document.getElementById('timer1');
      let timer2=document.getElementById('timer2');
      let timer3=document.getElementById('timer3');
      let timer4=document.getElementById('timer4');
      let timer5=document.getElementById('timer5');

      const ws=new WebSocket(window.location.href.replace('http://','ws://').slice(0,-1)+':81');

      let connectionTimeout;
      function resetConnectionTimeout() {
        clearTimeout(connectionTimeout);
        connectionTimeout = setTimeout(() => {
          ws.close(1000);
          console.log('WebSocket connection closed due to inactivity');
        }, 50000);
      }

      ws.onopen=function(){console.log('connected');resetConnectionTimeout();};
      ws.onmessage=function(event){
        console.log('recived '+event.data);
        updateFromJSON(event.data);
        resetConnectionTimeout();
      };
      ws.onclose=function(){
        console.log('WebSocket connection closed');
        clearTimeout(connectionTimeout);
      }
    

      btn1.addEventListener('click',()=>{toggle(0);});
      btn2.addEventListener('click',()=>{toggle(1);});
      btn3.addEventListener('click',()=>{toggle(2);});
      btn4.addEventListener('click',()=>{toggle(3);});
      btn5.addEventListener('click',()=>{toggle(4);});

      timer1.addEventListener('click',()=>{toggle2(0);});
      timer2.addEventListener('click',()=>{toggle2(1);});
      timer3.addEventListener('click',()=>{toggle2(2);});
      timer4.addEventListener('click',()=>{toggle2(3);});
      timer5.addEventListener('click',()=>{toggle2(4);});

      update_status()
  </script>
  </html>
)";
//-------------------------------------------------------------------------------------------------------------
//STRUCT FUNCTIONS
typedef struct statusType{
  uint8_t pin_;
  uint8_t value_;
  uint8_t lastValue_;
  int16_t sleep_;
  int16_t remSleep_;
};

typedef struct dailyData{
  int time_;
  int values_[5];
};

dailyData timing[10];
int dailyIndex=0;

boolean push(int t,int v1,int v2,int v3,int v4,int v5){
  if(dailyIndex>9){
    return false;
  }
  dailyData d;
  d.time_=t;
  d.values_[0]=v1;
  d.values_[1]=v2;
  d.values_[2]=v3;
  d.values_[3]=v4;
  d.values_[4]=v5;

  timing[dailyIndex]=d;
  dailyIndex++;
  return true;
}

boolean pop(int index_){
  if(dailyIndex<0 || index_>dailyIndex-1){
    return false;
  }
  for(int i=index_;i<dailyIndex;i++){
    timing[i]=timing[i+1];
  }
  dailyIndex--;
  return true;
}

void show(){
  for(int i=0;i<dailyIndex;i++){
    Serial.print(timing[i].time_);
    Serial.print(" ");
    for(int j=0;j<5;j++)
      Serial.print(timing[i].values_[j]);
    Serial.println();
  }
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//VARIABLES 

statusType status[5];
int time_now;
bool first=true;
ulong first_secs;
int wifi_stats=0;

//------------------------------------------------------------------------------------------------------------
//FLASH VARIABLES

void loadConfig(){
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("failed");
    return;
  }

  DynamicJsonDocument jsonDocument(1024);
  auto error=deserializeJson(jsonDocument,configFile);
  if(error)
    return;
  
  JsonArray sleepArr=jsonDocument["sleepValues"];
  JsonArray dailyTime=jsonDocument["dailyTime"];
  JsonArray dailyValues=jsonDocument["dailyValues"];
  status[0].pin_=R1;
  status[1].pin_=R2;
  status[2].pin_=R3;
  status[3].pin_=R4;
  status[4].pin_=R5;
  FOR{
    status[i].value_=0;
    status[i].lastValue_=0;
    status[i].sleep_=sleepArr[i];
    status[i].remSleep_=0;
  }

  for(int i=0;i<dailyTime.size();i++){
    JsonArray temp=dailyValues[i];
    push(dailyTime[i],temp[0],temp[1],temp[2],temp[3],temp[4]);
  }

  wifi_stats=jsonDocument["wf"];
  Serial.println("Config loaded!");
}
void saveConfig(){
  DynamicJsonDocument jsonDocument(1024);
  JsonArray sleepValues = jsonDocument.createNestedArray("sleepValues");
  JsonArray dailyTime =jsonDocument.createNestedArray("dailyTime");
  JsonArray dailyValues =jsonDocument.createNestedArray("dailyValues");
  FOR{
    sleepValues.add(status[i].sleep_);
  }

  for(int j=0;j<dailyIndex;j++){
    dailyTime.add(timing[j].time_);
    JsonArray timeArr=dailyValues.createNestedArray();
    FOR{
      timeArr.add(timing[j].values_[i]);
    }
  }
  
  jsonDocument["wf"]=wifi_stats;

  String jsonString;
  serializeJson(jsonDocument, jsonString);
  Serial.println(jsonString);

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("file save fail");
    return;
  }

  serializeJson(jsonDocument,configFile);
}
//------------------------------------------------------------------------------------------------------------
void toggle(int p,int v);
void update_status();
void update_pins(String data);
void sendToWeb();
void checkTimer();
void checkDaily();
void handleRoot(AsyncWebServerRequest *request);
void handlePing(AsyncWebServerRequest *request);
void handleRemaingTime(AsyncWebServerRequest *request);
void handleTime(AsyncWebServerRequest *request);
void handleSetSleep(AsyncWebServerRequest *request);
void handleSetDaily(AsyncWebServerRequest *request);
void handleRemoveDaily(AsyncWebServerRequest *request);
void handleGetDaily(AsyncWebServerRequest *request);
void handleSetPin(AsyncWebServerRequest *request);
void handleGetPin(AsyncWebServerRequest *request);
//-------------------------------------------------------------------------------------------------------------
void updateTime(){
  timeClient.update();
  time_now=((int)timeClient.getHours())*60+(int)timeClient.getMinutes();
  first_secs=(60-timeClient.getSeconds())*1000;

}

void toggle(int p,int v){
  if(p==R5){
    if(v==0)
      digitalWrite(p,0);
    else
      digitalWrite(p,1);
  }else{
    if(v==0)
      digitalWrite(p,1);
    else
      digitalWrite(p,0);
  }
}

void update_status(){
  FOR{
    if(status[i].value_!=status[i].lastValue_){
      if(status[i].value_==2){
        status[i].remSleep_+=status[i].sleep_;
      }else{
        status[i].remSleep_=0;
      }

      status[i].lastValue_=status[i].value_;
    }
    toggle(status[i].pin_,status[i].value_);
  }
  sendToWeb();
}

void update_pins(String data){
  DynamicJsonDocument jsonDocument(1024);
  DeserializationError error = deserializeJson(jsonDocument, data);
  if(!error){
    status[0].value_=(int)jsonDocument["P1"];
    status[1].value_=(int)jsonDocument["P2"];
    status[2].value_=(int)jsonDocument["P3"];
    status[3].value_=(int)jsonDocument["P4"];
    status[4].value_=(int)jsonDocument["P5"];
  }
  update_status();
}
void sendToWeb(){
  DynamicJsonDocument jsonDocument(1024);
  JsonArray jsonArray = jsonDocument.createNestedArray("status");

  FOR{
    jsonArray.add(status[i].value_);
  }

  String jsonString;
  serializeJson(jsonDocument, jsonString);
  webSocket.broadcastTXT(jsonString);
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      Serial.printf("[%u] connected!\n", num);
      {
        sendToWeb();
      }
      break;
    case WStype_TEXT:
       String payloadString = String((char*)payload);
      update_pins(payloadString);
      break;
  }
}

IPAddress staticIP(192, 168,   0, 159);
IPAddress  gateway(192, 168,   0,   1);
IPAddress   subnet(255, 255, 255,   0);
IPAddress dns1(192,168,0,1);
IPAddress dns2(8,8,8,8);
//-------------------------------------

void setup() {

  //pinmode
  pinMode(R1,OUTPUT);
  pinMode(R2,OUTPUT);
  pinMode(R3,OUTPUT);
  pinMode(R4,OUTPUT);
  pinMode(R5,OUTPUT);
  digitalWrite(R1,1);
  digitalWrite(R2,1);
  digitalWrite(R3,1);
  digitalWrite(R4,1);
  digitalWrite(R5,0);
  //FILESYSTEM
  if (!LittleFS.begin()) {
    return;
  }
  
  // Connect to WiFi
  Serial.begin(115200);
  uint8_t countConnet=0;

  loadConfig();
  
  if(wifi_stats){
    WiFi.config(staticIP, gateway, subnet,dns1,dns2);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
      countConnet++;
      if(countConnet==10)
        break;
    }

    if(WiFi.status()!=WL_CONNECTED){
      wifi_stats=0;
      saveConfig();
      ESP.restart();
    }else
      Serial.println("Connected to WiFi");
  }else{
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(staticIP, gateway, subnet);
    WiFi.softAP("Smart Board");
    Serial.println("Hotspot started -- Smart Board");
    wifi_stats=1;
    saveConfig();
  }
  Serial.println(WiFi.localIP());

  // Start the HTTP server
  server.on("/", HTTP_GET,[](AsyncWebServerRequest *request){handleRoot(request);});
  server.on("/esp", HTTP_GET,[](AsyncWebServerRequest *request){handlePing(request);});
  server.on("/getrem",HTTP_GET,[](AsyncWebServerRequest *request){handleRemaingTime(request);});
  server.on("/getsleep", HTTP_GET,[](AsyncWebServerRequest *request){handleTime(request);});
  server.on("/setsleep", HTTP_GET,[](AsyncWebServerRequest *request){handleSetSleep(request);});
  server.on("/setdaily", HTTP_GET,[](AsyncWebServerRequest *request){handleSetDaily(request);});
  server.on("/remdaily", HTTP_GET,[](AsyncWebServerRequest *request){handleRemoveDaily(request);});
  server.on("/getdaily", HTTP_GET,[](AsyncWebServerRequest *request){handleGetDaily(request);});
  server.on("/setpin", HTTP_GET,[](AsyncWebServerRequest *request){handleSetPin(request);});
  server.on("/getpin", HTTP_GET,[](AsyncWebServerRequest *request){handleGetPin(request);});
  

  AsyncElegantOTA.begin(&server);
  server.begin();
  

  // Start the WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


  update_status();

  timeClient.begin();
  updateTime();
}

ulong currTime,lasTime=0,lasTime_=0;
uint8_t loop_count=0;
void loop() {
  webSocket.loop();
  currTime=millis();
  if(currTime-lasTime_>=5000){
    if(WiFi.status()!=WL_CONNECTED){
      wifi_stats=0;
      ESP.restart();
    }
  }
  if(currTime-lasTime>=first_secs){
    if(loop_count>=60){
      updateTime();
      loop_count=0;
    }else{
      loop_count++;
    }

    if(first){
      first=false;
      first_secs=60000;
    }

    time_now=(time_now+1)%1440;
    Serial.println(dailyIndex);
    for(int i=0;i<dailyIndex;i++){
      int k=timing[i].time_;

      if(time_now==k){
        Serial.println("changing");
        for(int j=0;j<5;j++){
          if(timing[i].values_[j]!=3)
            status[j].value_=timing[i].values_[j];
        }
      update_status();
      }
    }

    FOR{
      if (status[i].value_==2){
        if(status[i].remSleep_<=0){
          status[i].value_=0;
          update_status();
        }
        status[i].remSleep_=status[i].remSleep_-1;
      }
    }
    lasTime=currTime;
  }
}




void handleRoot(AsyncWebServerRequest *request){
  request->send(200, "text/html", root);
}

void handlePing(AsyncWebServerRequest *request){
  request->send(200,"application/json","{\"response\":'OK'}");
}

void handleRemaingTime(AsyncWebServerRequest *request){
  uint8_t pinV=0;
  if(request->hasParam("pin")){
    String paramValue=request->getParam("pin")->value();
    pinV=paramValue.toInt()-1;
  }
  String responseValue="{\"response\":"+String(status[pinV].remSleep_)+",\"def\":"+String(status[pinV].sleep_)+"}";
  request->send(200,"application/json",responseValue);
}

void handleTime(AsyncWebServerRequest *request){
  String responseValue="{\"response\":["+String(status[0].sleep_)+","+String(status[1].sleep_)+","+String(status[2].sleep_)+","+String(status[3].sleep_)+","+String(status[4].sleep_)+"]}";
  request->send(200,"application/json",responseValue);
}

void handleSetSleep(AsyncWebServerRequest *request){
  if(request->hasParam("a")){
    String value=request->getParam("a")->value();
    status[0].sleep_=value.toInt();
  }if(request->hasParam("b")){
    String value=request->getParam("b")->value();
    status[1].sleep_=value.toInt();
  }if(request->hasParam("c")){
    String value=request->getParam("c")->value();
    status[2].sleep_=value.toInt();
  }if(request->hasParam("d")){
    String value=request->getParam("d")->value();
    status[3].sleep_=value.toInt();
  }if(request->hasParam("e")){
    String value=request->getParam("e")->value();
    status[4].sleep_=value.toInt();
  }

  handleTime(request);
  saveConfig();
}

void handleSetDaily(AsyncWebServerRequest *request){
  if(request->hasParam("a") && request->hasParam("b") && request->hasParam("c") && request->hasParam("d") && request->hasParam("e") && request->hasParam("hr") && request->hasParam("mn")){
    String a,b,c,d,e,hr,mn;
    a=request->getParam("a")->value();
    b=request->getParam("b")->value();
    c=request->getParam("c")->value();
    d=request->getParam("d")->value();
    e=request->getParam("e")->value();
    hr=request->getParam("hr")->value();
    mn=request->getParam("mn")->value();
    int tempTime=(hr.toInt()*60)+mn.toInt();
    if(push(tempTime,a.toInt(),b.toInt(),c.toInt(),d.toInt(),e.toInt())){
      String responseVal="{\"response\":["+a+","+b+","+c+","+d+","+e+"],'hr':"+hr+",'mn':"+mn+"}";
      request->send(200,"application/json",responseVal);
    }else{
      request->send(200,"application/json","{\"response\":'Full'}");
    }

    saveConfig();
  }
}

void handleRemoveDaily(AsyncWebServerRequest *request){
  if(request->hasParam("i")){
    String ind=request->getParam("i")->value();
    if(pop(ind.toInt()-1))
      request->send(200,"application/json","{\"response\":'OK'}");
    else
      request->send(200,"application/json","{\"response\":'Fail'}");
    
    saveConfig();
  }
}
void handleGetDaily(AsyncWebServerRequest *request){
  DynamicJsonDocument jsonDocument(1024);
  JsonArray DailyValues=jsonDocument.createNestedArray("DailyValue");
  Serial.println(dailyIndex);
  for(int j=0;j<dailyIndex;j++){
    JsonArray objArr=DailyValues.createNestedArray();
    objArr.add(timing[j].time_);
    JsonArray timeArr=objArr.createNestedArray();
    FOR{
      timeArr.add(timing[j].values_[i]);
    }
  }

  String jsonString;
  serializeJson(jsonDocument, jsonString);
  request->send(200,"application/json",jsonString);
}

void handleSetPin(AsyncWebServerRequest *request){
  if(request->hasParam("pin")&&request->hasParam("val")){
    String pin=request->getParam("pin")->value();
    String val=request->getParam("val")->value();
    status[pin.toInt()-1].value_=val.toInt();
    update_status();
    request->send(200,"application/json","{\"response\":\"OK\"}");
  }else
    request->send(200,"application/json","{\"response\":\"Fail\"}");
}

void handleGetPin(AsyncWebServerRequest *request){
  String response="{\"a\":"+String(status[0].value_)+",\"b\":"+String(status[1].value_)+",\"c\":"+String(status[2].value_)+",\"d\":"+String(status[3].value_)+",\"d\":"+String(status[4].value_)+"}";
  request->send(200,"application/json",response);
}
