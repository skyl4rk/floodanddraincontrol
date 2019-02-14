// Pump Timer with Sensor Control and Morse Readout
// Capacitive Soil Moisture Sensor with AOUT to pin 1
// Pump will turn off if soil becomes moist during pumping.
// Paul VandenBosch, 20190212

// Thermistor to Green and White/Green

#define HEATSET 350     // Sensor value to turn on heater
#define DRYSOILSET 460  // Moisture sensor Value that triggers watering, soil is dry at this value
#define WETSOILSET 400  // Moisture sensor Value that shuts off water pump
#define AIRTEMPSET 350  // Sensor value to turn on fan

#define WATERPUMPTIME 60     // Seconds to run water pump for each watering event
#define FIRSTWATERING 5  // Day after start for the first watering event
#define SECONDWATERING 7  // Day after start for the first watering event
#define THIRDWATERING 15  // Day after start for the first watering event
#define OVERFLOWDAYS 15   // Days after which the crop will be completed

#define HEATRELAYPIN 4  // Pin used for heat relay  
#define PUMPRELAYPIN 7      // Pin used for pump relay
#define FANRELAYPIN 6  //  Pin used for fan relay

#define MOISTUREPIN 1       // Analog pin used for moisture sensor
#define BUTTONPIN 12    // Pin used to start manual pump by grounding it
#define TRAYSENSORPIN 0 // Pin used for heat sensor on tray at seed level
#define BOXSENSORPIN 2 // Pin used for heat sensor in box for air temperature

int moistureSensor = analogRead(MOISTUREPIN);
int heatSensor = analogRead(TRAYSENSORPIN);
int airTempSensor = analogRead(BOXSENSORPIN);

unsigned long dayMs = 86400000;
unsigned long hourMs = 3600000;
unsigned long minuteMs = 60000;

unsigned long waterPumpTime = WATERPUMPTIME * 1000.;
unsigned long firstWatering = FIRSTWATERING * dayMs;
unsigned long secondWatering = SECONDWATERING * dayMs; 
unsigned long thirdWatering = THIRDWATERING *dayMs;

unsigned long currentMillis;


// Simple Arduino Morse Beacon
// ****** Morse Beacon Begins ******

#define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))
#define SPEED  (15)
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
  pinMode(PUMPRELAYPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(TRAYSENSORPIN, INPUT);
  pinMode(BOXSENSORPIN, INPUT);
  pinMode(HEATRELAYPIN, OUTPUT);
  pinMode(FANRELAYPIN, OUTPUT);
  
  // ****** Morse Beacon Begins ******
  pinMode(morseLEDpin, OUTPUT) ;
  // ****** Morse Beacon Ends ******
}

void loop() {
  moistureSensor = analogRead(MOISTUREPIN);
  heatSensor = analogRead(TRAYSENSORPIN);
  airTempSensor = analogRead(BOXSENSORPIN);
  
  currentMillis = millis();

  if (currentMillis > firstWatering) {
    digitalWrite(PUMPRELAYPIN, HIGH);
  }
  if (currentMillis > firstWatering + (WATERPUMPTIME * 1000.)) {
    firstWatering = OVERFLOWDAYS * dayMs;
    digitalWrite(PUMPRELAYPIN, LOW);
  }

  if (currentMillis > secondWatering) {
    digitalWrite(PUMPRELAYPIN, HIGH);
  }
  if (currentMillis > secondWatering + (WATERPUMPTIME * 1000.)) {
    secondWatering = OVERFLOWDAYS * dayMs;
    digitalWrite(PUMPRELAYPIN, LOW);
  }
  
  if (currentMillis > thirdWatering) {
    digitalWrite(PUMPRELAYPIN, HIGH);
  }
  if (currentMillis > thirdWatering + (WATERPUMPTIME * 1000.)) {
    thirdWatering = OVERFLOWDAYS * dayMs;
    digitalWrite(PUMPRELAYPIN, LOW);
  }

  if (airTempSensor > AIRTEMPSET) {
    digitalWrite(FANRELAYPIN, HIGH);
  }
  else {
    digitalWrite(FANRELAYPIN, LOW);
  }

  if (heatSensor < HEATSET) {
    digitalWrite(HEATRELAYPIN, HIGH);
  }
  else {
    digitalWrite(HEATRELAYPIN, LOW);
  }

  if (digitalRead(BUTTONPIN) == LOW) {
    digitalWrite(HEATRELAYPIN, LOW);
    digitalWrite(PUMPRELAYPIN, HIGH);
    Serial.println("");
    Serial.print("Manual Engagement of Pump by Button for ");
    Serial.print(WATERPUMPTIME);
    Serial.println(" Seconds Run Time");
    delay(waterPumpTime);
    digitalWrite(PUMPRELAYPIN, LOW);
  }

  if (moistureSensor < WETSOILSET) {     // turn off pump if soil is wet
    digitalWrite(PUMPRELAYPIN, LOW);
  }


    // ****** Morse Beacon Begins ******
    //char message[] = "1234568"; // worked
    int morseI = 57; // set integer variable to be read out equal to morseI here
    char moistureMessage[21]; // enough to hold all numbers up to 64-bits
    char heat0Message[21]; // enough to hold all numbers up to 64-bits
    char airTempMessage[21]; // enough to hold all numbers up to 64-bits
    itoa(moistureSensor, moistureMessage, 10);
    itoa(heatSensor, heat0Message, 10);
    itoa(airTempSensor, airTempMessage, 10);
    Serial.println("Sending Morse");

    sendmsg("M ");
    sendmsg(moistureMessage) ;
    delay(1000);
    sendmsg("H ");
    sendmsg(heat0Message);
    sendmsg("A ");
    sendmsg(airTempMessage);
    Serial.println("");
    //sendmsg("K6HX/B CM87") ;// original
    delay(1000) ;
    // ****** Morse Beacon Ends ******


  
  Serial.println("*******************************");
  Serial.println("Arduino Growth Chamber Controller");
  Serial.print("Days since restart: ");
  Serial.println(currentMillis / 86400000);
  Serial.print("Days to First Watering: ");
  Serial.println((firstWatering - currentMillis) / dayMs);
  Serial.print("Hours to First Watering: ");
  Serial.println((firstWatering - currentMillis) / hourMs);
  Serial.print("Days to Second Watering: ");
  Serial.println((secondWatering - currentMillis) / dayMs);
  Serial.print("Hours to Second Watering: ");
  Serial.println((secondWatering - currentMillis) / hourMs);
  Serial.print("Moisture Sensor: ");
  Serial.println(moistureSensor);
  Serial.print("Heat Sensor: ");
  Serial.println(heatSensor);
  Serial.print("Air Temp Sensor: ");
  Serial.println(airTempSensor);
}
