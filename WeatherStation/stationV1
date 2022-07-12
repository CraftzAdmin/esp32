/*
 * 
                  __ _       
                 / _| |      
   ___ _ __ __ _| |_| |_ ____
  / __| '__/ _` |  _| __|_  /
 | (__| | | (_| | | | |_ / / 
  \___|_|  \__,_|_|  \__/___|
   Weather Station V1.0 - ESP32
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

#include <Wire.h> // Biblioteca para sensores que usam SDA / SCL
#include "Adafruit_BMP085.h" //Biblioteca do Sensor GY-65(BMP085) - (Barômetro / temperatura / Altitude)
#include <DHT.h> //Biblioteca Sensor DHT
#include <WiFi.h> //Biblioteca Wifi
#include "HTTPClient.h" // Para enviar e receber requests
#include <ESP32Time.h> // Sets RTC/NTPTime
#include <BH1750.h> //Sensor de Luz https://github.com/claws/BH1750/blob/master/examples/BH1750autoadjust/BH1750autoadjust.ino

//Definição de Pinos ESP32
#define sensorLuz 36 //Sensor de Luz Ambiente
#define MqDigitalPin 26 //Sensor MQ135
#define MqAnalogPin 33 //Sensor MQ135 
#define DHTPIN 4 //PINO DIGITAL UTILIZADO PELO DHT22
#define UVOUTPIN 35 //PINO ANALOGICO UTILIZADO PELO SensorUV
#define UVREF 32 //PINO DIGITAL UTILIZADO PELO SensorUV 
#define RAIN 34 //PINO analogico UTILIZADO PELO SensorChuva 
#define RAINDIG 27  //PINO Digital UTILIZADO PELO SensorChuva
#define OPTO 23  //PINO Digital UTILIZADO pelo 4N25
// pino 39 disponivel

//ESP32 DeepSleep
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  240        /* Time ESP32 will go to sleep (in seconds)  - 5 minutes*/
RTC_DATA_ATTR int bootCount = 0; /* Variável para controlar o número de reboots  */

ESP32Time rtc;
//ESP32Time rtc(-10800);  // offset in seconds GMT-3
//Variáveis NTPServer
const char* ntpServer = "br.pool.ntp.org";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 0;

// WiFi credentials
const char* ssid = "SSID";         // change SSID
const char* password = "PASSWORD";    // change password

// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "GscriptID";    // change Gscript ID

//Variáveis de Leitura
#define DHTTYPE DHT22  //DEFINE O MODELO DO SENSOR (DHT22 / AM2302)

//Inicializa Sensor GY-65 (BMP085)
Adafruit_BMP085 bmp;

// Parâmetros do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

//Motivo de retorno deepsleep
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

//Takes an average of readings on a given pin
int averageAnalogRead(int pinToRead)
{
  analogReadResolution(10);
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;

  for (int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return (runningValue);
}

//float function
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Sensor de Luz BH1750
BH1750 lightMeter;

void setup()
{
  // Definição dos Pinos
  pinMode(sensorLuz, INPUT);
  pinMode(MqDigitalPin, INPUT);
  pinMode(MqAnalogPin, INPUT);
  pinMode(UVOUTPIN, INPUT);
  pinMode(UVREF, INPUT);
  pinMode(RAIN, INPUT);
  pinMode(RAINDIG, INPUT);
  pinMode(OPTO, OUTPUT);

  Serial.begin(9600);
  Wire.begin();
  print_wakeup_reason();

  //Sensor de Luz BH1750
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  } else {
    Serial.println(F("Error initialising BH1750"));
  }

  //Sensor DHT
  dht.begin();


  // Sensor GY-65
  if (!bmp.begin()) {
    Serial.println("Não foi encontrado um sensor BMP085!");
    while (true) {}
  }

  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Initialize a NTPClient to get time
  if (bootCount == 0) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      Serial.println("Trying Again!");
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      struct tm timeinfo;
    }
    if (getLocalTime(&timeinfo)) {
      rtc.setTimeStruct(timeinfo);
      Serial.println(rtc.getTime("%F-%T-%Z"));
    }
  }
}

