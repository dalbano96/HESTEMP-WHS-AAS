/**
  Daryl Albano
  02/03/17
  PMSensor-v7.c
  Collects analyzed data received from sensor. Stores data to .csv file
**/

#include<SPI.h>
#include<SD.h>
#include<LCD.h>
#include<LiquidCrystal_I2C.h>
#include<Wire.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7);

const int pmPin = 8;
const int cardSelect = 4;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
double batteryVoltage = 7.4;
String newFilename;

/**
	Function to retrieve the current date and time
	@pre retrieve date and time
	@post set file date and time
*/
/* void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  *date = FAT_DATE(now.year(), now.month(), now.day());
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
} */

/**
	Initial program setup
*/
void setup() {
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);
  lcd.clear();

  while (!Serial) {
    ;
  }
  Serial.print("Initializing SD card...");
  lcd.setCursor(0, 0);
  lcd.print("Initializing SD card...");
  delay(1000);
  lcd.clear();

  if (!SD.begin(cardSelect)) {
    Serial.println("Card failed, or not present");
    lcd.setCursor(0, 0);
    lcd.print("Card failed, or not present");
    delay(1000);
    lcd.clear();
    return;
  }
  Serial.println("Card initialized");

  lcd.setCursor(0, 0);
  lcd.print("Card initialized");

  pinMode(pmPin, INPUT);
  starttime = millis();
  newFilename = setFilename();
  // Remove trim if it has no effect
  newFilename.trim();

  /* // Checking the connection to the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC!");
    lcd.setCursor(0, 0);
    lcd.print("Couldn't find RTC!");
    delay(1000);
    lcd.clear();
    while (1);
  } */

  /* // Checking if the RTC is running
  if (!rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    lcd.setCursor(0, 0);
    lcd.print("RTC is NOT running!");
    delay(1000);
    lcd.clear();
  } */

  // Set the date and time for the RTC
  // F(__DATE__) - Retrieve the system's current date
  // F(__TIME__) - Retrieve teh system's current time
  /***  Adjust date and time once, then upload, then comment out and upload operation code ***/
  /* rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  SdFile::dateTimeCallback(dateTime); */

  Serial.begin(57600);
}

/**
	Program entry point
*/
void loop() {
  duration = pulseIn(pmPin, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;
  if ((millis() - starttime) > sampletime_ms) {
    lcd.clear();
    ratio = lowpulseoccupancy / (sampletime_ms * 10.0);
    concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;
    writeToFile();
    printBatteryPerc();
    lowpulseoccupancy = 0;
    starttime = millis();
  }
}

/**
	Method to write data to file
	Pre: File has not been opened for writing
	Post: File updated with new entry
*/
void writeToFile() {
  lcd.clear();
  char tempCharFilename[newFilename.length() + 1];
  newFilename.toCharArray(tempCharFilename, sizeof(tempCharFilename));

  File dataFile = SD.open(tempCharFilename, FILE_WRITE);
  // DateTime now = rtc.now();
  if (dataFile) {
    dataFile.print(lowpulseoccupancy);
    dataFile.print(",");
    dataFile.print(ratio);
    dataFile.print(",");
    dataFile.print(concentration);
    dataFile.print(",");
    /* dataFile.print(now.year(), DEC);
    dataFile.print(now.month(), DEC);
    dataFile.print(now.day(), DEC);
    dataFile.print(",");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(':');
    dataFile.println();

    /* Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.print(now.day(), DEC);
    Serial.print("/");
    Serial.print(now.year(), DEC);
    Serial.println();

    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    Serial.print(now.second(), DEC); */
    Serial.println();

    Serial.print(" Concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs / 0.01 cf");

    lcd.setCursor(0, 0);
    lcd.print("conc:");
    lcd.setCursor(5, 0);
    lcd.print(concentration);
    lcd.setCursor(0, 1);
    lcd.print(newFilename);


    // lcd.print(now.hour(), DEC);
    // lcd.print(":");
    // lcd.print(now.minute(), DEC);
    // lcd.print(":");
    // lcd.print(now.second(), DEC);
    // lcd.println();

    dataFile.close();
  }

  else Serial.println("error opening file");
}

/**
	Function to create a unique and incremental filename. Returns fileName variable
	Solved 02/21/17 - Issues was the length of the file name (8.3) > 8 characters. 3 characters ex) 12345678.123
	Pre: File name not declared
	Post: File name declared
*/
String setFilename() {
  String fileName = String();
  unsigned int fileN = 1;
  while (!fileN == 0) {
    fileName = "pm_";
    fileName += fileN;
    fileName += ".csv";

    char charFilename[fileName.length() + 1];
    fileName.toCharArray(charFilename, sizeof(charFilename));

    if (SD.exists(charFilename)) {
      fileName += " exists. ";
      Serial.println(fileName);
      fileN++;
    }
    else return fileName;
  }
}

/**
	Function to print battery voltage and percentage
	Pre: Analog value read
	Post: Percentage and voltage values calculated from analog value
*/
void printBatteryPerc() {
  int sensorValue = analogRead(A0);
  float voltage = (sensorValue * (5.0 / 1023.0)) * 2;
  float deltaPercentage = ((voltage - batteryVoltage) / 0.7) * 100;
  float percentage = 100.0 - deltaPercentage;
  Serial.print("Votage: ");
  Serial.println(voltage);
  Serial.print("Percentage: ");
  Serial.print(percentage);
  Serial.println("%");
  lcd.setCursor(0, 1);
  //	lcd.print("Volt: ");
  //	lcd.print(voltage);
  lcd.print("Perc: ");
  lcd.print(percentage);
  lcd.print("%");
  Serial.println();
  delay(1000);
}
