all:
	arm-linux-gnueabihf-gcc -Wall -pthread -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror main.c joystick.c LEDMatrix.c mainHelper.c sampling.c analyzing.c displayJoystick.c -o main
	cp main $(HOME)/cmpt433/public/myApps