void loop() {

  // Send Signal - OptoPin

  digitalWrite(OPTO, HIGH); //Sets Opto 4N25 HIGH
  delay(15000); // Delay para permitir acionamento dos sensores quando retorna do deepsleep

  // Leitura do Sensor GY-65(BMP085)
  float temperaturaCelsius = bmp.readTemperature(); //Leitura Celsius (30 °C × 9/5) + 32 = 86 °F
  float temperaturaFarenheit = (bmp.readTemperature() * 1.8) + 32; //Leitura Farenheit (30 °C × 9/5) + 32 = 86 °F
  float pressaoPascal = bmp.readPressure(); //Leitura pressão Pascal
  double pressaoAtm = pressaoPascal / 101325; //Leitura pressão ATM
  double pressaoBar = pressaoPascal / 100000; //Leitura pressão Bar
  float pressaoPsi = pressaoPascal / 6895; //Leitura pressão Bar
  float altitudeMetros = bmp.readAltitude(); //Leitura Altitude Metros
  float altitudePes = bmp.readAltitude() * 3.281; //Leitura Altitude Pés

  //Leitura Sensor DHT
  delay(2000); //INTERVALO DE 2 SEGUNDO ANTES DE INICIAR
  float umidade = dht.readHumidity(); //realiza a leitura Umidade do DHT
  float temperatura2Celsius = dht.readTemperature(); //realiza a leitura temperatura DHT - Celisus
  float temperatura2Farenheit = dht.readTemperature(true); // Read temperature as Fahrenheit (isFahrenheit = true)
  float hif = dht.computeHeatIndex(temperatura2Farenheit, umidade); // Heat index em Farenheit
  float hic = dht.computeHeatIndex(temperatura2Celsius, umidade, false); // Heat index em Celsius

  // Leitura do Sensor MQ135 (Gás)
  // Cálcula offset Atmosférico e mapeia os valores para monitoramento
  analogReadResolution(10);
  int co2Zero = 60; //Calibra o offset do C02 atmosférico - valor de Referência
  int MQsensorValue =  averageAnalogRead(MqAnalogPin) - co2Zero; //Leitura Analogica MQ135 com compensação
  int MQdigitalValue = digitalRead(MQdigitalValue); //Leitura Digital MQ135
  int co2ppm = map(MQsensorValue, 0, 1023, 0, 5000);
  delay(2000);

  //Leitura Sensor UV
  analogReadResolution(10);
  int uvLevel = averageAnalogRead(UVOUTPIN);
  int refLevel = averageAnalogRead(UVREF);
  delay(1000);
  float outputVoltage = 3.3 * uvLevel / refLevel;
  float uvIntensity = mapfloat(uvLevel, 310, 1023, 1.0, 15.0);  // 310 = 1V, 1023 = 3.3V
  delay(2000);

  /* Leitura do Sensor TEMT6000
    analogReadResolution(10);
    float volts =  analogRead(sensorLuz) * 5 / 1024.0; // Converte Leitura ADC para LUX
    float luzPercent = analogRead(sensorLuz) / 1024.0 * 100; //Realiza a Leitura da Luz em percentual
    float amps = volts / 10000.0;  // em 10,000 Ohms
    float microamps = amps * 1000000; // Converte em Microamps
    float lux = microamps * 2.0; // Converte em Lux */

  //Leitura do Sensor BH1750
  float lux = lightMeter.readLightLevel();
  float luzPercent = lux / 4095.0 * 100; //Realiza a Leitura em percentual

  if (lux < 0) {
    Serial.println(F("Error condition detected"));
  } else {
    if (lux > 40000.0) {
      // reduce measurement time - needed in direct sun light
      if (lightMeter.setMTreg(32)) {
        Serial.println(
          F("Setting MTReg to low value for high light environment"));
        float lux = lightMeter.readLightLevel();
        float luzPercent = lux / 4095.0 * 100; //Realiza a Leitura em percentual
      } else {
        Serial.println(
          F("Error setting MTReg to low value for high light environment"));
      }
    } else {
      if (lux > 10.0) {
        // typical light environment
        if (lightMeter.setMTreg(69)) {
          Serial.println(F(
                           "Setting MTReg to default value for normal light environment"));
          float lux = lightMeter.readLightLevel();
          float luzPercent = lux / 4095.0 * 100; //Realiza a Leitura em percentual
        } else {
          Serial.println(F("Error setting MTReg to default value for normal "
                           "light environment"));
        }
      } else {
        if (lux <= 10.0) {
          // very low light environment
          if (lightMeter.setMTreg(138)) {
            Serial.println(
              F("Setting MTReg to high value for low light environment"));
            float lux = lightMeter.readLightLevel();
            float luzPercent = lux / 4095.0 * 100; //Realiza a Leitura em percentual
          } else {
            Serial.println(F("Error setting MTReg to high value for low "
                             "light environment"));
          }
        }
      }
    }
  }

  delay(1000);

  // Leitura do Sensor Chuva
  analogReadResolution(10);
  int chuva = digitalRead(RAINDIG); // Realiza a leitura do pino Digital - 1=Sem Chuva / 0 = Chuva
  int ChuvaInt = analogRead(RAIN); // Realiza a leitura do pino Analogico medição intensidade

  delay(2000);

  //Envia Dados Para o Google Sheets
  String formattedDate(rtc.getTime("%F-%T-%Z")); // Date String
  String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "lux=" + String(lux) +
                    "&date=" + formattedDate +
                    "&luxpercent=" + String(luzPercent) +
                    "&temp_celsius=" + String(temperaturaCelsius) +
                    "&temp_farenheit=" + String(temperaturaFarenheit) +
                    "&pressao_pascal=" + String(pressaoPascal) +
                    "&pressao_atm=" + String(pressaoAtm) +
                    "&pressao_bar=" + String(pressaoBar) +
                    "&pressao_psi=" + String(pressaoPsi) +
                    "&altitude_m=" + String(altitudeMetros) +
                    "&altitude_ft=" + String(altitudePes) +
                    "&co2ppm=" + String(co2ppm) +
                    "&gas=" + String(MQdigitalValue) +
                    "&umidade=" + String(umidade) +
                    "&heat_index_f=" + String(hif) +
                    "&heat_index_c=" + String(hic) +
                    "&uv_intensity=" + String(uvIntensity) +
                    "&chuvaint=" + String(ChuvaInt) +
                    "&chuva=" + String(chuva);

  //Serial.print("POST data to spreadsheet:");
  //Serial.println(urlFinal);
  HTTPClient http;
  http.begin(urlFinal.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int httpCode = http.GET();
  Serial.print("HTTP Status Code: ");
  Serial.println(httpCode);
  //---------------------------------------------------------------------
  //getting response from google sheet
  String payload;
  if (httpCode > 0) {
    payload = http.getString();
    // Serial.println("Payload: "+payload);
  }
  //---------------------------------------------------------------------
  http.end();

  delay(1000);


  // Outputs Saída Serial (Para leitura e debug)

  Serial.print("LUX - ");
  Serial.print(lux);
  Serial.println(" lx");
  Serial.print(luzPercent);
  Serial.println("%");

  Serial.print("Temperatura: ");
  Serial.print(temperaturaCelsius);
  Serial.println(" °C");

  Serial.print("Temperatura: ");
  Serial.print(temperaturaFarenheit);
  Serial.println(" °F");

  Serial.print("Pressão: ");
  Serial.print(pressaoPascal);
  Serial.println(" Pa");

  Serial.print("Pressão: ");
  Serial.print(pressaoAtm , 2);
  Serial.println(" Atm");

  Serial.print("Pressão: ");
  Serial.print(pressaoBar);
  Serial.println(" Bar");

  Serial.print("Pressão: ");
  Serial.print(pressaoPsi);
  Serial.println(" Psi");

  Serial.print("Altitude: ");
  Serial.print(altitudeMetros);
  Serial.println(" metros");

  Serial.print("Altitude: ");
  Serial.print(altitudePes);
  Serial.println(" ft");

  Serial.print("PPM: ");
  Serial.println(co2ppm);
  Serial.print("GAS: ");
  Serial.println(MQdigitalValue);

  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println("%");

  Serial.print("Heat index: ");
  Serial.print(hif);
  Serial.print("°F - ");
  Serial.print(hic);
  Serial.println("°C");

  Serial.print("UV Intensity (mW/cm^2): ");
  Serial.print(uvIntensity, 4);
  Serial.print(" - REFLevel: ");
  Serial.print(refLevel);
  Serial.print(" - ML8511 voltage ");
  Serial.print(outputVoltage);
  Serial.print(" - ML8511 output: ");
  Serial.println(uvLevel);

  Serial.print("Chuva: ");
  Serial.println(chuva);
  Serial.print("Intensidade: ");
  Serial.println(ChuvaInt);

  Serial.println("Boot number: " + String(bootCount));
  Serial.println();

  // ESP32 Deepsleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");
  //Incrementa o bootcount, importante para o RTC
  if (bootCount > 12) {
    bootCount = 0;
  }
  else {
    ++bootCount;
  }

  esp_deep_sleep_start();
}
