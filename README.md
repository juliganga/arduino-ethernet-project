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
#### Wait for a new client, read HTTP request made by client

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
        }
  ```
<br>

This project was made for a school proyect, published to help other similar proyects and allow expansion of the concept :)
