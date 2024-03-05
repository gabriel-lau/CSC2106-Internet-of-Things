//************************************************************
// this is a simple example that uses the painlessMesh library to 
// connect to a node on another network. Please see the WIKI on gitlab
// for more details
// https://gitlab.com/painlessMesh/painlessMesh/wikis/bridge-between-mesh-and-another-network
//************************************************************
#include <Arduino.h>
#include "painlessMesh.h"
#include <PubSubClient.h>
#include <WiFiClient.h>

#define   MESH_PREFIX     "homeMesh"
#define   MESH_PASSWORD   "homeMesh"
#define   MESH_PORT       5555

#define   WIFI_SSID     ""
#define   WIFI_PASSWORD ""
// Channel set to 6. Make sure to use the same channel for your mesh and for you other network (STATION_SSID)
#define   WIFI_CHANNEL 6

// prototypes
void receivedCallback( uint32_t from, String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);

IPAddress getlocalIP();

IPAddress myIP(0,0,0,0);

painlessMesh  mesh;
WiFiClient wifiClient;
PubSubClient mqttClient("test.mosquitto.org", 1883, mqttCallback, wifiClient);

void setup() {
  Serial.begin(115200);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages


  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, WIFI_CHANNEL);
  // Setup over the air update support
  mesh.initOTAReceive("bridge");

  mesh.stationManual(WIFI_SSID, WIFI_PASSWORD);
  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);


  mesh.onReceive(&receivedCallback);
}

void loop() {
  mesh.update();
  
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());

    if (mqttClient.connect("painlessMeshClient")) {
      mqttClient.publish("iot_mesh","Ready!");
      mqttClient.subscribe("iot_mesh");
    } 
  }
}

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  mqttClient.publish("iot_mesh", msg.c_str());
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length+1);
  memcpy(cleanPayload, payload, length);
  cleanPayload[length] = '\0';
  String msg = String(cleanPayload);
  free(cleanPayload);

  String targetStr = String(topic).substring(16);

  if(targetStr == "gateway")
  {
    if(msg == "getNodes")
    {
      auto nodes = mesh.getNodeList(true);
      String str;
      for (auto &&id : nodes)
        str += String(id) + String(" ");
      mqttClient.publish("iot_mesh", str.c_str());
    }
  }
  else if(targetStr == "broadcast") 
  {
    mesh.sendBroadcast(msg);
  }
  else
  {
    uint32_t target = strtoul(targetStr.c_str(), NULL, 10);
    if(mesh.isConnected(target))
    {
      mesh.sendSingle(target, msg);
    }
    else
    {
      mqttClient.publish("iot_mesh", "Client not connected!");
    }
  }
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}