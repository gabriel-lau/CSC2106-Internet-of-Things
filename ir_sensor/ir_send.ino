#include <M5StickCPlus.h>

#define IR_SEND_PIN 9 // The pin that the IR LED is attached to. Adjust as needed.
#include <IRremote.hpp>

void setup() {
  M5.begin();
  IrSender.begin(); // Initialize IR sender.
  Serial.begin(9600);
}

void loop() {
int input;
Serial.println("Welcome to the IR remote control system! \n");

Serial.println("Select the command to send: \n");
Serial.println("1. Light switched on \n");
Serial.println("2. Light switched off \n");
Serial.println("3. Fan switched on/off \n");
Serial.println("4. Fan low speed \n");
Serial.println("5. Fan medium speed \n");
Serial.println("6. Fan high speed \n");
Serial.println("7. Fan osciliate \n");
Serial.println("8. Fan stay still \n");
Serial.println("9. Fan stay still \n");
Serial.println("10. Aircon switch off \n");
Serial.println("11. Aircon switch on \n");
Serial.println("12. Aircon increase temp \n");
Serial.println("13. Aircon decrease temp \n");
// get input from user in hexadecimal format
scanf("%d", &input);

  IrSender.sendFAST(input, 2); // Send command 11 on pin 3 with 2 repeats.
  Serial.println("IR signal sent! \n");
  delay(5000); // Wait for 5 seconds before sending the signal again.
}