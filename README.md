 LILYGO T-Display-S3 DHT11 Temperature and Humidity Sensor Project

 Description:
   This code reads temperature and humidity data from a DHT11 sensor and displays it on the built-in
   screen of the LilyGO T-Display-S3 using the TFT_eSPI library.
   
   The code uses state machine logic to avoid using delays, which is code blocking.
   
   If the sensor readings are 0 or invalid, it displays "Sensor Not Connected" on the screen.
   
   The screen is only updated if there is a change in the sensor readings.

 How It Works:
   1. Sensor Reading: The code reads temperature and humidity data from the DHT11 sensor as float vaues
    at regular 2 second intervals.
   2. Display: The sensor data is updated on the screen only when there is a difference in readings.
   3. State Machine: A state machine is used to manage the timing of sensor readings and display updates.
   4. Sensor Check: If the sensor readings are 0 or invalid, it displays "Sensor Not Connected".

 Pin Connections:
   - DHT11 Data Pin -> GPIO2 
   - LCD Backlight  -> GPIO15
   - Ground         -> GND
   - Voltage        -> 5V

 Notes:
   - The DHT library is used to read data from the DHT11 sensor.
   - The TFT_eSPI library is configured to work with the LilyGO T-Display-S3, providing an easy way to
      display information on the built-in screen.
   - DHT11 pinout: [-] = GND | [S] = Signal PIN | [MIDDLE PIN] = Supply Voltage PIN.
      (5V or 3.5V pins on the T-Display can be used to power the DHT11 but the resolution will be lower
       if the 3.5V pin is used.)
   - DHT11 uses float as the data-type, rounded to the 2nd decimal poition (00.00)
 
 DHT11 Specifications:
   - Operating Voltage: 3.5V to 5.5V
   - Operating current: 0.3mA (measuring) 60uA (standby)
   - Output: Serial data
   - Temperature Range: 0°C to 50°C
   - Humidity Range: 20% to 90%
   - Resolution: Temperature and Humidity both are 16-bit
   - Accuracy: ±1°C and ±1%
