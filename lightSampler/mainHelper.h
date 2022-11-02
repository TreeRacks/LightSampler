#ifndef mainHelper_H_
#define mainHelper_H_

#define userButton "/sys/class/gpio/gpio72/value"

void runCommand(char* command);
void configureI2C();
bool UserButtonPressed();
void pressButtonToEndProgram();

#endif