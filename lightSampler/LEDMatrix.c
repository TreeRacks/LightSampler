#include "LEDMatrix.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define I2C_DEVICE_ADDRESS 0x70
#define SYS_SETUP_REG 0X21
#define DISPLAY_SETUP_REG 0x81

static unsigned char logicalFrameArr[NUMBER_OF_MATRIX_ROWS];
static unsigned char physicalFrameArr[NUMBER_OF_MATRIX_ROWS*2];

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


void writeI2cBytes(unsigned char* physicalFrameValues){
  int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
  // for(int i = 0; i < 16; i += 2){ // for all 8 rows
  //  for(int j = 0; j < 8; j++){
  //   writeI2cReg(i2cFileDesc, i, physicalFrameValues[j]);
  //     printf("%c\n", physicalFrameValues[j]);
      writeI2cReg(i2cFileDesc, 0x00, physicalFrameValues[0]);
      writeI2cReg(i2cFileDesc, 0x02, physicalFrameValues[1]);
      writeI2cReg(i2cFileDesc, 0x04, physicalFrameValues[2]);
      writeI2cReg(i2cFileDesc, 0x06, physicalFrameValues[3]);
      writeI2cReg(i2cFileDesc, 0x08, physicalFrameValues[4]);
      writeI2cReg(i2cFileDesc, 0x0A, physicalFrameValues[5]);
      writeI2cReg(i2cFileDesc, 0x0C, physicalFrameValues[6]);
      writeI2cReg(i2cFileDesc, 0x0E, physicalFrameValues[7]);
  //   }
  // }
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
    {'0', 4, {0x20, 0x50, 0x50, 0x50, 0x50, 0x50, 0x20, 0x00}},
    {'1', 4, {0x20, 0x30, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00}},
    {'2', 4, {0x20, 0x50, 0x40, 0x20, 0x20, 0x10, 0x70, 0x00}},
    {'3', 4, {0x30, 0x40, 0x40, 0x70, 0x40, 0x40, 0x30, 0x00}},
    {'4', 4, {0x40, 0x60, 0x50, 0x50, 0x70, 0x40, 0x40, 0x00}},
    {'5', 4, {0x70, 0x10, 0x10, 0x70, 0x40, 0x50, 0x20, 0x00}},
    {'6', 4, {0x60, 0x10, 0x10, 0x30, 0x50, 0x50, 0x20, 0x00}},
    {'7', 4, {0x70, 0x40, 0x40, 0x40, 0x20, 0x20, 0x20, 0x00}},
    {'8', 4, {0x20, 0x50, 0x50, 0x20, 0x50, 0x50, 0x20, 0x00}},
    {'9', 4, {0x20, 0x50, 0x50, 0x60, 0x40, 0x40, 0x30, 0x00}},
    {'.', 1, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40}},
    {' ', 4, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
};

#define EMPTY 0
charInfo* searchForCharInfo(char c){ // searches for a char and then returns the address if it is found
    for(int i = 0; charInfoMatrix[i].digit != EMPTY; i++){
      if (charInfoMatrix[i].digit == c){
        return &charInfoMatrix[i];
      }
    }
    return NULL;
}

char shiftLeftOnMatrixBy(int x, char c){ //shiftLeftBy(2,'1')
  if(x >= 0){
    return c >> x;
  }
  else{
    return c << -1*x; //do nothing
  }
  return 0;
}



void logicalFrame(){
  for(int i = 0; i < NUMBER_OF_MATRIX_ROWS; i++){
    //unsigned char logicalRows = logicalFrameArr[i]; // use [i] or [NUMBER_OF_MATRIX_ROWS - i - 1] instead to flip the direction.

    unsigned char physicalRows = ((logicalFrameArr[i] >> 1) | (logicalFrameArr[i] << 7)); // & 0xFF;

    physicalFrameArr[i] = physicalRows;
    physicalFrameArr[i*2 + 1] = 0x00; // might not be necessary
    printf("%d\n", logicalFrameArr[i]);
  }
  writeI2cBytes(physicalFrameArr);
}

void displayMatrix(char* display){
  
  char current = ' '; // initialize the current char to be empty
  if(*display != EMPTY){
    current = *display; 

  }
  charInfo* currentCharInfo = searchForCharInfo(current);
  printf("search returns %d\n",searchForCharInfo(current)->cols);
  char* charRowByRowBits = currentCharInfo->rowBitArr; 
  int charCurrentColumns = currentCharInfo->cols;
  printf("current columns is %d\n", charCurrentColumns);
  for(int col = 0; col < NUMBER_OF_MATRIX_COLS; col+=charCurrentColumns){ // go through all columns
    current = ' '; // initialize the current char to be empty
    if(*display != EMPTY){
      current = *display;
      ++display;      
    }
    printf("current is %c\n",current);

    currentCharInfo = searchForCharInfo(current);
    printf("search returns %p\n", searchForCharInfo(current));
    charRowByRowBits = currentCharInfo->rowBitArr; //charInfo or charInfoMatrix
    printf("currentCharInfo is %d\n", currentCharInfo->rowBitArr[1]);

    charCurrentColumns = currentCharInfo->cols;

    for(int i = 0; i < NUMBER_OF_MATRIX_ROWS; i++){ // for every row
      int n = NUMBER_OF_MATRIX_COLS - col - charCurrentColumns;
      printf("n is %d\n", n);
      printf("rowbits before shifting are %d\n", charRowByRowBits[i]);
      char rowBits = shiftLeftOnMatrixBy(n,charRowByRowBits[i]); //shift each digit left on the display with a right bitwise shift (>>)
      printf("rowbits are %d\n", rowBits);
      logicalFrameArr[i] |= rowBits;
    }

   
    }
   logicalFrame();
  }




void displayInteger(int i){
  if(i > 99){
    i = 99;
  }
  else if(i < 0){
    i = 0; 
  }
  else if ((i <= 99) | (i >= 0)){
    char buff[10];
    snprintf(buff, 10, "%d", i);
    displayMatrix(buff); //do some math: eg. from 0-4096 to 0-99
  }
}

void displayDouble(double d){ 
  if(d > 9.9){
    d = 9.9;
  }
  else if(d < 0.0){
    d = 0.0; 
  }
  else if ((d <= 9.9) | (d >= 0.0)){
    char buff[10];
    snprintf(buff, 10, "%f", d);
    displayMatrix(buff); //do some math: eg. from 0-4096 to 0-99
  }
    
}