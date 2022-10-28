#ifndef LEDMATRIX_H_
#define LEDMATRIX_H_

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define NUMBER_OF_MATRIX_ROWS 8
#define NUMBER_OF_MATRIX_COLS 8

int initI2cBus(char* bus, int address);
void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr);

void initializeStartRegisters();
void displayMatrix(char* display);

int displayInteger(int i);
double displayDouble(double d);

#endif