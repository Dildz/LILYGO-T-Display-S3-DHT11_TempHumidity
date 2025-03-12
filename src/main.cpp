/*********************************************************************************************************
 * LILYGO T-Display-S3 DHT11 Temperature and Humidity Sensor Project
 *
 * Description:
 *   This code reads temperature and humidity data from a DHT11 sensor and displays it on the built-in
 *    screen of the LilyGO T-Display-S3 using the TFT_eSPI library.
 *   The code uses state machine logic to avoid using delays, which is code blocking.
 *   If the sensor readings are 0 or invalid, it displays "Sensor Not Connected" on the screen.
 *   The screen is only updated if there is a change in the sensor readings.
 *
 * How It Works:
 *   1. Sensor Reading: The code reads temperature and humidity data from the DHT11 sensor as float vaues
 *    at regular 2 second intervals.
 *   2. Display: The sensor data is updated on the screen only when there is a difference in readings.
 *   3. State Machine: A state machine is used to manage the timing of sensor readings and display updates.
 *   4. Sensor Check: If the sensor readings are 0 or invalid, it displays "Sensor Not Connected".
 *
 * Pin Connections:
 *   - DHT11 Data Pin -> GPIO1
 *   - LCD Backlight  -> GPIO15
 *   - Ground         -> GND
 *   - Voltage        -> 5V
 *
 * Notes:
 *   - The DHT library is used to read data from the DHT11 sensor.
 *   - The TFT_eSPI library is configured to work with the LilyGO T-Display-S3, providing an easy way to
 *      display information on the built-in screen.
 *   - DHT11 pinout: [-] = GND | [S] = Signal PIN | [MIDDLE PIN] = Supply Voltage PIN.
 *      (5V or 3.3V pins on the T-Display can be used to power the DHT11 but the resolution will be lower
 *       if the 3.3V pin is used.)
 *   - DHT11 uses float as the data-type, rounded to the 2nd decimal poition (00.00)
 * 
 * DHT11 Specifications:
 *   - Operating Voltage: 3V to 5V
 *   - Operating current: 0.3mA (measuring) 60uA (standby)
 *   - Output: Serial data
 *   - Temperature Range: 0°C to 50°C
 *   - Humidity Range: 20% to 90%
 *   - Resolution: Temperature and Humidity both are 16-bit
 *   - Accuracy: ±1°C and ±1%
**********************************************************************************************************/


/*************************************************************
******************* INCLUDES & DEFINITIONS *******************
**************************************************************/

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <DHT.h>

// TFT_eSPI
TFT_eSPI tft = TFT_eSPI();

// DHT11 Sensor
#define DHT11_PIN 1
DHT dht11(DHT11_PIN, DHT11);

// State Machine States
enum class State {
  READ_SENSOR,    // state for reading sensor data
  UPDATE_DISPLAY, // state for updating the display
  WAIT            // state for waiting between sensor reads
};

// Global variables
State currentState = State::READ_SENSOR;       // initial state
unsigned long previousMillis = 0;              // for non-blocking timing
const unsigned long sensorReadInterval = 2000; // read sensor every 2 seconds (recommended interval)
float temperature = 0.0;                       // variable to store temperature reading
float humidity = 0.0;                          // variable to store humidity reading
float previousTemperature = 0.0;               // store previous temperature value
float previousHumidity = 0.0;                  // store previous humidity value
bool redrawRequired = true;                    // flag to indicate when to redraw the screen
bool sensorConnected = true;                   // flag to track sensor connection


/*************************************************************
********************** HELPER FUNCTIONS **********************
**************************************************************/

// Function to draw static elements on the TFT screen
void drawStaticElements() {
  tft.fillScreen(TFT_BLACK);              // clear the screen
  tft.setTextFont(2);                     // set the font
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color and background

  // Draw static text or elements
  tft.setCursor(0, 0);
  tft.println("---------------------------");
  tft.println("- DHT11 Sensor Module -");
  tft.println("---------------------------");

  tft.setCursor(0, 70);
  tft.println("Status:");

  tft.setCursor(0, 120);
  tft.println("Temperature:");

  tft.setCursor(0, 170);
  tft.println("Humidity:");
}

// Function to update dynamic elements on the TFT screen
void updateDynamicElements() {
  // Update sensor status
  tft.setCursor(0, 90);  // move cursor to the line below "Status:"
  tft.print("        "); // clear previous value
  tft.setCursor(0, 90);
  tft.print(sensorConnected ? "CONNECTED" : "DISCONNECTED");

  // Update temperature
  tft.setCursor(0, 140); // move cursor to the line below "Temperature:"
  tft.print("        "); // clear previous value
  tft.setCursor(0, 140);
  if (sensorConnected) {
    tft.print(String(temperature) + " C");
  } else {
    tft.print("N/A");
  }

  // Update humidity
  tft.setCursor(0, 190); // move cursor to the line below "Humidity:"
  tft.print("        "); // clear previous value
  tft.setCursor(0, 190);
  if (sensorConnected) {
    tft.print(String(humidity) + " %");
  } else {
    tft.print("N/A");
  }
}


/*************************************************************
*********************** MAIN FUNCTIONS ***********************
**************************************************************/

// SETUP
void setup() {
  // Initialize the TFT display
  tft.init();
  tft.setRotation(0);                     // adjust rotation (0 & 2 portrait | 1 & 3 landscape)
  tft.fillScreen(TFT_BLACK);              // black background
  tft.setTextFont(2);                     // set the font (you can experiment with different fonts)
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text colour (white) and background colour (black)

  // Initialize the DHT11 sensor
  dht11.begin();

  // Draw static elements once
  drawStaticElements();

  // Initial display update
  redrawRequired = true; // update display
}

// MAIN LOOP
void loop() {
  unsigned long currentMillis = millis(); // get the current millis time

  // State Machine Logic
  switch (currentState) {
    case State::READ_SENSOR:
      // Read sensor data
      temperature = dht11.readTemperature();
      humidity = dht11.readHumidity();

      // Check if readings are valid
      if (isnan(temperature) || isnan(humidity)) {
        sensorConnected = false; // sensor not connected or malfunctioning
      }
      else {
        sensorConnected = true;  // sensor is connected and working
      }

      // Check if the readings have changed
      if (temperature != previousTemperature || humidity != previousHumidity || !sensorConnected) {
        redrawRequired = true;             // update display
        previousTemperature = temperature; // update previous temperature
        previousHumidity = humidity;       // update previous humidity
      }

      // Move to the next state
      currentState = State::UPDATE_DISPLAY;
      break;

    case State::UPDATE_DISPLAY:
      // Update the display with the new sensor data
      if (redrawRequired) {
        updateDynamicElements();
        redrawRequired = false; // reset the flag
      }

      // Move to the WAIT state
      currentState = State::WAIT;
      previousMillis = currentMillis; // record the previous millis time
      break;

    case State::WAIT:
      // Wait for the specified interval before reading the sensor again
      if (currentMillis - previousMillis >= sensorReadInterval) {
        currentState = State::READ_SENSOR;
      }
      break;

    default:
      // Default case (should not happen)
      currentState = State::READ_SENSOR;
      break;
  }
}
