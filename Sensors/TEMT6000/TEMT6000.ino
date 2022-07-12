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
