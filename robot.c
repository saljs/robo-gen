/*    
 *    Sensing and motor funtions for robot
 *    Sal Skare
 */

#include "robot.h"

//setup different inputs and outputs
int setup()
{
	//load the spi daemon
    system("/usr/local/bin/gpio load spi");
    if (wiringPiSetup () == -1)
    {
        return 1;
    }

    //atd converter
    mcp3004Setup (200, 0);

    //collision sensors
    pinMode (5, INPUT);

    //motors
    pinMode(7, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(2, OUTPUT);

    //ultrasonic
    pinMode(8, OUTPUT);
    pinMode(9, INPUT);

    //Power button
    pinMode (11, INPUT);

    //random seed
    struct timeval time;
    gettimeofday(&time,NULL);
    srandom((time.tv_sec * 100) + (time.tv_usec / 100));

    //pizeo setup
    pinMode(3, OUTPUT);
    softToneCreate(3);

    //status LED
    pinMode(6, OUTPUT);
    //turn the LED on. When the program exits it will turn off, indicating a dead battery
    digitalWrite(6, HIGH);

    return 0;
}

//read distance in CM from an ultrasonic sensor on the front of the robot
int getDistance()
{
    //Send trig pulse
    digitalWrite(8, HIGH);
    delayMicroseconds(20);
    digitalWrite(8, LOW);

    //Wait for echo start
    while(digitalRead(9) == LOW);

    //Wait for echo end
    long startTime = micros();
    while(digitalRead(9) == HIGH);
    long travelTime = micros() - startTime;

    //Get distance in cm
    int distance = travelTime / 58;

    return distance;
}

//read values from 4 photocells connected to the ATD converter
cds readLights()
{
    cds tmp;
    tmp.Fleft = analogRead(200);
    tmp.Fright = analogRead(201);
    tmp.Bleft = analogRead(203);
    tmp.Bright = analogRead(202);

    return tmp;
}

//move the robot forward untill stopped
void forward()
{
    digitalWrite(2, LOW);
    delay(300);
    digitalWrite(7, HIGH);
    digitalWrite(4, HIGH);
    delay(300);
    digitalWrite(2, HIGH);
}

//move the robot backward untill stopped
void backward()
{
    digitalWrite(2, LOW);
    delay(300);
    digitalWrite(7, LOW);
    digitalWrite(4, LOW);
    delay(300);
    digitalWrite(2, HIGH);
}

//stop the robot
void stop()
{
    digitalWrite(2, LOW);
    delay(300);
    digitalWrite(7, LOW);
    digitalWrite(4, LOW);
}

//turn the X degrees in direction Y
void turn(int degrees, char direction)
{
    digitalWrite(2, LOW);
    delay(300);
    int millis = degrees * 28;

    if(direction == 'l')
    {
        digitalWrite(7, LOW);
        digitalWrite(4, HIGH);
        delay(300);
        digitalWrite(2, HIGH);
        delay(millis);
        digitalWrite(2, LOW);
    }
    else if(direction == 'r')
    {
        digitalWrite(7, HIGH);
        digitalWrite(4, LOW);
        delay(300);
        digitalWrite(2, HIGH);
        delay(millis);
        digitalWrite(2, LOW);
    }
}

//turn the robot toward the greatest source of light, as read from the photocells
void turnTowardLight()
{
    cds mostlight = readLights();
    int largest, last = 0;
    int allcds[4];
    allcds[0] = mostlight.Fleft;
    allcds[1] = mostlight.Fright;
    allcds[2] = mostlight.Bleft;
    allcds[3] = mostlight.Bright;
    for(int i = 0; i < 4; i++)
    {
        if(allcds[i] > last)
        {
            largest = i;
        }
        last = allcds[i];
    }

    if(largest == 0)
    {
        turn(20, 'l');
    }
    else if(largest == 1)
    {
        turn(20, 'r');
    }
    else if(largest == 2)
    {
        turn(160, 'l');
    }
    else if(largest == 3)
    {
        turn(160, 'r');
    }
}

//get the voltage of the battery
float getBattery()
{
    int val = analogRead(207);
    float Vout = (val / 1023.0) * 3.3;
    float Vin = Vout/0.333333;
    return Vin;
}

//get the voltage from the solar panel
float getSolar()
{
    int val = analogRead(206);
    float Vout = (val / 1023.0) * 3.3;
    float Vin = Vout/0.357143;
    return Vin;
}
