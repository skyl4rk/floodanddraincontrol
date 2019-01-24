//-----------------------------------------------------------
//       FLOOD AND DRAIN PUMP TIMER WITH
//           HEAT MAT THERMOSTAT AND
//      MORSE CODE FLASH OF SENSOR VALUES
//-----------------------------------------------------------
// Pump Timer with Sensor Control and Morse Readout
// Capacitive Soil Moisture Sensor with AOUT to pin 1
// LED on Pin 13 will flash sensor values in morse code
// M indicates moisture sensor value, H indicates thermistor value
// Pump Relay is on Pin 4
// Pump is activated once every 24 hours but will only run if soil moisture sensor indicates dry soil
// Pump will turn off if soil becomes moist during pumping.
// Change pumpCycleOnDuration to set pump run time
// Change pumpCycleOffDuration to set delay between pump runs
// Ground Pin 12 with a button switch to manually run pump once 
// Thermistor is connected to 3.3V and Pin 0, with a 10k Ohm Resistor from Pin 0 to Ground
// Heat relay is on Pin 5
// Confirm DRYSOILSET, WETSOILSET, HEATSET values and pump cycle durations prior to use 
// Paul VandenBosch, 20190119

#define DRYSOILSET 460  // Moisture sensor Value that triggers watering, soil is dry at this value
#define WETSOILSET 400  // Moisture sensor Value that shuts off water pump
#define RELAYPIN 4      // Pin used for pump relay
#define MOISTUREPIN 1       // Analog pin used for moisture sensor
#define BUTTONPIN 12    // Pin used to start manual pump by grounding it
#define HEATSENSORPIN 0 // Pin used for heat sensor
#define HEATSET 340     // Sensor value to turn on heater
#define HEATRELAYPIN 5  // Pin used for heat relay  

int moistureSensor = analogRead(MOISTUREPIN);
int heatSensor = analogRead(HEATSENSORPIN);
int pumpToggle = 1;
unsigned long pumpCycleStartTime = 0;
unsigned long pumpCycleOnDuration = 50000;  // test value: 50 seconds: 50000
unsigned long pumpCycleOffDuration = 86400000;  // test value: 24 hours: 86400000
unsigned long pumpOffTime;
unsigned long currentMillis;

// Simple Arduino Morse Beacon
// ****** Morse Beacon Begins ******

#define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))
#define SPEED  (12)
#define DOTLEN  (1200/SPEED)
#define DASHLEN  (4*(1200/SPEED))

int morseLEDpin = 13 ;

struct t_mtab {
  char c, pat;
} ;

struct t_mtab morsetab[] = {
  {'.', 106},
  {',', 115},
  {'?', 76},
  {'/', 41},
  {'A', 6},
  {'B', 17},
  {'C', 21},
  {'D', 9},
  {'E', 2},
  {'F', 20},
  {'G', 11},
  {'H', 16},
  {'I', 4},
  {'J', 30},
  {'K', 13},
  {'L', 18},
  {'M', 7},
  {'N', 5},
  {'O', 15},
  {'P', 22},
  {'Q', 27},
  {'R', 10},
  {'S', 8},
  {'T', 3},
  {'U', 12},
  {'V', 24},
  {'W', 14},
  {'X', 25},
  {'Y', 29},
  {'Z', 19},
  {'1', 62},
  {'2', 60},
  {'3', 56},
  {'4', 48},
  {'5', 32},
  {'6', 33},
  {'7', 35},
  {'8', 39},
  {'9', 47},
  {'0', 63}
} ;

void
dash()
{
  digitalWrite(morseLEDpin, HIGH) ;
  delay(DASHLEN);
  digitalWrite(morseLEDpin, LOW) ;
  delay(DOTLEN) ;
}

void
dit()
{
  digitalWrite(morseLEDpin, HIGH) ;
  delay(DOTLEN);
  digitalWrite(morseLEDpin, LOW) ;
  delay(DOTLEN);
}

