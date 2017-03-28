#include<SPI.h>
#include<SD.h>

const int cardSelect = 10;

void setup() {
	Serial.begin(9600);
	while(!Serial) {
		;
	}
	
	Serial.print("Initializing SD card...");
	
	if(!SD.begin(chipSelect)) {
		Serial.println("Card failed, or not present");
		return;
	}
	Serial.println("Card initialized");
}

void loop() {
	// Create string to assemble data to log
	String dataString = " ";
	
	// Read sensors
	// Replace with "Hello World"
	for(int analogPin = 0; analogPin < 3; analogPin++) {
		int sensor = analogRead(analogPin);
		dataString += String(sensor);
		if(analogPin < 2) {
			dataString = ",";
		}
	}
	
	// Open the file. (Only one file can be open at a time)
	File dataFile = SD.open("datalog.txt", FILE_WRITE);
	
	// Write to file if it is available
	if(dataFile) {
		dataFile.println(dataString);
		dataFile.close();
		// Print to the serial port
		Serial.println(dataString);
	}
	
	// Print error message if file isn't open
	else Serial.println("error opening datalog.txt");
}