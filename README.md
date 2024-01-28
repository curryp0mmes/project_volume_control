## Volume Control

My current plan is building a hardware volume controller based on a motorized slider. It is all based on the ESP32-C3 Mini dev board, paired witha tft screen and some additional inputs for page selection etc.

## Hardware

ESP32-C3 Mini [AliExpress](https://www.aliexpress.com/item/1005006334515931.html) [data sheet](https://ae01.alicdn.com/kf/S45a14239903a4132a32cc4ab52abfeadq.jpg) [pinout](https://europe1.discourse-cdn.com/arduino/optimized/4X/9/2/f/92f1e8e2fcc888726ff7838dad725dd94f43438d_2_690x301.jpeg)

Slider Poti Motorized [AliExpress](https://www.aliexpress.com/item/1005006223166484.html)

Motor Controller [AliExpress](https://www.aliexpress.com/item/33024774147.html)

TFT Screen [AliExpress](https://www.aliexpress.com/item/1005006368548507.html)
> 1.3 inch \
> Display ModeNormally black IPS \
> Display FormatGraphic 240RGB*240 Dot-matrix \
> Input DataSPI interface \
> Drive ICST7789VW \
> Dimensional outline27.78(W)*39.22 (H)*3.0+/-0.1(T)mm \
> Resolution240RGB*240 Dots \
> LCD Active area23.4 (W)*23.4 (H) \
> Dot pitch0.0975(H) x 0.0975(V) mm \
> Viewing DirectionAll View \
> Operating Temperature-20～70°C \

## Current Wiring

TFT - ESP

GND - GND \
VCC - 3V3 \
SCK - GPIO9 / SCL \
SDA - GPIO8 / SDA \
RES - GPIO4 / SCK \
DC  - GPIO2 \
BLK - not connected 

Piezo is connected to GND and GPIO0/ADC0 with a 100k resistor in parallel to prevent charge buildup 

Poti - ESP \
1 - GPIO21 \
3 - GND \
4 - GPIO1/ADC1

Motor Controller for Poti Motor \
5V \
GND \
IN1 - GPIO3/A1 \
IN2 - GPIO5/A3 



## Roadmap

- [x] WiFi Connectivity
- [x] Printing to TFT Screen
- [ ] Connecting to Spotify API
- [x] Reading Potis
- [x] Talking to Motor of Slider
- [ ] Proper PIDs
- [ ] UI
- [ ] Make it standalone with battery power
- [ ] Design and printing 3d case
- [ ] Full Construction