void
send(char c)
{
  int i ;
  if (c == ' ') {
    Serial.print(c) ;
    delay(7 * DOTLEN) ;
    return ;
  }
  for (i = 0; i < N_MORSE; i++) {
    if (morsetab[i].c == c) {
      unsigned char p = morsetab[i].pat ;
      Serial.print(morsetab[i].c) ;

      while (p != 1) {
        if (p & 1)
          dash() ;
        else
          dit() ;
        p = p / 2 ;
      }
      delay(2 * DOTLEN) ;
      return ;
    }
  }
  /* if we drop off the end, then we send a space */
  Serial.print("?") ;
}

void
sendmsg(char *str)
{
  while (*str)
    send(*str++) ;
  Serial.println("");
}
// ****** Morse Beacon Ends ******

void setup() {
  Serial.begin(9600);
  pinMode(MOISTUREPIN, INPUT);
  pinMode(RELAYPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(HEATSENSORPIN, INPUT);
  pinMode(HEATRELAYPIN, OUTPUT);

  // ****** Morse Beacon Begins ******
  pinMode(morseLEDpin, OUTPUT) ;
  // ****** Morse Beacon Ends ******
}

void loop() {
  moistureSensor = analogRead(MOISTUREPIN);
  heatSensor = analogRead(HEATSENSORPIN);
  currentMillis = millis();

  if (heatSensor < HEATSET) {
    digitalWrite(HEATRELAYPIN, HIGH);
  }
  else {
    digitalWrite(HEATRELAYPIN, LOW);
  }

  if (digitalRead(BUTTONPIN) == LOW) {
    digitalWrite(HEATRELAYPIN, LOW);
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("");
    Serial.print("Manual Engagement of Pump by Button for ");
    Serial.print(pumpCycleOnDuration / 1000);
    Serial.println(" Seconds Run Time");
    delay(pumpCycleOnDuration);
    digitalWrite(RELAYPIN, LOW);
  }

  if (moistureSensor < WETSOILSET) {     // turn off pump if soil is wet
    digitalWrite(RELAYPIN, LOW);
  }

  if (pumpCycleStartTime < currentMillis && moistureSensor > DRYSOILSET) {  // Pump will be authorized if past start time and moisture sensor shows dry soil
    pumpOffTime = currentMillis + pumpCycleOnDuration;               // pumpOffTime sets the maximum time that pump can run
    pumpToggle = 1;                                             // pumpToggle authorizes pump to start
    pumpCycleStartTime = currentMillis + pumpCycleOnDuration + pumpCycleOffDuration;  // reset pump start time to next day
  }

  if (pumpToggle == 1) {                           // test if pump is toggled
    if (pumpOffTime > currentMillis) {                  // check to see if pump on duration time has passed
      if (moistureSensor > WETSOILSET) {                  // check to see if soil is wet
        digitalWrite(HEATRELAYPIN, LOW);
        digitalWrite(RELAYPIN, HIGH);                     // turn pump on
      }
    }
    else {                                       // if past pump on duration time, turn pump off
      pumpToggle = 0;                            // turn off pump toggle
      digitalWrite(RELAYPIN, LOW);                      // turn off pump
    }
  }

  if (pumpToggle == 1) {
    Serial.println("");
    Serial.print("Pump Time Remaining: ");
    Serial.print((pumpOffTime - currentMillis) / 1000);
    Serial.println(" Seconds");
    Serial.println("");
  }

  if (pumpToggle == 0) {
    Serial.print("Next Pump Cycle in: ");
    Serial.print((pumpCycleStartTime - currentMillis) / 3600000);
    Serial.println(" Hours");

    // ****** Morse Beacon Begins ******
    int morseI = 57; // set integer variable to be read out equal to morseI here
    char message[21]; // enough to hold all numbers up to 64-bits
    itoa(moistureSensor, message, 10);
    Serial.println("Sending Morse");

    sendmsg("M ");
    sendmsg(message) ;
    delay(1000);
    sendmsg("H ");
    sendmsg(heatSensor);

    Serial.println("");
    delay(2000) ;
    // ****** Morse Beacon Ends ******

  }
  Serial.println("*******************************");
  Serial.print("Minutes since restart: ");
  Serial.println(currentMillis / 60000);
  Serial.print("Days since restart: ");
  Serial.println(currentMillis / 86400000);
  Serial.print("Moisture Sensor: ");
  Serial.println(moistureSensor);
  Serial.print("Heat Sensor: ");
  Serial.println(heatSensor);
}
