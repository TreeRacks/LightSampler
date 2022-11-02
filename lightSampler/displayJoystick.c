#include "displayJoystick.h"
#include "joystick.h"
#include "LEDMatrix.h"
#include "sampling.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>

static bool stopButtonPress = false;

void displayJoystickValues(){
    while(!stopButtonPress){
        clearDisplay();
        if(JoystickLimit < joyStickCalculationY()){
            displayDec(1.0);
        } else if(-JoystickLimit > joyStickCalculationY()){
            displayDec(9.0); 
        } else if(JoystickLimit < joyStickCalculationX()){
            displayDec(1.1);
        } else if(-JoystickLimit > joyStickCalculationX()){
            displayDec(9.9);
        } else{
            
        }
        sleepForMs(100);
    }
}

void clearDisplay(){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    for(int i = 0; i < 16; i+=2){
        writeI2cReg(i2cFileDesc, i, 0x00);
    }
}