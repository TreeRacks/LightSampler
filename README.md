# Light Sampler

##### Light Sampling mechanism using a BeagleBone Green 

Connected with an Adafruit 8x8 LED Matrix and a GPIO Adafruit two-axis joystick on a half breadboard. 

### Hardware

* Operates successfully on a Debian 11/Bullseye image (September 2022).

* For pin setup, refer to the BBG pin map at https://wiki.seeedstudio.com/BeagleBone_Green_Wireless/ for all relevant GPIO pins.

* Please note that the Adafruit photocell CdS photoresistor should be connected in series with two 10k resistors in a voltage divider configuration in order to comply with the BBG voltage that will be supplied to the breadboard.

* Configure pins on the BBG for I2C and GPIO by running the following commands on the target:
```
config-pin i2c PX_XX
```
and 
```
config-pin gpio PX_XX
```
respectively, where X_XX is the relevant pin (eg. P9_17).

NOTE: i2c-tools should be installed on the target in order to enable the hardware I2C bus(es).

### Implementation

* The program is entirely written in C.  
* This implementation uses `makefile` to generate an executable binary which is then cross-compiled onto the target from the host and executed on the BBG.

