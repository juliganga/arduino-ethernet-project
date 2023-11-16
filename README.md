# Arduino Ethernet Proyect

## Introduction
Enables a local client to connect to an Arduino UNO that displays a website that allows control over the device.
<br>

## Instructions
#### You must have the following items:
- An arduino UNO (or other model that is compatible with an ethernet shield)
- An Ethernet Shield with an SD card reader
- Ethernet Cable
- An HTM file on the SD card (so that the arduino sends it to the client)

You also need to install [this library](https://github.com/bblanchon/ArduinoContinuousStepper "ContinousStepper by bblanchon") if you want to use the continous stepper like i did

## Relevant Code
#### Set-up IP address and MAC

```c++
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // set arduino shield's MAC address
byte ip[] = { 192, 168, 120, 10 }; // Set IP that you'll connect to through the browser
```
#### Check if SD card has certain file, create if not
```c++
  if (!SD.exists("registro.txt")) { // In case that a text file doesn't exist, create a new file naned whatever you want...
    Serial.println("registro.txt doesn't exist.");
    MyFile = SD.open("example.txt", FILE_WRITE); // ...here
    MyFile.close(); // close file
  }
```
#### Start ethernet, do it on setup()
```c++
  Ethernet.begin(mac, ip); // Start ethernet connection
```
#### Wait for a new client, read HTTP request made by client and react accordingly

```c++
EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = +client.read(); // Take http request to pass to "c"
        
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;

        if (c == '\n') { //if HTTP request has ended

          ///////////////
          readString.remove(0, 6); // Remove useless information so its easier to work with the newly recieved HTTP request
          readString.remove(10, 50); // ^^
          Serial.println(readString); // Print so i can tell whats going on


          if (readString.equals("ITEM_1_ON ")) {  // This has a whitespace as i wanted to preserve the full string from the website
            
            if (!LightOn){
              LightOn = true;
              MyReg = SD.open("registro.txt", FILE_WRITE); // Open txt file to write on it
              if (MyReg) {
                Serial.print("Writing to registro.txt...");
                MyReg.println("Se prendio la luz"); // message to write to file
                // close the file:
                MyReg.close();
                Serial.println("done.");
             }

              Serial.println("Luz ON");
              digitalWrite(relay, LOW);
            }

          }

          if (readString.equals("ITEM_1_OFF")) {

            if (LightOn){
              LightOn = false;
              Serial.println("Luz OFF");
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

          }
}
  ```
#### Send HTM page to client once they connect
```c++
File myFile = SD.open("control.htm");  // Prepares the file that the arduino will write
          if (myFile) { 
            // Read from the file until there's nothing else in it:
            while (myFile.available()) { 
              client.write(myFile.read()); // Sends the client the file over ethernet, letting us see the page
            }
            // close the file:
            myFile.close();
          }
```

## 

<br>

This project was made for a school proyect, published to help other similar proyects and allow expansion of the concept :)
