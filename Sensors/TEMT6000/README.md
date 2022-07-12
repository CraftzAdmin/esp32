# TEMT6000 (Breakout Board)

TEMT6000 is an ambient **ANALOG**  light sensor. It is sensitive to visible light much like the human eye and has peak sensitivity at 570 nm. The breakout board contains a TEMT6000 in the form of a phototransistor in a small SMD housing with a wide sensitivity angle. The output voltage increases with increasing intensity of the incident light. 

The operation for this sensor is very similar to a NPN transistor. The 10kOhm shown on the schematics is already present on the breakout board.

It detects the light density and reflect the analog voltage signal back to ESP/Arduino controller. You can set the threshold of voltage level to trigger other units on your ESP32 project.

## Specs

| TEMT6000       |   Specs     |
| ------------- |:-------------|
| Supply Voltage|DC 3.3 ~ 5.5V|
| llumination Range|1 – 1000 Lux|
| Output |Analog voltage -  **@VCC=5V** 0 ~ 5V  or **@VCC=3.3V** 0 ~ 3.3V |
| Datasheet|[TEMT6000 - VISHAY](https://www.vishay.com/docs/81579/temt6000.pdf)|
| Supply Voltage|DC 3.3V ~ 5.5V|
| Half sensitivity wide angle ϕ | +/-60° degrees|
|Current consumption| 20 mA|
| Dimensions | 30 x 22 mm |

## Considerations

This sensor is more adequate for indoors / shades, due to it´s limited range (1 – 1000 Lux). This means that the sensor quickly saturates under a bright light source (ex. Sun - 120,000 lux). This means that any direct high source will quickly achieve max readings. If you plan on using this on a weather station, please consider **BH1750** (1 - 65535 Lux).

## Sensor Photos
<img src="https://github.com/CraftzAdmin/esp32/blob/96055247139d4a98f6111b92b2918d85521470b6/Sensors/photos/TEMT600_FRONT.jpg" width="300" height="300"> <img src="https://github.com/CraftzAdmin/esp32/blob/96055247139d4a98f6111b92b2918d85521470b6/Sensors/photos/TEMT6000_back.jpg" width="300" height="300">
<img src="https://github.com/CraftzAdmin/esp32/blob/2be047704930a3eb39e4b08d5c9615c42d0e86c2/Sensors/photos/TEMT6000_schematic.jpg" width="200" height="300">


## PINS - ESP32 DEVKIT-V1 (30 PIN)

Since the TEMT6000 is an **ANALOG** sensor, select one of the **ADC1** pins on ESP32 DEV Boards. **ADC2** can also be used if your project is not using WIFI. If you´re using more sensors in your project, please consider supplying an external 3.3V line directly. 

ESP32 DEVKIT V1 ADC1 Pins -  `GPIO32` `GPIO33` `GPIO34` `GPIO35` `GPIO36` `GPIO39` 

| TEMT6000      |    ESP32 DEVKIT V1    | Obs |
| ------------- |:-------------|:-------------|
| S (OUT) |`GPIO34`|**@VCC=3.3V** 0 ~ 3.3V MAX |
| V (VCC) | `3.3V` | 3.3V - If possible use external supply |
| G (GND) | `GND`| Common GND |

## Schematics
<center><img src="https://github.com/CraftzAdmin/esp32/blob/87ade5df7aac9b75e9ee8d0871e21fb86b9d571e/Sensors/photos/temt_fritzing.png" width="300" height="300"></center>

## Required Libraries

No required libraries.

## Basic Code (ARDUINO IDE)

```C++ 

/*
 * 
                  __ _       
                 / _| |      
   ___ _ __ __ _| |_| |_ ____
  / __| '__/ _` |  _| __|_  /
 | (__| | | (_| | | | |_ / / 
  \___|_|  \__,_|_|  \__/___|
   TEMT6000 Test - ESP32
   Code: Daniel Brunod
   https://www.craftz.com.br

Copyright 2022 Daniel Brunod

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
//PIN DEFINITION  - ESP32
#define TEMT6000 34 //PIN on TEMT6000

void setup()
{
  // PIN MODE
  pinMode(TEMT6000, INPUT);
  
  Serial.begin(9600);
}

void loop() {

  // Light Reading - TEMT6000
    analogReadResolution(10);
    
    float volts =  analogRead(TEMT6000) * 5 / 1024.0; // Convert reading to VOLTS
    float VoltPercent = analogRead(TEMT6000) / 1024.0 * 100; //Reading to Percent of Voltage
    
    //Conversions from reading to LUX
    float amps = volts / 10000.0;  // em 10,000 Ohms
    float microamps = amps * 1000000; // Convert to Microamps
    float lux = microamps * 2.0; // Convert to Lux */
    delay(1000);

  // Output Serial

  // Output Serial
  Serial.print("LUX - ");
  Serial.print(lux);
  Serial.println(" lx");
  Serial.print(VoltPercent);
  Serial.println("%");
  Serial.print(volts);
  Serial.println(" volts");
  Serial.print(amps);
  Serial.println(" amps");
  Serial.print(microamps);
  Serial.println(" microamps");
  delay(1000);
}

```

 ## Code Explanation
 
 1. we first initiate the program by pointing the ***SIG*** Analog input to the board.
 
 ```C++
//PIN DEFINITION  - ESP32
#define TEMT6000 34 //PIN on TEMT6000
 ```
2. Then on Setup() we define the pinmode to INPUT, in order to receive the data. Since the breakout board already has all the resistors needed, we don need to make input pullup. Also we define the connection to the serial port for debugging and reading sensor values.

```C++
void setup()
{
  // PIN MODE
  pinMode(TEMT6000, INPUT);
  
  Serial.begin(9600);
} 
```

3. ESP32 by default has a 12bit resolution on ADC (readings from 0-4095). For this sensor we set the resolution to 10bit (0-1024) to get the correct readings. All inside Loop ().

```C++
void loop() {
 // Light Reading - TEMT6000
    analogReadResolution(10);
```
4. Sensor will give us a reading in voltage, so we will need to convert it to lux. First step s to determine V. We will do that by multiplying the reading by 5 (equivalent to 5V) and dividing by 1024 (max reading if output was 5V). The Volt percent is just anonther variable, not required for the calculation process.

```C++
 float volts =  analogRead(TEMT6000) * 5 / 1024.0; // Convert reading to VOLTS
    float VoltPercent = analogRead(TEMT6000) / 1024.0 * 100; //Reading to Percent of Voltage
 ```
 
5. Finally we will convert the readings to LUX. In the datasheet you will find the relationship between lux and current. From that chart you can calculate the formula. Then you just need to convert the voltage measured to a current measurement. We will use as in the datasheet a series resistor is 10,000 Ohms so the current should be voltage / 10000. The formula is y = 1/2(x) + 0.

    This means that lux = 2 * microamps. In the end we will delay 1000 (1 second) to give enought time for the reading. This should give you a value between **0 and 1000 lux.**

```C++
 //Conversions from reading to LUX
    float amps = volts / 10000.0;  // em 10,000 Ohms (Convert to Current)
    float microamps = amps * 1000000; // Convert to Microamps
    float lux = microamps * 2.0; // Convert to Lux */
    delay(1000);
 ```
 6. All info will be output to serial monitor and add a delay for reading.
 
 ```C++
 // Output Serial
  Serial.print("LUX - ");
  Serial.print(lux);
  Serial.println(" lx");
  Serial.print(VoltPercent);
  Serial.println("%");
  Serial.print(volts);
  Serial.println(" volts");
  Serial.print(amps);
  Serial.println(" amps");
  Serial.print(microamps);
  Serial.println(" microamps");
  delay(1000);
}
 ```
 ## Fritzing Files
 [TEMT6000 - Fritzing Download](https://github.com/CraftzAdmin/esp32/blob/6651d76bbaf319e584c2fedc8f2056fe58d56fa9/Sensors/TEMT6000/TEMT6000_Breakout_Board.fzpz)
 Courtesy of http://omnigatherum.ca/wp/?tag=fritzing
 
 
