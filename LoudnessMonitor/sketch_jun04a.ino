#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
int  sensorAnalogPin = A0;    // Select the Arduino input pin to accept the Sound Sensor's analog output 
int  sensorDigitalPin = 7;    // Select the Arduino input pin to accept the Sound Sensor's digital output
int  analogValue = 0;         // Define variable to store the analog value coming from the Sound Sensor
int  digitalValue;            // Define variable to store the digital value coming from the Sound Sensor
int  setupstage = 0;
int  average = 0;
int  upbound = 0;
int  lowbound = 0;
bool runnin = false;
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Begun");
  #define LED 4
  #define button 5
  lcd.begin(16, 2);
  pinMode(sensorDigitalPin, INPUT);  // Define pin 7 as an input port, to accept digital input
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  pinMode(button, INPUT_PULLUP);
  lcd.print("Boot Done.");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(button) == LOW){
    switch (setupstage) {
      case 0:
        calibratebase();
        break;
      case 1:
        calibraterange();
        runnin = true;
        break;
    }
  }
  if ((runnin) && (analogRead(sensorAnalogPin) > upbound || analogRead(sensorAnalogPin) < lowbound)){
    digitalWrite(LED, 255);
    delay(900);
  }
  else {
    digitalWrite(LED, 0);
  }
}

void calibratebase() {
  lcd.begin(16, 2);
  lcd.print("Calibrating...");
  lcd.setCursor(0, 1);
  lcd.print("Be Quiet!!!");
  int values[50]; 
  int i = 0;
  while (i < 50){
    analogValue = analogRead(sensorAnalogPin); // Read the value of the analog interface A0 assigned to digitalValue
    digitalValue = digitalRead(sensorDigitalPin); // Read the value of the digital interface 7 assigned to digitalValue 
    values[i] = analogValue;
    delay(50);                  // Slight pause so that we don't overwhelm the serial interface
    i++;
  }
  i = 0;
  while (i < 50) {
    average += values[i];
    i++;
  }
  average = (average / 50);
  String tavg = String(average);
  String bruh = "Avg Value: " + tavg;
  lcd.setCursor(0, 1);
  lcd.print(bruh);
  delay(8000);
  lcd.setCursor(0, 1);
  lcd.print("Base Calibrated");
  delay(3000);
  lcd.setCursor(0, 1);
  lcd.print("Press Again.   ");
  setupstage = 1;
}

void calibraterange() {
  lcd.begin(16, 2);
  lcd.print("Calibrating...");
  lcd.setCursor(0, 1);
  lcd.print("Talk at normal v");
  delay(900);
  lcd.setCursor(0, 1);
  lcd.print("alk at normal vo");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print("lk at normal vol");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print("k at normal volu");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print(" at normal volum");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print("at normal volume");
  int values[100]; 
  int i = 0;
  int adjbase = 2.5;
  int limitcount = 0;
  while (i < 100){
    analogValue = analogRead(sensorAnalogPin); // Read the value of the analog interface A0 assigned to digitalValue
    digitalValue = digitalRead(sensorDigitalPin); // Read the value of the digital interface 7 assigned to digitalValue 
    values[i] = analogValue;
    delay(30);                  // Slight pause so that we don't overwhelm the serial interface
    if (values[i] > (average * adjbase)) {
      values[i] = (average * adjbase);
      limitcount++;
    } 
    else if (values[i] < (average / adjbase)) {
      values[i] = (average * adjbase);
      limitcount++;
    } //adjust outliers
    i++;
    if (limitcount == 5){
      i = 0;
      limitcount = 0;
    } //reset if there were too many outliers.
  }
  int values_size = sizeof(values) / sizeof(values[0]);
  qsort(values, values_size, sizeof(values[0]), sort_desc);
  upbound = (int) (values[2] * 1.02) ;  // replace with standard deviation?
  lowbound = (int) (values[97] * .98) ;
  String upstring = "Upbound: " + upbound;
  String downstring = "Lowbound: " + lowbound;
  lcd.begin(16, 2);
  lcd.print(upstring);
  lcd.setCursor(0, 1);
  lcd.print(downstring);
}

int sort_desc(const void *cmp1, const void *cmp2)//Stolen from Johnny Mopp on Stack Exchange
{
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  return a > b ? -1 : (a < b ? 1 : 0);
}
