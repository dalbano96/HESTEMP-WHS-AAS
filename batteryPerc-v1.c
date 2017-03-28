/**
 * Daryl Albano
 * 03/06/17
 * batteryPerc-v1.c
 * Displays voltage and battery percentage of Li-Po battery
 */

double batteryVoltage = 7.4;

void setup() {
  Serial.begin(9600);
}

void loop() {
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