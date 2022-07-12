# TEMT6000 (Breakout Board)

TEMT6000 is an ambient **ANALOG**  light sensor. It is sensitive to visible light much like the human eye and has peak sensitivity at 570 nm.

It help you to to detect the light density and reflect the analog voltage signal back to ESP/Arduino controller. You can set the threshold of voltage level to trigger other units on your ESP32 project.

## SPECIFICATION

| TEMT6000       |   Specs     |
| ------------- |:-------------|
| Supply Voltage|DC 3.3 ~ 5.5V|
| llumination Range|1 – 1000 Lux|
| Output |Analog voltage -  **@VCC=5V** 0 ~ 5V  or **@VCC=3.3V** 0 ~ 3.3V |
| Datasheet|[TEMT6000 - VISHAY](https://www.vishay.com/docs/81579/temt6000.pdf)|
| Supply Voltage|DC 3.3 ~ 5.5V|
| Sensibility | +/-60 degrees|

## Sensor Photos
<img src="https://github.com/CraftzAdmin/esp32/blob/96055247139d4a98f6111b92b2918d85521470b6/Sensors/photos/TEMT600_FRONT.jpg" width="300" height="300"> <img src="https://github.com/CraftzAdmin/esp32/blob/96055247139d4a98f6111b92b2918d85521470b6/Sensors/photos/TEMT6000_back.jpg" width="300" height="300">

## PINS - ESP32 DEVKIT-V1 (30 PIN)

Since the TEMT6000 is an **ANALOG** sensor, select one of the **ADC1** pins on ESP32 DEV Boards. **ADC2** can also be used if your project is not using WIFI. If you´re using more sensors in your project, please consider supplying an external 3.3V line directly. 

ESP32 DEVKIT V1 ADC1 Pins -  `GPIO32` `GPIO33` `GPIO34` `GPIO35` `GPIO36` `GPIO39` 

| TEMT6000      |    ESP32 DEVKIT V1    | Obs |
| ------------- |:-------------|:-------------|
| S (OUT) |`GPIO34`|**@VCC=3.3V** 0 ~ 3.3V MAX |
| V (VCC) | `3.3V` | 3.3V - If possible use external supply |
| G (GND) | `GND`| Common GND |


