#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "temp_functions.h"

void AddRecord(sensor* record, int index, int day, int month, int year, int hour, int minute, int temp)
{
    record[index].day = day;
    record[index].month = month;
    record[index].year = year;
    record[index].hour = hour;
    record[index].minute = minute;
    record[index].t = temp;
}

int stringToNumber(char* string) {
    int negative_sign = 0;
    int len = strlen(string);
    string[len] = '\0';
    int num = 0;   
    if(len == 0) {
        return ERROR_CONVERSION_TO_NUMBER;
    }
    for(int i = 0; i < len; i++) {
        if(i == 0 && string[i] == '-') {
            negative_sign = 1;
            continue;
        }
        if(string[i] < '0' || string[i] > '9') {
            return ERROR_CONVERSION_TO_NUMBER;
        }
        int digit = string[i] - '0';
        num = num * 10 + digit;
    }
    if(negative_sign) {
        num = -num;
    }
    return num;
}

int AddInfo(sensor* dataStorage, char* path, int* errors)
{
    int index = 0;
    int number_of_lines = 0;
    int error_lines = 0;
    FILE* f_read = fopen(path, "r");
    if(f_read == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    char* line = (char*)malloc(MAX_LINE_LENGTH);
    if (line == NULL) {
        perror("Memory allocation failed");
        fclose(f_read);
        return EXIT_FAILURE;
    }
    char* buffer = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);
        if (buffer == NULL) {
            perror("Memory allocation failed");
            free(line);
            fclose(f_read);
            return EXIT_FAILURE;
        }
    while (fgets(line, MAX_LINE_LENGTH, f_read))
    {
        // Check if the first few characters match the UTF-8 BOM sequence (0xEF, 0xBB, 0xBF). BOM: Byte Order Mark at the start of a CSV file.
        if (strncmp(line, "\xEF\xBB\xBF", 3) == 0) {
            memmove(line, line + 3, strlen(line) - 2); // destination pointer, source pinter, number of bytes to copy from the source pointer
        }
        int line_len = strlen(line);
        int buf_index = 0;
        int lineData[NUMBER_OF_ELEMENTS];
        int elementsCount = 0;
        int num = 0;
        int flag = 1;
        for(int i = 0; i < line_len; i++) {
            if(line[i] == ',' || line[i] == ';' || i == line_len - 1) {
                buffer[buf_index] = '\0';
                num = stringToNumber(buffer);
                if(num == ERROR_CONVERSION_TO_NUMBER) {
                    flag = 0;
                    printf("Error reading data in line: %d (source line %d)\n", number_of_lines + 1, __LINE__);
                }
                lineData[elementsCount++] = num;
                buf_index = 0;
                continue;
            } else {
                buffer[buf_index++] = line[i];
            }
        }
        if(flag) {
            AddRecord(dataStorage, index++, lineData[2], lineData[1], lineData[0], lineData[3], lineData[4], lineData[5]);
        } else {
            error_lines++;
            continue;
        }
        number_of_lines++;
    }
    free(buffer);
    free(line);
    fclose(f_read);
    *errors = error_lines;
    return number_of_lines;
}

float averageYearTemperature(sensor *dataStorage, int number_of_lines){
    if (number_of_lines == 0) {
        return 0.0;
    }

    float sum = 0;
    sensor *currentSensor = dataStorage;

    for(int i = 0; i < number_of_lines; i++) {
        sum += currentSensor->t;
        currentSensor++;
    }

    return sum / number_of_lines;
}

int maxYearTemperature(sensor *dataStorage, int number_of_lines){
    if(dataStorage == NULL || number_of_lines <= 0) {
        return ERROR_CALCULATION;
    }

    sensor *currentSensor = dataStorage;
    int max_temp = currentSensor->t;

    for(int i = 1; i < number_of_lines; i++) {
        currentSensor++;
        if(currentSensor->t > max_temp) {
            max_temp = currentSensor->t;
        }
    }

    return max_temp;
}

int minYearTemperature(sensor *dataStorage, int number_of_lines){
    if(dataStorage == NULL || number_of_lines <= 0) {
        return ERROR_CALCULATION;
    }

    sensor *currentSensor = dataStorage;
    int min_temp = currentSensor->t;

    for(int i = 1; i < number_of_lines; i++) {
        currentSensor++;
        if(currentSensor->t < min_temp) {
            min_temp = currentSensor->t;
        }
    }

    return min_temp;
}

float averageMonthTemperature(sensor *dataStorage, int number_of_lines, int month){
    float sum = 0;
    int count = 0;
    for(int i = 0; i < number_of_lines; i++) {
        if(month == dataStorage[i].month) {
            sum += dataStorage[i].t;
            count++;
        }
    }
    if(count == 0) {
        return MONTH_IS_MISSING;
    }
    return sum / count;
}

int maxMonthTemperature(sensor *dataStorage, int number_of_lines, int month){
    if(dataStorage == NULL || number_of_lines <= 0 || month < 1 || month > 12) {
        return ERROR_CALCULATION;
    }
    int count = 0;

    int max = INT_MIN;
    for(int i = 0; i < number_of_lines; i++) {
        if(month == dataStorage[i].month) {
            count++;
            if(max < dataStorage[i].t) {
            max = dataStorage[i].t;
            }
        }
    }
    if(count == 0) {
        return MONTH_IS_MISSING;
    }

    return max;
}

