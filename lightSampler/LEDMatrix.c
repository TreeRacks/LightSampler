#include "LEDMatrix.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>

#define I2C_DEVICE_ADDRESS 0x70
#define SYS_SETUP_REG 0X21
#define DISPLAY_SETUP_REG 0x81

// Assume pins already configured for I2C:
// (bbg)$ config-pin P9_18 i2c
// (bbg)$ config-pin P9_17 i2c
int initI2cBus(char* bus, int address)
{
  int i2cFileDesc = open(bus, O_RDWR);
  int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
  if (result < 0) {
    perror("I2C: Unable to set I2C device to slave address.");
    exit(1);
  }
  return i2cFileDesc;
}

void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
  unsigned char buff[2];
  buff[0] = regAddr;
  buff[1] = value;
  int res = write(i2cFileDesc, buff, 2);
  if (res != 2) {
    perror("I2C: Unable to write i2c register.");
    exit(1);
  }
}

unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
  // To read a register, must first write the address
  int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
  if (res != sizeof(regAddr)) {
    perror("I2C: Unable to write to i2c register.");
    exit(1);
  }
  // Now read the value and return it
  char value = 0;
  res = read(i2cFileDesc, &value, sizeof(value));
  if (res != sizeof(value)) {
    perror("I2C: Unable to read from i2c register");
    exit(1);
  }
  return value;
}

void initializeStartRegisters(){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, SYS_SETUP_REG, 0x00); //write to the system setup register to turn on the matrix.
    writeI2cReg(i2cFileDesc, DISPLAY_SETUP_REG, 0x00); //write to display setup register to turn on LEDs, no flashing.
}

typedef struct {
    char digit; // 0-9 or . or empty space
    char cols; // how wide is this character in terms of columns
    char rowBitArr[NUMBER_OF_MATRIX_ROWS]; // represents each row of bits of the char
} charInfo;

static charInfo charInfoMatrix [] = { // holds all the bit data for each row for every character that may need to be displayed
    {'0', '4', {0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x04, 0x00}},
    {'1', '4', {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E, 0x00}},
    {'2', '4', {0x04, 0x0A, 0x02, 0x04, 0x04, 0x08, 0x0E, 0x00}},
    {'3', '4', {0x0C, 0x02, 0x02, 0x0E, 0x02, 0x02, 0x0C, 0x00}},
    {'4', '4', {0x02, 0x06, 0x0A, 0x0A, 0x0E, 0x02, 0x02, 0x00}},
    {'5', '4', {0x0E, 0x08, 0x08, 0x0E, 0x02, 0x0A, 0x04, 0x00}},
    {'6', '4', {0x06, 0x08, 0x08, 0x0C, 0x0A, 0x0A, 0x04, 0x00}},
    {'7', '4', {0x0E, 0x02, 0x02, 0x02, 0x04, 0x04, 0x04, 0x00}},
    {'8', '4', {0x04, 0x0A, 0x0A, 0x04, 0x0A, 0x0A, 0x04, 0x00}},
    {'9', '4', {0x04, 0x0A, 0x0A, 0x06, 0x02, 0x02, 0x0C, 0x00}},
    {'.', '1', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}},
    {' ', '4', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    
};

static charInfo* getCharInfo{
    for(int i = 0;  ){

    }
    return NULL;
}

void displayMatrix(char message){
    
}


int displayInteger(int i){
    int newInt = 0; //do some math: eg. from 0-4096 to 0-99
    return newInt;
}

double displayDouble(double d){ 
    double newDouble = 0; //do some math: eg. from 0-4096 to 0-9.9
    return newDouble;
}