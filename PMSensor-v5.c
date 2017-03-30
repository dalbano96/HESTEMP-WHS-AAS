/**
  Daryl Albano
  02/03/17
  PMSensor-v5.c
  Collects analyzed data received from sensor. Stores data to .csv file
**/

#include<SPI.h>
#include<SD.h>
#include<LCD.h>
#include<LiquidCrystal_I2C.h>
#include<Wire.h>
#include<RTClib.h>

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7);

const int pmPin = 4;
const int cardSelect = 10;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
double batteryVoltage = 7.4;
String newFilename;

void setup() {
	lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16,2);
  lcd.clear();
	
  while(!Serial) {;}
  Serial.print("Initializing SD card...");
	lcd.setCursor(0,0);
	lcd.print("Initializing SD card...");
	delay(1000);
	lcd.clear();
  
  if(!SD.begin(cardSelect)) {
    Serial.println("Card failed, or not present");
		lcd.setCursor(0,0);
		lcd.print("Card failed, or not present");
		delay(1000);
		lcd.clear();
    return;
  }
  Serial.println("Card initialized");
	
	lcd.setCursor(0,0);
	lcd.print("Card initialized");
  
	pinMode(pmPin,INPUT);
  starttime = millis();
  newFilename = setFilename();
  // Remove trim if it has no effect
  newFilename.trim();
  
  // Checking the connection to the RTC
  if (!rtc.begin()) {
	  Serial.println("Couldn't find RTC!");
		lcd.setCursor(0,0);
		lcd.print("Couldn't find RTC!");
		delay(1000);
		lcd.clear();
	  while(1);
  }

	// Checking if the RTC is running
  if(!rtc.isrunning()) {
	  Serial.println("RTC is NOT running!");
		lcd.setCursor(0,0);
		lcd.print("RTC is NOT running!");
		delay(1000);
		lcd.clear();
  }
  
  // Set the date and time for the RTC
  // F(__DATE__) - Retrieve the system's current date
  // F(__TIME__) - Retrieve teh system's current time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  Serial.begin(57600);
}

void loop() {
  duration = pulseIn(pmPin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) > sampletime_ms) {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62;
  writeToFile();
  printBatteryPerc();
  lowpulseoccupancy = 0;
    starttime = millis();
  }
}

/*
	Method to write data to file
*/
void writeToFile() {
	lcd.clear();
  char tempCharFilename[newFilename.length() + 1];
  newFilename.toCharArray(tempCharFilename, sizeof(tempCharFilename));
  File dataFile = SD.open(tempCharFilename, FILE_WRITE);
  DateTime now = rtc.now();
  if(dataFile) {
    dataFile.print(lowpulseoccupancy);
    dataFile.print(",");
    dataFile.print(ratio);
    dataFile.print(",");
    dataFile.print(concentration);
	dataFile.print(",");
	dataFile.print(now.year(), DEC);
	dataFile.print(now.month(), DEC);
	dataFile.print(now.day(), DEC);
	dataFile.print(",");
	dataFile.print(now.hour(), DEC);
	dataFile.print(now.minute(), DEC);
	dataFile.print(now.second(), DEC);
	dataFile.println();
	
	Serial.print(now.month(), DEC);
	Serial.print("/");
	Serial.print(now.day(), DEC);
	Serial.print("/");
	Serial.print(now.year(), DEC);
	Serial.println();
	
	Serial.print(now.hour(), DEC);
	Serial.print(":");
	Serial.print(now.minute(), DEC);
	Serial.print(":");
	Serial.print(now.second(), DEC);
	Serial.println();
	
	Serial.print(" Concentration = ");
    Serial.print(concentration);
	Serial.println(" pcs / 0.01 cf");
	
	lcd.setCursor(0,0);
	lcd.print("conc:");
	lcd.setCursor(5,0);
	lcd.print(concentration);
    
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

void printBatteryPerc() {
	int sensorValue = analogRead(A0);
	float voltage = 2 * (sensorValue * (5.0 / 1023.0));
	float percentage = ((voltage - batteryVoltage) / 0.7) * 100;
	Serial.print("Votage: ");
	Serial.println(voltage);
	Serial.print("Percentage: ");
	Serial.println(percentage);
	Serial.println();
	delay(1000);
}