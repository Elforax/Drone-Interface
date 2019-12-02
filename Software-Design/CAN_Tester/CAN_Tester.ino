// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Sender");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
}

bool is_sending = false;

void loop() {
  Serial.println("ID number please 0-2047:");
  //while(!Serial.available()){}
  int id = SerialReadInt();
  if(id <= 0x7FF){
    is_sending = true;
    while(is_sending){
      Serial.print("ID: ");
      Serial.println(id, HEX);

      Serial.println("byte amount please 1-8 (0 is cancel):");
      //while(!Serial.available()){}
      int byte_amount = SerialReadInt();
      if(byte_amount <= 8){
        Serial.println(byte_amount);
        if(byte_amount == 0){
          is_sending = false;
        }
      }
      delay(1000);
    }
  }
  delay(1000);
  //sender(0x001, 8);
}

int SerialReadInt(){
  int result;
  if(Serial.available() > 0){
   result = Serial.parseInt();
  }else{
    result = 0xFFF;
  }
  return result;
}

void sender(unsigned int id, unsigned int packet_size){
    // send packet: id is 11 bits, packet can contain up to 8 bytes of data
  if(packet_size > 8){
    packet_size = 8;
  }
  

  
  for(int i=0; i<packet_size;i++){
    Serial.print("information on byte (");
    Serial.print(i+1);
    Serial.println(")");
  }
  //CAN.beginPacket(id);
  
  //CAN.endPacket();
  //Serial.print("Sending packet ... ");
  //Serial.println("done");

  delay(1000);
}

void reciever(){
  
}
