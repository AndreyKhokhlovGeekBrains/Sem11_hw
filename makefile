all: sensor_demo

sensor_demo: main.o temp_functions.o
	gcc -o sensor_demo temp_functions.o main.o

main.o: main.c
	gcc -c --std=c99 -o main.o main.c

temp_functions.o: temp_functions.c temp_functions.h
	gcc -c --std=c99 -o temp_functions.o temp_functions.c

clean:
	del *.o
	del sensor_demo.exe