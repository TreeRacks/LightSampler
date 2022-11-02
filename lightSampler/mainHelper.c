#include "mainHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void configureI2C(){
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
}

void runCommand(char* command)
{
 // Execute the shell command (output into pipe)
 FILE *pipe = popen(command, "r");
 // Ignore output of the command; but consume it
 // so we don't get an error when closing the pipe.
 char buffer[1024];
 while (!feof(pipe) && !ferror(pipe)) {
 if (fgets(buffer, sizeof(buffer), pipe) == NULL)
 break;
 // printf("--> %s", buffer); // Uncomment for debugging
 }
 // Get the exit code from the pipe; non-zero is an error:
 int exitCode = WEXITSTATUS(pclose(pipe));
 if (exitCode != 0) {
 perror("Unable to execute command:");
 printf(" command: %s\n", command);
 printf(" exit code: %d\n", exitCode);
 }
}

int readFromFileToScreen(char *button)
{
    FILE *pFile = fopen(button, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", button);
        exit(-1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);
    // Close
    fclose(pFile);
    //printf("Read: '%s'\n", buff);
    return(atoi(buff));
}

void writingToGPIO(float value){
    FILE *pFile = fopen("/sys/class/gpio/export", "w");
    if (pFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    fprintf(pFile, "%f", value);
    fclose(pFile);
}

bool UserButtonPressed(){
    return (readFromFileToScreen(userButton) == 0);
}

void pressButtonToEndProgram(){
    runCommand("config-pin p8.43 gpio");
    runCommand("config-pin -q p8.43");
    writingToGPIO(72);
    printf("Starting to sample data...\n");
    while(!UserButtonPressed()){
        if(UserButtonPressed){
            printf("Shutting down...\n");
        }
    }
}

