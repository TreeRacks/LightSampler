#include "LEDMatrix.h"
#include "joystick.h"
#include "sampling.h"
#include "mainHelper.h"
#include "displayJoystick.h"
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
  
  //Starting new threads
  displayJoystick_startDisplay();
  Sampler_startSampling();
  startAnalyzing();

  //On main thread, just stay in loop till user button pressed to runs clean up
  pressButtonToEndProgram();

  //cleaning up threads
  displayJoystick_stopDisplay();
  Sampler_stopSampling();
  stopAnalyzing();

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
  printf("\nDone shutdown! Goodbye!");
  return 0;
}