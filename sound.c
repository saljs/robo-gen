/*
 *    Output sounds for different events
 *    Sal Skare
 */

#include "sound.h"

void Sound_on()
{
    softToneWrite(3, 400);
    delay(300);
    softToneWrite(3, 500);
    delay(300);
    softToneWrite(3, 600);
    delay(300);
    softToneWrite(3, 700);
    delay(300);
    softToneWrite(3, 600);
    delay(200);
    softToneWrite(3, 700);
    delay(500);
    softToneWrite(3, 0);
}

void Sound_off()
{
    softToneWrite(3, 700);
    delay(300);
    softToneWrite(3, 600);
    delay(300);
    softToneWrite(4, 600);
    delay(300);
    softToneWrite(3, 0);
}

void Sound_hit()
{
    softToneWrite(3, 800);
    delay(300);
    softToneWrite(3, 100);
    delay(100);
    softToneWrite(3, 0);
}

void Sound_limit()
{
    softToneWrite(3, 100);
    delay(300);
    softToneWrite(3, 800);
    delay(100);
    softToneWrite(3, 0);
}

void Sound_beep()
{
    softToneWrite(3, 950);
    delay(300);
    softToneWrite(3, 0);
}
