#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

RTC_DS3231 rtc; // Create an RTC object

LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int trigPin = 7; // Ultrasound Trigger Pin to Pin 7
const int echoPin = 6; // Ultrasound Echo Pin to Pin 6

bool objectDetected = false; // Flag to track object detection
unsigned long objectStart = 0; // Timestamp when object was detected

void setup() {
  Serial.begin(9600); // Begin serial communication
  pinMode(trigPin, OUTPUT); // Set trigger pin to output
  pinMode(echoPin, INPUT); // Set echo pin to input
  lcd.init(); // Initialize LCD
  lcd.backlight(); // Turn on LCD backlight
  lcd.setCursor(0, 0); // Set LCD cursor to the first line
  lcd.print("Time: "); // Print label for real-time clock
  Wire.begin(); // Begin I2C communication
  rtc.begin(); // Begin RTC module time count

if (!rtc.begin()) {
    Serial.println("Couldn't find RTC"); //if rtc module is not wired correctly, indicate as such
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time..."); //if rtc loses power or battery runs out, allow for adjusting time manually
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now(); // Get current time from RTC module
  lcd.setCursor(6, 0); // Set cursor position to print real-time clock
  printTime(now); // Print real-time clock on LCD and serial monitor

  // Trigger ultrasonic sensor and check for object detection
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  // If an object is detected, store the timestamp
  if (distance > 0 && distance < 400) { // Modify measurement as needed
    if (!objectDetected) {
      objectStart = now.unixtime();
      objectDetected = true;
    }
  } else {
    // If no object is detected and object was previously detected, print the time when the object was removed
    if (objectDetected) {
      objectDetected = false;
      lcd.setCursor(0, 1); // Set cursor to the second line of the LCD
      lcd.print("Dash: "); // Print label for object removal time
      printTime(DateTime(objectStart)); // Print time when the object was removed
    }
  }

  delay(500); // Delay for smoother display
}

void printTime(const DateTime& time) {
  lcd.print(formatDigits(time.hour())); // Print hours
  lcd.print(':');
  lcd.print(formatDigits(time.minute())); // Print minutes
  lcd.print(':');
  lcd.print(formatDigits(time.second())); // Print seconds
}

String formatDigits(int digits) {
  String formattedDigits = String(digits);
  if (digits < 10) {
    formattedDigits = '0' + formattedDigits; // Add leading zero if the value is less than 10
  }
  return formattedDigits;
}