/*
 *    Main control loop and fitness function for robot
 *    Sal Skare
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#include "robot.h"
#include "sound.h"
#include "genetics.h"

float fitness(decodedGene vars, bool* isCrit);

int main(void)
{
    //wait for on button
    if(setup() != 0)
    {
        return 1;
    }
    Sound_on();
    while(digitalRead(11) == HIGH)
    {
        delay(100);
    }
    Sound_beep();

    //check the battery
    if(getBattery() <= 6.2)
    {
        stop();
        delay(100);
        Sound_beep();
        delay(100);
        Sound_beep();
        delay(100);
        system("/sbin/poweroff");
        return -1;
    }

    gene population[BATCHSIZE];
    int generation = 0;

    //read in or create population
    if(access("/home/pi/gene.save", F_OK) != -1)
    {
        generation = read_pop(&population);
        if(generation == -1)
        {
            return 2;
        }
    }
    else
    {
        for(int i = 0; i < BATCHSIZE; i++)
        {
            init_pop(&population[i]);
        }
    }

    gene newgen[BATCHSIZE];
    bool isCrit = false;

    //main loop
    while(true)
    {
        //save current population
        saveGenes(&population, generation);

        //score the population
        for(int i = 0; i < BATCHSIZE; i++)
        {
            //skip if already scored
            if(population[i].score != 0)
            {
                continue;
            }
            decodedGene toScore = parseGene(population[i]);
            population[i].score = fitness(toScore, &isCrit);
            if(isCrit == true)
            {
                stop();
                Sound_off();
                delay(100);
                system("/sbin/poweroff");
                return -1;
            }
            //save new score
            saveGenes(&population, generation);
        }

        //log some stats
        float avg = 0;
        for(int i = 0; i < BATCHSIZE; i++)
        {
            avg = avg + population[i].score;
        }
        avg = avg / BATCHSIZE;
        FILE* logfile = fopen("/home/pi/robogen.log", "a");
        if(logfile == NULL)
        {
            return 3;
        }
        fprintf(logfile, "Gen: %d Avg: %f\n", generation, avg);
        fclose(logfile);

        //generate a new population
        int c = 0;
        while(c < BATCHSIZE)
        {
            newpop(roulettewheel(&population), roulettewheel(&population), population, &newgen[c], &newgen[c+1]);
            c = c+2;
        }

        //copy new generation to the population
        for(int i = 0; i < BATCHSIZE; i++)
        {
            population[i] = newgen[i];
        }
        generation++;
    }
    return 0;
}

//return a score based on how well a genome works withing 5 mins of running
float fitness(decodedGene vars, bool* isCrit)
{
    unsigned long start = millis();
    unsigned long turnint = 0;

    int counter = 0;
    int turnc = 0;
    float avg = 0;
    float penal = 0;
    bool isCrashed = false;
    bool isStopped = true;


    //take battery start voltage
    float startBatt = getBattery();

    //exit after 5 mins
    while(millis() - start < 300000)
    {
        //check for collisions
        if(getDistance() < vars.limit / 10 || isCrashed == true)
        {
            stop();
            Sound_limit();
            delay(500);
            backward();
            delay(vars.distance * 10);
            turn(vars.degrees, vars.direction);
            isCrashed = false;
            isStopped = true;
        }

        //check to see if this is a good spot to stop
        if(getSolar() > vars.threshold)
        {
            stop();
            while(getSolar() > vars.threshold && millis() - start < 300000)
            {
                //check to see if battery is dead or program is interrupted manually
                delay(500);
                if(getBattery() <= 6.2 || digitalRead(11) == LOW)
                {
                    *isCrit = true;
                    return -1;
                }
            }
            isStopped = true;
        }

        //make sure we have enough juice
        if(getBattery() <= 6.2)
        {
            stop();
            isStopped = true;
            delay(500);
            if(getBattery() <= 6.2)
            {
                *isCrit = true;
                return -1;
            }
        }

        //turn toward greatest light source if applicable
        turnint = (millis() - start) - (vars.ttl * 100 * turnc);
        if(turnint >= vars.ttl * 100)
        {
            turnTowardLight();
            turnc++;
            isStopped = true;
        }

        //add some stuff to averages
        counter++;
        avg = avg + getSolar();

        if(isStopped == true)
        {
            forward();
            isStopped = false;
        }

        unsigned long deltimest = millis();

        //drive forwards unless stopped
        while(millis() - deltimest < vars.delay * 100)
        {
            //if the robot has run into something
            if(digitalRead(5) == LOW)
            {
                stop();
                Sound_hit();
                //subtract a penalty from the score and manuever to change direction
                penal = penal + PENALTY;
                isCrashed = true;
                break;
            }
            //if there's been a manual interrupt
            if(digitalRead(11) == LOW)
            {
                *isCrit = true;
                return -1;
            }
            delay(200);
        }
    }
    //return a score for the gene
    stop();
    float endBatt = (getBattery() - startBatt) * 0.1;
    //score is: solar avg - penalies - battery drop/10
    return avg / counter - penal + endBatt;
}
