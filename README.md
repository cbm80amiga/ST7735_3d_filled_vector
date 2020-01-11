# ST7735 3D Filled Vector Graphics Example
Smooth 3D Filled Vector Graphics implementation for STM32 and ST7735 1.8" 128x160 using fast SPI 36Mbps interface

Achieved up to 80 fps

YouTube video:

https://youtu.be/5y28ipwQs-E

More ST7735 and STM32 videos:

https://www.youtube.com/watch?v=o3AqITHf0mo&list=PLxb1losWErZ6y6GombzvtwRZ2l7brPv1s


## Implemented features
- optimized rendering without local framebuffer, in STM32 case 1 to 32 lines buffer can be used
- pattern based background
- 3D starfield
- no floating point arithmetic
- no slow trigonometric functions
- rotations around X and Y axes
- simple outside screen culling
- rasterizer working for all convex polygons
- backface culling
- visible faces sorting by Z axis
- Tested with stm32duino and Arduino IDE 1.6.5

## Connections (header at the top):

|LCD pin|LCD pin name|STM32|
|--|--|--|
 |#01| LED| 3.3V|
 |#02| SCK |PA5/SCK|
 |#03| SCA |PA7/MOSI|
 |#04| A0/DC|PA1 or any digital
 |#05| RESET|PA0 or any digital|
 |#06| CS|PA2 or any digital|
 |#07| GND | GND|
 |#08| VCC | 3.3V|


If you find it useful and you want to buy me a coffee or a beer:

https://www.paypal.me/cbm80amiga