int minMonthTemperature(sensor *dataStorage, int number_of_lines, int month){
    if(dataStorage == NULL || number_of_lines <= 0 || month < 1 || month > 12) {
        return ERROR_CALCULATION;
    }

    int count = 0;

    int min = INT_MAX;
    for(int i = 0; i < number_of_lines; i++) {
        if(month == dataStorage[i].month) {
            count++;
            if(min > dataStorage[i].t) {
            min = dataStorage[i].t;
            }
        }
    }
    if(count == 0) {
        return MONTH_IS_MISSING;
    }

    return min;
}

void printData(sensor *dataStorage, int number_of_lines)
{
    for(int i = 0; i < number_of_lines; i++) {
        printf("%02d.%02d.%4d %02d:%02d %3d\n",
            dataStorage[i].day,
            dataStorage[i].month,
            dataStorage[i].year,
            dataStorage[i].hour,
            dataStorage[i].minute,
            dataStorage[i].t
            );
    }
}

char* defineMonth(int month_number) {
    switch (month_number)
    {
    case 1:
        return "January";
        break;
    case 2:
        return "February";
        break;
    case 3:
        return "March";
        break;
    case 4:
        return "April";
        break;
    case 5:
        return "May";
        break;
    case 6:
        return "June";
        break;
    case 7:
        return "July";
        break;
    case 8:
        return "August";
        break;
    case 9:
        return "September";
        break;
    case 10:
        return "October";
        break;
    case 11:
        return "November";
        break;
    case 12:
        return "December";
        break;
    
    default:
        break;
    }
}

void printMonthlyStatistics(sensor *dataStorage, int number_of_lines) {
    float averageMonthTemp = 0;
    int minMonthTemp = 0;
    int maxMonthTemp = 0;
    for(int i = 1; i <= 12; i++) {
        averageMonthTemp = averageMonthTemperature(dataStorage, number_of_lines, i);
        minMonthTemp = minMonthTemperature(dataStorage, number_of_lines, i);
        maxMonthTemp = maxMonthTemperature(dataStorage, number_of_lines, i);

        printf(
            "Statistics for %s:\n average temperature:  %4.2f\n min temperature:     %4d\n max temperature:     %4d\n", 
        defineMonth(i),
        averageMonthTemp,
        minMonthTemp,
        maxMonthTemp
        );
    }
}

void printStatisticsForMonth(sensor *dataStorage, int number_of_lines, int month) {
    float averageMonthTemp = averageMonthTemperature(dataStorage, number_of_lines, month);
    int minMonthTemp = minMonthTemperature(dataStorage, number_of_lines, month);
    int maxMonthTemp = maxMonthTemperature(dataStorage, number_of_lines, month);

    printf(
        "Statistics for %s:\n average temperature:  %4.2f\n min temperature:     %4d\n max temperature:     %4d\n", 
    defineMonth(month),
    averageMonthTemp,
    minMonthTemp,
    maxMonthTemp
        );
}

void printYearlyStatistics(sensor *dataStorage, int number_of_lines) {
    float averageYearTemp = averageYearTemperature(dataStorage, number_of_lines);
    int minYearTemp = minYearTemperature(dataStorage, number_of_lines);
    int maxYearTemp = maxYearTemperature(dataStorage, number_of_lines);

    printf("Statistics for the year:\n average temperature: %3.2f\n min temperature: %3d\n max temperature for the year: %3d", averageYearTemp, minYearTemp, maxYearTemp);
}

void printDescription(void) {
    printf(
"The application displays statistics of the data read from a .csv file:\n \
key '-h' displays a description of the application's functionality;\n \
key '-f' is the path to the input .csv file, this key is set in the following format: 'path_to_folder\\\\file_name.csv';\n \
key '-m' is the number of the month for which statistics are to be displayed, if this key is set, then only statistics for the specified month are displayed, if the key is not set, then statistics are displayed monthly and for the whole year, if the -m flag is omitted, statistics will not be displayed;\n \
if the parameters are not set, then help is issued.");
}

void printHelp(void) {
    printf(
"Please enter the necessary parameters:\n \
'-f' in the format 'path_to_folder\\\\file_name.csv'\n \
'-m' with or without the month number. If the -m flag is omitted, statistics will not be displayed.");
}

void commandLineArgs(int argc, char* argv[]) {
    sensor* my_data = NULL;
    int error_lines = 0;
    int number_of_lines = 0;

    if(argc == 1) {
        printHelp();
    }

    int opt;
    while ((opt = getopt(argc, argv, "hf:m::")) != -1) {
        switch (opt) {
        case 'h':
            printDescription();
            break;
        case 'f':
            char* path = optarg;
            printf("Path to the input file is: %s \n", optarg);
            my_data = (sensor*)malloc(sizeof(sensor) * MAX_SENSOR_SIZE);
            if (my_data == NULL) {
                perror("Memory allocation failed");
                return;
            }
            number_of_lines = AddInfo(my_data, path, &error_lines);
            printf("Number of lines read: %d\n", number_of_lines);
            printf("Total number of lines in the file: %d\n", number_of_lines + error_lines);
            break;
        case 'm':
            int monthNum = atoi(optarg);
            if(monthNum) {
                printStatisticsForMonth(my_data, number_of_lines, monthNum);
            } else {
                printMonthlyStatistics(my_data, number_of_lines);
                printf("*** *** *** *** *** *** ***\n");
                printYearlyStatistics(my_data, number_of_lines);
            }
            free(my_data);
            break;
        case '?':
            printf("Error\n");
            break;
        default:
            printHelp();
            break;
        }
    }
}
