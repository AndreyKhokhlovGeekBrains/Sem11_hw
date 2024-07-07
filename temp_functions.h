#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 100
#define MAX_SENSOR_SIZE 600000
#define NUMBER_OF_ELEMENTS 6
#define ERROR_CONVERSION_TO_NUMBER 5000
#define ERROR_CALCULATION 6000
#define MONTH_IS_MISSING 7000

typedef struct data
{
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t minute;
    int8_t t;
} sensor;

void AddRecord(sensor* record, int index, int day, int month, int year, int hour, int minute, int temp);

int stringToNumber(char* string);

int AddInfo(sensor* dataStorage, char* path, int* errors);

float averageYearTemperature(sensor *dataStorage, int number_of_lines);

int maxYearTemperature(sensor *dataStorage, int number_of_lines);

int minYearTemperature(sensor *dataStorage, int number_of_lines);

float averageMonthTemperature(sensor *dataStorage, int number_of_lines, int month);

int maxMonthTemperature(sensor *dataStorage, int number_of_lines, int month);

int minMonthTemperature(sensor *dataStorage, int number_of_lines, int month);

void printData(sensor *dataStorage, int number_of_lines);

char* defineMonth(int month_number);

void printMonthlyStatistics(sensor *dataStorage, int number_of_lines);

void printStatisticsForMonth(sensor *dataStorage, int number_of_lines, int month);

void printYearlyStatistics(sensor *dataStorage, int number_of_lines);

void printDescription(void);

void printHelp(void);

void commandLineArgs(int argc, char* argv[]);
