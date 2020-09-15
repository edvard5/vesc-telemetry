//-- Libraries Included --------------------------------------------------------------
  #include <ESP8266WiFi.h>                       // The Basic Function Of The ESP NOD MCU
  #include <ESP8266WiFiMulti.h>                  // ---...---...---
  #include "VescUart.h"                          // VESC UART Functions
  #include "U8g2lib.h"                           // OLED Library
  #include "wire.h"                              // I2C Library for OLED

//------------------------------------------------------------------------------------
  // Defines
 U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);          // Initialize oled display
 WiFiClient    TCP_Client;                       // Renamed function WiFiClient to TCP_Client

//------------------------------------------------------------------------------------
  // Authentication Variables
  const char*         ssid="ESK8";               // Wifi Name
  const char*         password="helloworld";     // Wifi Password
  const String        Devicename="RemCntrl";     // Device name

//------------------------------------------------------------------------------------
  // WIFI Client Module Role & Port
  IPAddress     TCP_Server(192, 168, 4, 1);       // TCP Server to connect to
  IPAddress     TCP_Gateway(192, 168, 4, 1);      // TCP Server Gateway
  IPAddress     TCP_Subnet(255, 255, 255, 0);     // TCP Server Subnet mask
  unsigned int  TCPPort=23;                       // TCP Connection port

//------------------------------------------------------------------------------------
  // Some Variables
  float RF=1.0;                                   // Resistors divider factor 
  float nRV=0;                                     // Remote voltage
  unsigned long wifiICON[]={0xE218,0xE219,0xE21A};

//------------------------------------------------------------------------------------
  //Functions to operate in TCP
//------------------------------------------------------------------------------------
void handshake(){                                       // Establishing connection (can add TCP_Client.print() function for debugging)
  TCP_Client.stop();                                    // First make sure you got disconnected
  if(TCP_Client.connect(TCP_Server, TCPPort)){
    delay(1000);
  }
  TCP_Client.setNoDelay(1);                             // Allow fast communication
}

void Check_WiFi_and_Connect_or_Reconnect(){
  if (WiFi.status() != WL_CONNECTED){                   // Check if connected to WIFI network
    TCP_Client.stop();                                  // Make Sure Everything Is Reset
    WiFi.disconnect();
    delay(50);
    WiFi.mode(WIFI_STA);                                // Station (Client) Only - to avoid broadcasting an SSID ??
    WiFi.begin(ssid,password);                          // Connection to specified WIFI AP
// Connecting till success -----------------------------------------------------------
    while(WiFi.status() != WL_CONNECTED){
      u8g2.setFont(u8g2_font_siji_t_6x10);
      for(int i=0; i <=2; i++){
        u8g2.clearBuffer();
        u8g2.drawStr(2,60,"Connecting");
        u8g2.drawGlyph(2,10,wifiICON[i]);
        u8g2.sendBuffer();
        digitalWrite(LED_BUILTIN, !HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, !LOW);
        delay(250);
// -----------------------------------------------------------------------------------
      }
    }

  // Stop blinking to indicate if connected -------------------------------------------
    digitalWrite(LED_BUILTIN, !HIGH);
    u8g2.clearBuffer();
    u8g2.drawGlyph(2,10,wifiICON[2]);
    u8g2.drawStr(5,60,"Connected");
    u8g2.sendBuffer();
    delay(1500);

  // Conecting as a client -------------------------------------------------------------
    handshake();

  }
}
// Get values through TCP also displays value on screen (specified params in com function)~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void com(int x, int y, String command){
  unsigned long tNow;
  tNow=millis();
  TCP_Client.print(command);
  TCP_Client.print('\r');
    while(1){                   
	  int len = TCP_Client.available();
      if (len > 0) {
        if (len > 80){ 
          len = 80;
        } 
      String line = TCP_Client.readStringUntil('\r'); // if '\r' is found  
      u8g2.drawStr(x,y,line.c_str());
      //  Just for Voltage battery icon change------ ~~~WIP~~~
      if(command=="voltage"){
        if(line.toFloat()<36){
          u8g2.drawGlyph(51,20,0xE24B);                     // draw LOW battery icon for Longboard 
        }
        else if(line.toFloat()<39 && 36<line.toFloat()){
          u8g2.drawGlyph(51,10,0xE247);                     // draw MID battery icon for Longboard
        }
        else if(line.toFloat()<42 && 39<line.toFloat()){
          u8g2.drawGlyph(51,10,0xE243);                     // draw FULL battery icon for Longboard
        }     
      }
      //---------------------------------------------          
      Serial.println(line);                           
      break;                                          // exit
      }                                              
      if((millis()-tNow)>1000){                       // if more then 1 Second No Reply -> exit
        Serial.println("SEND_timeout");                     
      break;                                          // exit
      }
  TCP_Client.flush();                                 // Empty Bufffer                                     
  Check_WiFi_and_Connect_or_Reconnect();
  }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
