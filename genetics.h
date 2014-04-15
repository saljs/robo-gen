/*    
 *    Genetic algorithm specific funtions
 *    Sal Skare
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef GENETICS_H_
#define GENETICS_H_

//size of the batch of genomes
#define BATCHSIZE 6

//rate at which bitstings cross
#define CROSSOVERRATE 0.9

//mutation rate for genomes
#define MUTATIONRATE 0.005

//penalty for running into something
#define PENALTY 0.01

//datatype to store genomes in
typedef struct
{
    int chrome[60];
    double score;
} gene;

//datatype to store decoded information from genomes
typedef struct
{
    int distance;
    int degrees;
    char direction;
    int limit;
    float threshold;
    int delay;
    int ttl;
} decodedGene;

void init_pop(gene* population);
int roulettewheel(gene population[]);
void newpop(int a, int b, gene population[], gene* newgen_a, gene* newgen_b);
int saveGenes(gene* population, int genum);
int read_pop(gene* population);
decodedGene parseGene(gene toparse);

#endif
