Arduino Controller for Flood and Drain System

This system automates watering of a flood and drain growing system.

This code is under development and will likely change soon.
Some features not yet tested!

Features include:

Pump can be started up to three times per crop
Pump controlled by capacitive moisture sensor with a timeout limit
Heat Mat Thermostat Control with a thermistor
Button switch to manually run pump one time
Exhaust fan controled by box air temperature
Air circulation fan runs when exhaust fan is off
Thermistor and moisture sensor values are flashed in morse code
Morse turned off to speed up cycle



//-----------------------------------------------------------

//       FLOOD AND DRAIN PUMP TIMER WITH

//           HEAT MAT THERMOSTAT AND

//      MORSE CODE FLASH OF SENSOR VALUES

//-----------------------------------------------------------

// Pump Timer with Sensor Control and Morse Readout

// Capacitive Soil Moisture Sensor with AOUT to pin 1

// LED on Pin 13 will flash sensor values in morse code

// M indicates moisture sensor value, H indicates tray thermistor value, A indicates box air thermistor value

// Pump Relay is on Pin 4
// Circulation fan relay is Pin 5
// Exhaust fan relay is Pin 6
// Heat relay is Pin 7

// Pump will only run if soil moisture sensor indicates dry soil

// Pump will turn off if soil becomes moist during pumping.

// Change pumpCycleOnDuration to set pump run time

// Ground Pin 12 with a button switch to manually run pump once 

// Tray thermistor is connected to 3.3V and Pin 0, with a 10k Ohm Resistor from Pin 0 to Ground

// Box air thermistor is connected to 3.3V and Pin 2, with a 10k Ohm Resistor from Pin 0 to Ground

// Confirm DRYSOILSET, WETSOILSET, HEATSET values and pump cycle durations prior to use 

// Paul VandenBosch, 20190220

Suggested Product List (January, 2019):

Arduino Uno Clone: https://www.aliexpress.com/item/1pcs-Smart-Electronics-high-quality-UNO-R3-MEGA328P-CH340G-for-arduino-Compatible-with-USB-CABLE/32803501325.html

Four Relay Shield for Arduino: https://www.aliexpress.com/item/4-Channel-5V-Relay-Swtich-Expansion-Drive-Board-LED-indicator-Relay-Shield-V2-0-Development-Board/32886386019.html

Capacitive Soil Moisture Sensor: https://www.aliexpress.com/item/Capacitive-Soil-Moisture-Sensor-Module-Not-Easy-to-Corrode-Wide-Voltage-Wire-3-3-5-5V/32892951369.html

Note: Capacitive Soil Moisture Sensor must be coated with varnish, epoxy or similar waterproof coating.

Thermistor on 2m cable: https://www.aliexpress.com/item/2m-200cm-Two-2-Meter-Waterproof-NTC-Thermistor-Accuracy-Temperature-Sensor-10K-1-3950-Wire-Cable/32904302533.html