// Draw things ---------------------------------------------------------------------------
void  drawObjects_BOT(){
  u8g2.setFont(u8g2_font_siji_t_6x10);
  u8g2.drawGlyph(2,10,wifiICON[2]);
  if(nRV<3.6){
    u8g2.drawGlyph(50,10,0xE236);                     // draw LOW battery icon for remote controller
  }
  else if(nRV<3.9 && 3.6<nRV){
    u8g2.drawGlyph(50,10,0xE237);                     // draw MID battery icon for remote controller
  }
  else if(nRV<4.2 && 3.9<nRV){
    u8g2.drawGlyph(50,10,0xE238);                     // draw FULL battery icon for remote controller
  }  
  //u8g2.drawGlyph(51,20,0xE24B);                       // draw battery icon for longboard
  u8g2.drawGlyph(0,95,0xE040);                        // draw lighting bolt for amphours
  u8g2.drawGlyph(0,105,0xE06A);                       // draw arrow to TRIP:
  u8g2.drawGlyph(0,115,0xE01D);                       // draw TEMP icon to ESC temp
  u8g2.drawStr(3,32,"Speed Km/h");
  u8g2.drawStr(16,95,"mAh:");
  u8g2.drawStr(10,105,"Trip:");
  u8g2.drawStr(10,115,"Temp:");
}
void drawObjects_TOP(){
  u8g2.setFont(u8g2_font_trixel_square_tf);
  u8g2.drawStr(15,10,"RBAT:");
  u8g2.drawStr(15,20,"LBAT:");
  u8g2.drawLine(0,22,64,22);
  u8g2.drawLine(0,82,64,82);
}
//----------------------------------------------------------------------------------------

void setup(){
  // setting the serial port ----------------------------------------------
  Serial.begin(115200);  
  u8g2.begin();
  u8g2.setFont(u8g2_font_siji_t_6x10);
  u8g2.setDisplayRotation(U8G2_R1);
  pinMode(A0,INPUT);

  // setting the mode of pins ---------------------------------------------
  pinMode(LED_BUILTIN, OUTPUT);                          // WIFI OnBoard LED Light
  
  // WiFi Connect ----------------------------------------------------
  Check_WiFi_and_Connect_or_Reconnect();                  // Checking For Connection
  
  
}
void loop(){
  char RV[4];
  u8g2.clearBuffer();
  drawObjects_BOT();
  com(40,95,"amps");
  com(40,105,"tacho");
  com(40,115,"voltage"); // just for reference, should be temp
  drawObjects_TOP();
  nRV=(analogRead(A0)/1024.0)*5;
  dtostrf(nRV,3,2,RV);
  u8g2.drawStr(34,10,RV);
  com(34,20,"voltage");
  u8g2.setFont(u8g2_font_7Segments_26x42_mn);
  com(2,80,"speed");
  u8g2.sendBuffer();
}










