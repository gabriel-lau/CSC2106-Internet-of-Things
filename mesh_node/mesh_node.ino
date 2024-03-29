/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-mesh-esp32-esp8266-painlessmesh/
  
  This is a simple example that uses the painlessMesh library: https://github.com/gmag11/painlessMesh/blob/master/examples/basic/basic.ino
*/
#define IR_SEND_PIN 9

#include "painlessMesh.h"
#include <M5StickCPlus.h>
#include <IRremote.hpp>  // include the library

#define   MESH_PREFIX     "homeMesh"
#define   MESH_PASSWORD   "homeMesh"
#define   MESH_PORT       5555

#define   WIFI_CHANNEL 4

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
//void sendMessage() ; // Prototype so PlatformIO doesn't complain

//Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
/*
void sendMessage() {
  String msg = "Hi from node";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
  Serial.println(msg);
}*/

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Mesh Node \n", 0);
  M5.Lcd.printf("Received from %u \nCommand: Turn on/off sent", from, msg.c_str());
  IrSender.sendNEC(0x4040, 0xA, 10);
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection,\nnodeId = %u\n", nodeId);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("Mesh Node \n", 0);
    M5.Lcd.printf("Connected to master Node,\n nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);

  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Mesh Node", 0);

  IrSender.begin();

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT , WIFI_AP_STA, WIFI_CHANNEL);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  //userScheduler.addTask( taskSendMessage );
  //taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
