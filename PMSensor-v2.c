/**
	Daryl Albano
	02/03/17
	PMSensor-v2.c
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
}

void loop() {
  duration = pulseIn(pmPin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) > sampletime_ms) {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
	writeToFile();
	lowpulseoccupancy = 0;
    starttime = millis();
  }
}

void writeToFile() {
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if(dataFile) {
    dataFile.print(lowpulseoccupancy);
    dataFile.print(",");
    dataFile.print(ratio);
    dataFile.print(",");
    dataFile.println(concentration);

    Serial.print("Low pulse occupancy: ");
    Serial.print(lowpulseoccupancy);
    Serial.print(",");
    Serial.print(" Ratio: ");
    Serial.print(ratio);
    Serial.print(",");
    Serial.print(" Concentration: ");
    Serial.println(concentration);
    
    dataFile.close();
  }
  
  else Serial.println("error opening file");
}