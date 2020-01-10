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
  reciever();
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
      Serial.println("Lock servo toggle");
      Serial.println("");
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
      Serial.println("Recieve Lock status");
      Serial.println("");
      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x100);
      CAN.write(0);
      CAN.endPacket();

      Serial.println("done");
      test_passed = true;
    break;
    case 2:
      Serial.println("Switch Ext. Servo");
      Serial.println("");
      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x113);
      CAN.write(1);
      CAN.endPacket();

      Serial.println("done");

      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x101);
      CAN.write(0);
      CAN.endPacket();

      Serial.println("done");
      delay(1000);
      reciever();
      
      delay(1000);
      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x113);
      CAN.write(0);
      CAN.endPacket();

      Serial.println("done");
      test_passed = true;
    break;
    case 3:
      Serial.println("Move Ext. Servo to 200 [0-255]");
      Serial.println("");
      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x111);
      CAN.write(200);
      CAN.endPacket();

      Serial.println("done");

      delay(1000);
      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x101);
      CAN.write(0);
      CAN.endPacket();

      Serial.println("done");
      delay(1000);
      reciever();
      test_passed = true;
    break;
    case 4:
      Serial.println("Move Ext. Servo to 90 degrees");
      Serial.println("");
      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x112);
      CAN.write(90);
      CAN.endPacket();

      Serial.println("done");
      
      delay(1000);
      Serial.print("Sending packet ... ");
    
      CAN.beginPacket(0x101);
      CAN.write(0);
      CAN.endPacket();

      Serial.println("done");
      delay(1000);
      reciever();
      test_passed = true;
    break;
    case 5:
      Serial.println("Activate Buzzer for 5 sec");
      Serial.println("");
      Serial.print("Sending packet ... ");

      CAN.beginPacket(0x115);
      CAN.write(0); // 1
      CAN.write(0); // 10
      CAN.write(50); // 100
      CAN.endPacket();

      Serial.println("done");
      test_passed = true;
    break;
    case 6:
      Serial.println("Get Indicator status");
      Serial.println("");
      Serial.print("Sending packet ... ");

      CAN.beginPacket(0x103);
      CAN.write(0); // 1
      CAN.endPacket();

      Serial.println("done");
      delay(1000);
      reciever();
      test_passed = true;
    break;
    case 7:
      Serial.println("Internal reset module arduino");
      Serial.println("");
      Serial.print("Sending packet ... ");

      CAN.beginPacket(0x117);
      CAN.write(0);
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
  Serial.println("");
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
