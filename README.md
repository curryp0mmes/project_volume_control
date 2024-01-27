## Volume Control

My current plan is building a hardware volume controller based on a motorized slider. It is all based on the ESP32-C3 Mini dev board, paired witha tft screen and some additional inputs for page selection etc.

## Hardware

ESP32-C3 Mini data sheet https://ae01.alicdn.com/kf/S45a14239903a4132a32cc4ab52abfeadq.jpg
TFT Screen
> 1.3 inch
> Display ModeNormally black IPS
> Display FormatGraphic 240RGB*240 Dot-matrix
> Input DataSPI interface
> Drive ICST7789VW
> Dimensional outline27.78(W)*39.22 (H)*3.0+/-0.1(T)mm
> Resolution240RGB*240 Dots
> LCD Active area23.4 (W)*23.4 (H)
> Dot pitch0.0975(H) x 0.0975(V) mm
> Viewing DirectionAll View
> Operating Temperature-20～70°C

## Current Wiring

TFT - ESP

- GND - GND
- VCC - 3V3
- SCK - GPIO9 / SCL
- SDA - GPIO8 / SDA
- RES - GPIO4 / SCK
- DC  - GPIO2
- BLK - not connected
