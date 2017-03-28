/**
  Daryl Albano
  02/03/17
  PMSensor-v3.c
  Collects analyzed data received from sensor. Stores data to .csv file
**/

#include<SPI.h>
#include<SD.h>

const int pmPin = 8;
const int cardSelect = 10;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
String newFilename;

void setup() {
  Serial.begin(9600);
  while(!Serial) {;}
  Serial.print("Initializing SD card...");
  
  if(!SD.begin(cardSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized");
  pinMode(pmPin,INPUT);
  starttime = millis();
  newFilename = setFilename();
  // Remove trim if it has no effect
  newFilename.trim();
}

void loop() {
  duration = pulseIn(pmPin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) > sampletime_ms) {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;
  writeToFile();
  lowpulseoccupancy = 0;
    starttime = millis();
  }
}

/*
	Method to write data to file
*/
void writeToFile() {
  char tempCharFilename[newFilename.length() + 1];
  newFilename.toCharArray(tempCharFilename, sizeof(tempCharFilename));
  File dataFile = SD.open(tempCharFilename, FILE_WRITE);
  if(dataFile) {
    dataFile.print(lowpulseoccupancy);
    dataFile.print(",");
    dataFile.print(ratio);
    dataFile.print(",");
    dataFile.println(concentration);

    Serial.print(" Concentration = ");
    Serial.print(concentration);
	Serial.println(" pcs / 0.01 cf");
    
    dataFile.close();
  }
  
  else Serial.println("error opening file");
}

// Method to create a unique and incremental filename. Returns fileName variable
// Solved 02/21/17 - Issues was the length of the file name (8.3) > 8 characters. 3 characters ex) 12345678.123
String setFilename() {
  String fileName = String();
  unsigned int fileN = 1;
  while (!fileN == 0) {
    fileName = "pm_";
    fileName += fileN;
    fileName += ".csv";
	
    char charFilename[fileName.length() + 1];
    fileName.toCharArray(charFilename, sizeof(charFilename));
    
    if(SD.exists(charFilename)) {
      fileName += " exists. ";
      Serial.println(fileName);
      fileN++;
    }
	else return fileName;
  }
}