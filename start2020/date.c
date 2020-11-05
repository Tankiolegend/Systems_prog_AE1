#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "date.h"
//
// Created by James Ross on 29/10/2020.
//

//Defines structs
struct date{

    int day;
    int month;
    int year;

};

//Creates an instance of the date struct
Date *date_create(char *datestr){

    struct date *p;

    char delim[] = "/";
    char datevalue[strlen(datestr)] ;
    strcpy(datevalue,datestr);

    char *ptr = strtok(datevalue, delim);

    //Allocates memory for the date struct, if successful initialise values
    if ((p = (struct date *)malloc(sizeof(struct date))) != NULL){

        p->day = atoi(ptr);

        ptr = strtok(NULL, delim);
        p->month = atoi(ptr);

        ptr = strtok(NULL, delim);
        p->year = atoi(ptr);

    }

    return p;

}

//Duplicates a given date
Date *date_duplicate(Date *d){

    struct date *p;

    if ((p = (struct date *)malloc(sizeof(struct date))) != NULL){

        p->day = d->day;
        p->month = d->month;
        p->year = d->year;

    }

    return p;

}

//Compares two dates to ee which is more recent
int date_compare(Date *date1, Date *date2){

    if(date1->year == date2->year){

        if(date1->month == date2->month){

            if(date1->day == date2->day){

                return 0;

            }else if(date1->day < date2->day){

                return -1;

            } else{

                return 1;

            }

        }else if(date1->month < date2->month){

            return -1;

        }else{

            return 1;

        }

    }else if(date1->year < date2->year){

        return -1;

    }else{

        return 1;

    }


}

//Frees the memory allocated to a date
void date_destroy(Date *d){

    free(d);

}

