// the code can definitely have some improvements
// but you can get an idea on how you can make an arduino a webserver that responds to a user client :)



#include <SD.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>
#include <ContinuousStepper.h> // credits to https://github.com/bblanchon/ArduinoContinuousStepper



ContinuousStepper<FourWireStepper> stepper;
int relay = 14;       // RELAY PIN
int incremental = 0;  // TIME COUNTER

File MyReg;
File MyFile;

// Servo controls and states
Servo lockServo;
int lockPos = 15; // expressed in degrees
int unlockPos = 80;

boolean locked = true;
boolean StepperSpin = false; // To set the stepper to start spinning
boolean Spinning = false; // To tell that the stepper is spinning or not

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // physical mac address
byte ip[] = { 192, 168, 120, 20 };                    // local ip address

EthernetServer server(80);  //server port
String readString;


//////////////////////

void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  Serial.print("Starting SD..");
  if (!SD.begin(4)) Serial.println("failed");
  else Serial.println("ok");

  if (!SD.exists("registro.txt")) { // In case that a text file doesn't exist, create a new file naned whatever you want...
    Serial.println("registro.txt doesn't exist.");
    MyFile = SD.open("example.txt", FILE_WRITE); // ...here
    MyFile.close(); // close file
  }

  Ethernet.begin(mac, ip); // Start ethernet connection
  digitalWrite(10, HIGH);

  //delay(2000);
  server.begin();
  Serial.println("Ready");

  stepper.begin(18, 16, 17, 15); // Set up stepper motor pins
  lockServo.attach(3); // Set up Servo pin
  lockServo.write(lockPos); // Rotate it to the "locked" pos
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
}

void loop() {
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      // String cadena = ""; // realised that this is pointless 
      if (client.available()) {
        char c = +client.read(); // Take http request to pass to "c"
        
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
        }

        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          readString.remove(0, 6); // Remove useless information so its easier to work with the newly recieved HTTP request
          readString.remove(10, 50); // ^^
          Serial.println(readString); // Print so i can tell whats going on


          if (readString.equals("ITEM_1_ON ")) {  // This has a whitespace as i wanted to preserve the full string from the website
            digitalWrite(relay, LOW);


            MyReg = SD.open("registro.txt", FILE_WRITE); // Open txt file to write on it
            if (MyReg) {
              Serial.print("Writing to registro.txt...");
              MyReg.println("Se prendio la luz"); // message to write to file
              // close the file:
              MyReg.close();
              Serial.println("done.");
            }
          }

          if (readString.equals("ITEM_1_OFF")) {
            digitalWrite(relay, HIGH);

            MyReg = SD.open("registro.txt", FILE_WRITE);
            if (MyReg) {
              Serial.print("Writing to registro.txt...");
              MyReg.println("Se apago la luz");
              // close the file:
              MyReg.close();
              Serial.println("done.");
            }
          }

          if (readString.equals("ITEM_2_ON ")) {
            lockServo.write(unlockPos);

            MyReg = SD.open("registro.txt", FILE_WRITE);
            if (MyReg) {
              Serial.print("Writing to registro.txt...");
              MyReg.println("Se abrio la puerta");
              // close the file:
              MyReg.close();
              Serial.println("done.");
            }
          }

          if (readString.equals("ITEM_2_OFF")) {
            lockServo.write(lockPos);

            MyReg = SD.open("registro.txt", FILE_WRITE);
            if (MyReg) {
              Serial.print("Writing to registro.txt...");
              MyReg.println("Se cerro la puerta");
              // close the file:
              MyReg.close();
              Serial.println("done.");
            }
          }

          if (readString.equals("ITEM_3_ON ")) {
            if (!Spinning) { // if the stepper is NOT spinning
              stepper.spin(400); // Stepper goes backwards
              MyReg = SD.open("registro.txt", FILE_WRITE);
              if (MyReg) {
                Serial.print("Writing to registro.txt...");
                MyReg.println("Se Abrio la cortina");
                // close the file:
                MyReg.close();
                Serial.println("done.");
                StepperSpin = true; // Set stepper spin to true, so the stepper starts spinning
                Spinning = true; // same with spinning
              }
            }
          }

          if (readString.equals("ITEM_3_OFF")) {
            if (!Spinning) { // if the stepper is NOT spinning
              stepper.spin(-400); // Stepper goes backwards
              MyReg = SD.open("registro.txt", FILE_WRITE);
              if (MyReg) {
                Serial.print("Writing to registro.txt...");
                MyReg.println("Se cerro la cortina");
                // close the file:
                MyReg.close();
                Serial.println("done.");
                StepperSpin = true;
                Spinning = true;
              }
            }
          }


          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");       // Refreshing the page is possible, i didn't use it becuase there was no use for it in my case
          client.println();


          File myFile = SD.open("control.htm");  // Prepares the file that the arduino will write
          if (myFile) { 
            // Read from the file until there's nothing else in it:
            while (myFile.available()) { 
              client.write(myFile.read()); // Sends the client the file over ethernet, letting us see the page
            }
            // close the file:
            myFile.close();
          }

          delay(1); // wait a bit
          //stopping client
          client.stop();
          readString = ""; // Reset readString
          //}
        }
      }
    }
  }

  // loop code that runs within the arduino, no client needed

  if (StepperSpin == true && Spinning == true) {
    if (incremental < 8000) { // i use here a variable as a "timer" to avoid stopping the whole code execution, maybe theres a better way
      stepper.loop(); // here's also why i need to keep the code running (must be called on loop() )
      incremental = incremental + 1;
      Serial.println(incremental);
      if (incremental == 8000) { // whenever this reaches the limit, reset the incremental
        StepperSpin = false;
        incremental = 0;
        Spinning = false;
      }
    }
  }
}