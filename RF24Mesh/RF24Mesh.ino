#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include "printf.h"

/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

#define nodeID 5

const unsigned long interval = 10000; //ms  // How often to send the payload

uint32_t displayTimer = 0;

struct payload_commands_t
{
  unsigned long nodeId;
  unsigned long command;
  unsigned long value;
};

struct payload_sensordata_t
{
  unsigned long nodeId;
  char          cStatus;    /**< Status configured or not */
  char          HardwareID[3];  /**< Hardware unique ID */
  unsigned int  ambientTemp;
  unsigned int  nodePower;
  char          ambientHumidity;
  char          airQuality;
  char          ambientlux;
  unsigned char errorCount;
  char          nodeInfo[3];
};


void setup() {

  Serial.begin(115200);
  printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin(); 
}

void loop() {
  mesh.update();

  payload_sensordata_t payload = { nodeID, 0, 'a', 5, 0, 0, 0, 0, 0, 'b'};
    
  // Send to the master node every second
  if (millis() - displayTimer >= interval) {
    displayTimer = millis();

    if(!mesh.write(&payload,'G',sizeof(payload))){

      // If a write fails, check connectivity to the mesh network
      if ( !mesh.checkConnection() ) {
        //refresh the network address
        Serial.println("Renewing Address");
        mesh.renewAddress();
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: "); Serial.println(displayTimer);
    }
  }

  while (network.available()) {
    RF24NetworkHeader header;
    payload_commands_t payload_cmd;
    network.read(header, &payload_cmd, sizeof(payload_cmd));
    Serial.print("Received packet: "); 
    Serial.print(payload_cmd.nodeId); 
    Serial.print("\r cmd: ");
    Serial.print(payload_cmd.command); 
    Serial.print("\r value: ");
    Serial.print(payload_cmd.value); 
    Serial.print("\n");
  }
}






