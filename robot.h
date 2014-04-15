/*    
 *    Sensing and motor funtions for robot
 *    Sal Skare
 */

//library for GPIO functions
#include <wiringPi.h>
//library for ATD converter I'm using
#include <mcp3004.h>
#include <stdlib.h>
#include <time.h>
//library for producing sounds on GPIO pins
#include <softTone.h>

#ifndef ROBOT_H_
#define ROBOT_H_

//struct to store analog values from photoresistors
typedef struct
{
    int Fleft;
    int Fright;
    int Bleft;
    int Bright;
} cds;

int setup();
int getDistance();
cds readLights();
void forward();
void backward();
void stop();
void turn(int degrees, char direction);
void turnTowardLight();
float getBattery();
float getSolar();

#endif
