/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-mesh-esp32-esp8266-painlessmesh/
  
  This is a simple example that uses the painlessMesh library: https://github.com/gmag11/painlessMesh/blob/master/examples/basic/basic.ino
*/
#define IR_SEND_PIN 9

#include "painlessMesh.h"
#include <M5StickCPlus.h>
// #include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>  // include the library


#define   MESH_PREFIX     "homeMesh"
#define   MESH_PASSWORD   "homeMesh"
#define   MESH_PORT       5555

#define   WIFI_CHANNEL 11

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
const char* deviceName = "fan";


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
  // M5.Lcd.fillScreen(BLACK);
  // M5.Lcd.setCursor(0, 0);
  // M5.Lcd.printf("Mesh Node \n", 0);
  // M5.Lcd.printf("Received from %u \nCommand: Turn on/off sent", from, msg.c_str());
  // IrSender.sendFAST(0x4040, 0xA, 10);
  StaticJsonDocument<200> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, msg);
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return;
  }

    // Serialize JSON object to a string
  String jsonString;
  serializeJson(jsonDocument, jsonString);

  // Print the JSON string
  Serial.println("Received JSON document:");
  Serial.println(jsonString);
  
  // Access JSON data
  int command = jsonDocument["command"];
  const char* target_appliance_name = jsonDocument["device"];

  controlAppliance(target_appliance_name, command);
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

  IrSender.begin(); // Initialize IR sender.

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
  // curCommand = 3;

  // Serial.println("Sending IR Signal");
  // IrSender.sendFAST(curCommand,2);
  // delay(5000);
}

void controlAppliance(const char* appliance_name, int command) {
  if (strcmp(appliance_name, deviceName) == 0) {
    switch(command) {
      case 0:
        IrSender.sendFAST(2, 2);
        break;
      case 1:
        IrSender.sendFAST(3, 2);
        break;
      case 2:
        IrSender.sendFAST(4, 2);
        break;
      case 3:
        IrSender.sendFAST(5, 2);
        break;
      case 4:
        IrSender.sendFAST(6, 2);
        break;
      case 5:
        IrSender.sendFAST(7, 2);
        break;
      default:
        // Handle invalid command
        break;
    }
  }
}
