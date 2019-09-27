#include <iostream>
#include <string.h>

typedef struct{
    int id;
    char name[20];
    float percentage;
} status;

int main()
{
    status record;
    record.id=1;
    strcpy(record.name, "Raju");
    record.percentage = 86.5;
    printf(" Id is: %d \n", record.id);
    printf(" Name is: %s \n", record.name);
    printf(" Percentage is: %f \n", record.percentage);
    return 0;
}