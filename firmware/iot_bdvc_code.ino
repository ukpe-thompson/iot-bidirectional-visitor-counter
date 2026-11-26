// ============================================================================
// Project: IoT-Based Bidirectional Visitor Counter with Automatic Light Control
// Author:  Ukpe Thompson Pedro
// Registration No.: MOUAU/CME/17/98277
// Department: Computer Engineering
// Michael Okpara University of Agriculture, Umudike
// Board:   NodeMCU ESP8266
// ============================================================================

// Blynk Cloud Configuration (Replace placeholders with your Blynk credentials)
#define BLYNK_TEMPLATE_ID "your_blnk_template_id"
#define BLYNK_TEMPLATE_NAME "IOT VISITOR COUNTER WITH AUTO LIGHT CONTROL"
#define BLYNK_AUTH_TOKEN "your_blnk_auth_token"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// OLED Display Configuration (0.96" 128x64 I2C SSD1306)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (-1 shares ESP8266 reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi & Blynk Authentication Credentials
char auth[] = "your_blnk_auth_token"; // Blynk App Auth Token
char ssid[] = "your_wifi_SSID";       // Wi-Fi Network SSID
char pass[] = "your_wifi_password";   // Wi-Fi Network Password

// Hardware Pin Definitions
#define irPin1 14 // Entry/Outer IR Sensor connected to GPIO14 (D5)
#define irPin2 12 // Exit/Inner IR Sensor connected to GPIO12 (D6)
#define relay 0   // Active-LOW Relay Module connected to GPIO0 (D3)

// Occupancy Tracking & State-Machine Variables
int sensorState1 = 0; // Current digital reading of IR Sensor 1
int sensorState2 = 0; // Current digital reading of IR Sensor 2
int in = 0;           // Cumulative count of visitors entering
int out = 0;          // Cumulative count of visitors exiting
int now = 0;          // Net real-time occupancy (in - out)
int count = 0;        // Running occupancy counter
boolean state1 = true; // lock flag for IR Sensor 1
boolean state2 = true; // lock flag for IR Sensor 2
int i = 1;            // Sequence step tracker (1 = awaiting first trigger, 2 = awaiting second trigger)

// Blynk Virtual LED Widget on Virtual Pin V0 (mirrors physical relay state)
WidgetLED light(V0);

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi and Blynk Cloud
  Blynk.begin(auth, ssid, pass);
  delay(1000);

  // Initialize I2C OLED display at address 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);

  // Configure GPIO pin modes and initialize relay to OFF (Active-LOW: HIGH = OFF)
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  Serial.println("Visitor Counter Demo");

  // Splash Screen 1: Project Title
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(30, 20);
  display.print("IOT Visitor");
  display.setCursor(40, 40);
  display.print("Counter");
  display.setCursor((display.width() / 2) - 10, 55);
  display.print("By");
  display.display();
  delay(4000);

  // Splash Screen 2: Author & Matriculation Details
  display.clearDisplay();
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.print("Ukpe Thompson Pedro");
  display.setCursor(10, 40);
  display.print("MOUAU/CME/17/98277");
  display.display();
  delay(4000);
}

void loop() {
  Blynk.run(); // Maintain active Blynk cloud connection

  // Read current logic states from both IR sensors (Active-LOW: 0 = obstacle detected)
  sensorState1 = digitalRead(irPin1);
  sensorState2 = digitalRead(irPin2);

  // --- ENTRY SEQUENCE: IR Sensor 1 triggered first, followed by IR Sensor 2 ---
  if (!digitalRead(irPin1) && i == 1 && state1) {
    delay(100);
    i++;            // Advance sequence to await IR Sensor 2
    state1 = false; // Lock Sensor 1 until beam is cleared
  }

  if (!digitalRead(irPin2) && i == 2 && state2) {
    Serial.println("Entering into room");
    delay(100);
    i = 1;          // Reset sequence tracker
    count++;
    in++;
    Serial.print("No of persons inside the room: ");
    Serial.println(count);
    state2 = false; // Lock Sensor 2 until beam is cleared
  }

  // --- EXIT SEQUENCE: IR Sensor 2 triggered first, followed by IR Sensor 1 ---
  if (!digitalRead(irPin2) && i == 1 && state2) {
    delay(100);
    i = 2;          // Advance sequence to await IR Sensor 1
    state2 = false; // Lock Sensor 2 until beam is cleared
  }

  if (!digitalRead(irPin1) && i == 2 && state1) {
    Serial.println("Exiting from room");
    delay(100);
    count--;
    out++;
    Serial.print("No of persons inside the room: ");
    Serial.println(count);
    i = 1;          // Reset sequence tracker
    state1 = false; // Lock Sensor 1 until beam is cleared
  }

  // Re-arm sensor lock flags once each IR beam is unobstructed (HIGH)
  if (digitalRead(irPin1)) {
    state1 = true;
  }
  if (digitalRead(irPin2)) {
    state2 = true;
  }

  // Compute current net occupancy
  now = in - out;

  // --- AUTOMATED RELAY CONTROL & OLED DISPLAY UPDATE ---
  if (now <= 0) {
    // Room is empty: de-energize relay (turn light OFF)
    digitalWrite(relay, HIGH);
    light.off();

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 15);
    display.print("No Visitor");
    display.setCursor(5, 40);
    display.print("Light Off");
    display.display();

    Serial.println("No Visitors! Light Off");
    delay(100);
  } 
  else {
    // Room is occupied: energize relay (turn light ON) and show counts
    digitalWrite(relay, LOW);
    light.on();

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(15, 0);
    display.print("Current Visitors");
    display.setTextSize(2);
    display.setCursor(50, 15);
    display.print(now);
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.print("IN: ");
    display.print(in);
    display.setTextSize(1);
    display.setCursor(70, 40);
    display.print("OUT: ");
    display.print(out);
    display.display();

    Serial.print("Current Visitors: ");
    Serial.println(now);
    Serial.print("IN: ");
    Serial.println(in);
    Serial.print("OUT: ");
    Serial.println(out);
    delay(100);
  }

  // Push real-time occupancy metrics to Blynk Virtual Pins
  Blynk.virtualWrite(V1, in);  // Total Visitors In
  Blynk.virtualWrite(V2, out); // Total Visitors Out
  Blynk.virtualWrite(V3, now); // Net Current Visitors
  delay(100);
}

