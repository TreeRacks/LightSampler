#include "LEDMatrix.h"
#include "joystick.h"
#include "sampling.h"
#include "mainHelper.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define I2C_DEVICE_ADDRESS 0x70
#define SYS_SETUP_REG 0X21
#define DISPLAY_SETUP_REG 0x81
#define REG_DIRA 0x04
#define REG_DIRB 0x06
#define REG_OUTA 0x08
#define REG_OUTB 0x0A

int main(){


  printf("Drive display (assumes GPIO #61 and #44 are output and 1\n");
  configureI2C();
  initializeStartRegisters();
  int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
  for(int i = 0; i < 16; i+=2){
    writeI2cReg(i2cFileDesc, i, 0x00);
   }
  // writeI2cReg(i2cFileDesc, 0x00, 0x02);
  // writeI2cReg(i2cFileDesc, 0x02, 0x03);
  // writeI2cReg(i2cFileDesc, 0x04, 0x82);
  // writeI2cReg(i2cFileDesc, 0x06, 0x82);
  // writeI2cReg(i2cFileDesc, 0x08, 0x83);
  // writeI2cReg(i2cFileDesc, 0x0A, 0x02); 
  // writeI2cReg(i2cFileDesc, 0x0C, 0x02); 
  // writeI2cReg(i2cFileDesc, 0x0E, 0x00);
  
  displayInt(77);
  //writeI2cReg(i2cFileDesc, 0x00, 0x40);
  // Drive an hour-glass looking character
  // (Like an X with a bar on top & bottom)
  // writeI2cReg(i2cFileDesc, REG_OUTA, 0x2A);
  // writeI2cReg(i2cFileDesc, REG_OUTB, 0x54);
  // Read a register:
  unsigned char regVal = readI2cReg(i2cFileDesc, REG_OUTA);
  printf("Reg OUT-A = 0x%02x\n", regVal);
  // Cleanup I2C access;
  close(i2cFileDesc);
    // while(1){
    //     printf("The x reading is%f", Joystick_readX());
    //     printf("\n");
    //     printf("The y reading is%f", Joystick_readY());
    // }
  printf("Hello world!\n");
  return 0;
}