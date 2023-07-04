//Reads HC-SR04 sensors and turns on and off (with a delay)sound for art installation
//6/17/2023

#include <NewPing.h>

#define SONAR_NUM    3 // Number of sensors.
#define MAX_DISTANCE 40 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 50 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo)
#define Relay_OFF_TIME 3000 // Time in milliseconds for relay to stay ON
unsigned long RelayTimer[SONAR_NUM]; // Timer for each relay
int dis = 40; //trashold distance
int RelayState[SONAR_NUM] = {HIGH, HIGH, HIGH}; // Initialize Relay states
unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(10, 10, MAX_DISTANCE),
  NewPing(11, 11, MAX_DISTANCE),
  NewPing(12, 12, MAX_DISTANCE),
};

void setup() {
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pingTimer[0] = millis() + 75;           // First ping starts at 75ms
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
}

void loop() {
  if (millis() >= pingTimer[currentSensor]) {
    pingTimer[currentSensor] += PING_INTERVAL * SONAR_NUM; // Set next time this sensor will be pinged.
    sonar[currentSensor].timer_stop();                     // Make sure previous timer is canceled before starting a new ping (insurance).
    cm[currentSensor] = sonar[currentSensor].ping_cm();    // Do the ping and store the distance.

    if (cm[currentSensor] > 0 && cm[currentSensor] <= dis) {
      RelayState[currentSensor] = LOW;
      RelayTimer[currentSensor] = millis() + Relay_OFF_TIME; // Set the timer for this relay
    }
    currentSensor = (currentSensor + 1) % SONAR_NUM; // Move to the next sensor

    if (currentSensor == 0) {
      for (uint8_t i = 0; i < SONAR_NUM; i++) {
        if (RelayState[i] == LOW && millis() >= RelayTimer[i]) {
          RelayState[i] = HIGH; // Reset the relay state after Relay_OFF_TIME
        }
      }
      Relay(); // Turn ON the relays according to State.
    }
  }
}

void Relay() { // relay Write Function
  const uint8_t RelayPins[] = {3, 4, 5};

  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    digitalWrite(RelayPins[i], RelayState[i]);
  }
}
