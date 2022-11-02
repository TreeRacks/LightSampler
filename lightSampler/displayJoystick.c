#include "displayJoystick.h"
#include "joystick.h"
#include "LEDMatrix.h"
#include "sampling.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>

pthread_t thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static bool stopDisplay = false;

static void* displayJoystickValues(void* arg){
    while(!stopDisplay){

        clearDisplay();
        if(JoystickLimit < joyStickCalculationY()){
            displayDec(getMaxValue());
        } else if(-JoystickLimit > joyStickCalculationY()){
            displayDec(getMinValue()); 
        } else if(JoystickLimit < joyStickCalculationX()){
            displayDec(getMaxInterval() / 1000000.0);
        } else if(-JoystickLimit > joyStickCalculationX()){
            displayDec(getMinInterval() / 1000000.0);
        } else{
            displayInt(getNumberOfDips());
        }
        sleepForMs(200);
    }
}

void clearDisplay(){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    for(int i = 0; i < 16; i+=2){
        writeI2cReg(i2cFileDesc, i, 0x00);
    }
}

void displayJoystick_startDisplay(){
    pthread_create(&thread, NULL, displayJoystickValues, NULL);
}

void displayJoystick_stopDisplay(){
    stopDisplay = true;
    pthread_join(thread, NULL);
}