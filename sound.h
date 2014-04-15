/*    
 *    Output sounds for different events
 *    Sal Skare
 */

#include <wiringPi.h>
#include <softTone.h>

#ifndef SOUND_H_
#define SOUND_H_

void Sound_on();
void Sound_off();
void Sound_hit();
void Sound_limit();
void Sound_beep();

#endif
