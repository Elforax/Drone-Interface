#define DELAY 500
#define TESTS 8

#include <CAN.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("CAN Tester");

  // start the CAN bus at 500 kbps
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }
  delay(3000);
}

bool is_sending = false;
unsigned int running_test = 0;

void loop() {
  while(running_test < TESTS){
    sender(running_test++);
  }
  reciever();
}

void sender(unsigned int test_id){
  Serial.print("Running test ");
  Serial.println(test_id);

  bool test_passed = false;
  // add switch case with test here!
  switch(test_id){
    case 0:
      Serial.print("Sending packet ... ");

      CAN.beginPacket(0x110);
      CAN.write(0);
      CAN.endPacket();

      Serial.println("done");
      delay(1000);
      
      Serial.print("Sending packet ... ");

      CAN.beginPacket(0x110);
      CAN.write(1);
      CAN.endPacket();

      Serial.println("done");
      test_passed = true;
    break;
    
    case 1:
      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x100);
      CAN.write(0);
      CAN.endPacket();

      Serial.println("done");
      test_passed = true;
    break;
    case 2:

    break;
    case 3:

    break;
    case 4:

    break;
    case 5:
      Serial.print("Sending packet ... ");

      CAN.beginPacket(0x115);
      CAN.write(100);
      CAN.write(100);
      CAN.write(100);
      CAN.write(100);
      CAN.write(100);
      CAN.endPacket();

      Serial.println("done");
      test_passed = true;
    break;
    default:
      Serial.println("Unknown Test");
    break;
  }
  
  delay(DELAY);

  Serial.println("");
  Serial.println("");
  
  reciever();
  if(test_passed){
    Serial.print("Test ");
    Serial.print(test_id);
    Serial.println(" Done..");
  }else{
    Serial.print("Test ");
    Serial.print(test_id);
    Serial.println(" Fail..");
  }
}

void reciever(){
  int packetSize = CAN.parsePacket();
  
  if (packetSize) {
    // received a packet
    Serial.print("Received ");

    if (CAN.packetExtended()) {
    }

    if (CAN.packetRtr()) {
    }

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

      // only print packet data for non-RTR packets
      while (CAN.available()) {
        Serial.print(CAN.read());
      }
      Serial.println();
    }

    Serial.println();
  }

  delay(DELAY);
}
