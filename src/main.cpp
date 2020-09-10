//-- Libraries Included --------------------------------------------------------------
  #include <ESP8266WiFi.h>    // The Basic Function Of The ESP NOD MCU
  #include <ESP8266WiFiMulti.h>
  #include "VescUart.h"       // VESC UART Functions
  #include "U8g2lib.h"        // OLED Library
  #include "wire.h"           // I2C Library for OLED

//------------------------------------------------------------------------------------
  // Define I/O Pins
  //#define       LED_BUILTIN      2       // WIFI Module LED
 U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);
//------------------------------------------------------------------------------------
  // Authentication Variables
  const char*         ssid="AUTO";            // Wifi Name
  const char*         password="helloworld";     // Wifi Password
  const String Devicename="RemCntrl";
//------------------------------------------------------------------------------------
  // WIFI Client Module Role & Port
  IPAddress     TCP_Server(192, 168, 4, 1);       //TCP Server to connect to
  IPAddress     TCP_Gateway(192, 168, 4, 1);      //TCP Server Gateway
  IPAddress     TCP_Subnet(255, 255, 255, 0);     //TCP Server Subnet mask
  unsigned int  TCPPort=23;                     //TCP Connection port
  WiFiClient    TCP_Client;                       //Rename function WiFiClient to TCP_Client
//------------------------------------------------------------------------------------
  // Some Variables
  unsigned char buffer[80];
  unsigned long time_1=0;
  unsigned long time_2=0;
  String NUMB_1;
  String NUMB_2;

//====================================================================================
  //Functions to operate in TCP

void Tell_Server_we_are_there(){
  // first make sure you got disconnected
  TCP_Client.stop();

  // if sucessfully connected send connection message
  if(TCP_Client.connect(TCP_Server, TCPPort)){
    u8g2.clearBuffer();
    u8g2.drawStr(12,32,"~~Connected!~~");
    u8g2.sendBuffer();
    Serial.println    ("<" + Devicename + "-CONNECTED>");
    TCP_Client.println ("<" + Devicename + "-CONNECTED>");
    delay(1000);
  }
  u8g2.clearBuffer();
  TCP_Client.setNoDelay(1);                                     // allow fast communication?
}

void Check_WiFi_and_Connect_or_Reconnect(){
  if (WiFi.status() != WL_CONNECTED){
    
    TCP_Client.stop();                                  //Make Sure Everything Is Reset
    WiFi.disconnect();
    Serial.println("Not Connected...trying to connect...");
    delay(50);
    WiFi.mode(WIFI_STA);                                // station (Client) Only - to avoid broadcasting an SSID ??
    WiFi.begin("AUTO","helloworld");                         // the SSID that we want to connect to
    while(WiFi.status() != WL_CONNECTED){
      for(int i=0; i < 10; i++){
        u8g2.clearBuffer();
        u8g2.drawStr(12,32,"~~Connecting...~~");
        u8g2.drawStr(0+i,25,"- - -");
        u8g2.sendBuffer();
        digitalWrite(LED_BUILTIN, !HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, !LOW);
        delay(250);
        Serial.print(".");
      }
      Serial.println("");
    }
  // stop blinking to indicate if connected -------------------------------
    digitalWrite(LED_BUILTIN, !HIGH);
    Serial.println("!-- Client Device Connected --!");

  // Printing IP Address --------------------------------------------------
    Serial.println("Connected To      : " + String(WiFi.SSID()));
    Serial.println("Signal Strenght   : " + String(WiFi.RSSI()) + " dBm");
    Serial.print  ("Server IP Address : ");
    Serial.println(TCP_Server);
    Serial.print  ("Device IP Address : ");
    Serial.println(WiFi.localIP());
 
  // conecting as a client -------------------------------------
    Tell_Server_we_are_there();
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void ramen(int x, int y, String command){
  String line;
  unsigned long tNow;
  tNow=millis();
  while(1){                     // Check For Reply   
    // Serial.print("sent: ");                         // print the content
    // Serial.println(command);
    TCP_Client.print(command);
    TCP_Client.println('\r');
	 int len = TCP_Client.available();
     if (len > 0) {
       if (len > 80){ 
         len = 80;
       } 
      line = TCP_Client.readStringUntil('\n');         // if '\r' is found       
      Serial.println(line);                           
      break;                                              // exit
     }
     if((millis()-tNow)>1000){                             // if more then 1 Second No Reply -> exit
       Serial.println("SEND_timeout");                     // exit
     }
  u8g2.drawStr(x,y,line.c_str());
  delay(300); 
  TCP_Client.flush();                                     // Empty Bufffer 
  Check_WiFi_and_Connect_or_Reconnect();
  break;
  }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}

//=====================================================================================

void setup(){
  // setting the serial port ----------------------------------------------
  Serial.begin(115200);  
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  // setting the mode of pins ---------------------------------------------
  pinMode(LED_BUILTIN, OUTPUT);                          // WIFI OnBoard LED Light
  //digitalWrite(LED_BUILTIN, !LOW);                       // Turn WiFi LED Off
  
  // WiFi Connect ----------------------------------------------------
  Check_WiFi_and_Connect_or_Reconnect();          // Checking For Connection
  delay(3500);
  
}
void loop(){
  if(millis()>=time_1+500){
  u8g2.clearBuffer();
  time_1+=500;
  ramen(0,20,"NUMB_1");
  }

  if(millis()>=time_2+500){
  time_2+=500;
  ramen(10,25,"NUMB_2");
  u8g2.sendBuffer();
  }
  
}
