/*    
 *    Genetic algorithm specific funtions
 *    Sal Skare
 */

#include "genetics.h"

//initialize a random starting population
void init_pop(gene* population)
{
	//initialize the genome to a random binary string
    for(int c = 0; c < 60; c++)
    {
        if(random() >= (RAND_MAX/2))
        {
            population->chrome[c] = 1;
        }
        else
        {
            population->chrome[c] = 0;
        }
    }
    //set the score to 0
    population->score = 0;
}

//use the 'roulette wheel' method to select members from a population
int roulettewheel(gene population[])
{
	//add together all the scores of the generation to create a total
    float tmp = 0;
    for(int i = 0; i < BATCHSIZE; i++)
    {
        tmp = population[i].score + tmp;
    }
    //multiple by 1000000 to approximate it into an int value
    int total;
    total = tmp * 1000000;
    //choose a random nuber greater than 0 and less than the total of the scores in int value
    int ball = random() % total;

    //check to see where the ball has 'landed' within the total previosly created 
    int wheel = 0;
    for(int i = 0; i < BATCHSIZE; i++)
    {
        if(ball <= (population[i].score * 1000000) + wheel)
        {
            return i;
        }
        else
        {
            wheel = wheel + (population[i].score * 1000000);
        }
    }
    //return -1 on errors
    return -1;
}

//take two members of an existing population, and generate two for a new one
void newpop(int a, int b, gene population[], gene* newgen_a, gene* newgen_b)
{
    gene tmp_a;
    gene tmp_b;
    tmp_a.score = 0;
    tmp_b.score = 0;
    //check to see if the bitstrings are crossed
    if(random() <= RAND_MAX * CROSSOVERRATE)
    {
		//select a random point along the string, create two new genes with everything after that point flipped
        int crosslen = random() % 60;
        int i;
        for(i = 0; i < crosslen; i++)
        {
            tmp_a.chrome[i] = population[a].chrome[i];
        }
        while(i < 60)
        {
            tmp_a.chrome[i] = population[b].chrome[i];
            i++;
        }

        for(i = 0; i < crosslen; i++)
        {
            tmp_b.chrome[i] = population[b].chrome[i];
        }
        while(i < 60)
        {
            tmp_b.chrome[i] = population[a].chrome[i];
            i++;
        }

        *newgen_a = tmp_a;
        *newgen_b = tmp_b;
    }
    else
    {
		//copy the old genomes into the new ones unchanged
        *newgen_a = population[a];
        *newgen_b = population[b];
    }
    
    //parse over both genes and mutate bits proportional to the mutation rate
    for(int i = 0; i < 60; i++)
    {
        if(random() <= RAND_MAX * MUTATIONRATE)
        {
            if(newgen_a->chrome[i] == 0)
            {
                newgen_a->chrome[i] = 1;
            }
            else if(newgen_a->chrome[i] == 1)
            {
                newgen_a->chrome[i] = 0;
            }
        }
    }
    for(int i = 0; i < 60; i++)
    {
        if(random() <= RAND_MAX * MUTATIONRATE)
        {
            if(newgen_b->chrome[i] == 0)
            {
                newgen_b->chrome[i] = 1;
            }
            else if(newgen_b->chrome[i] == 1)
            {
                newgen_b->chrome[i] = 0;
            }
        }
    }
}

//write a population and generation number to a file
int saveGenes(gene* population, int genum)
{
    FILE* genesav = fopen("/home/pi/gene.save", "w");
    if(genesav == NULL)
    {
        return 1;
    }
    fwrite(population, sizeof(gene), BATCHSIZE, genesav);
    fwrite(&genum, sizeof(int), 1, genesav);
    fclose(genesav);
    return 0;
}

//read a population and generation number from a file
int read_pop(gene* population)
{
    FILE* genesav = fopen("/home/pi/gene.save", "r");
    if(genesav == NULL)
    {
        return -1;
    }
    fread(population, sizeof(gene), BATCHSIZE, genesav);
    int genum;
    fread(&genum, sizeof(int), 1, genesav);
    fclose(genesav);
    return genum;
}

//take the bitstring of a genome, and convert it into a readable struct for easier parsing
decodedGene parseGene(gene toparse)
{
    decodedGene tmp;

    tmp.distance = 0;
    for(int i = 0; i < 10; i++)
    {
        if(toparse.chrome[i] == 1)
        {
            tmp.distance = tmp.distance | 1<<(9-i);
        }
    }

    tmp.degrees = 0;
    for(int i = 10; i < 19; i++)
    {
        if(toparse.chrome[i] == 1)
        {
            tmp.degrees = tmp.degrees | 1<<(18-i);
        }
    }

    if(toparse.chrome[19] == 1)
    {
        tmp.direction = 'r';
    }
    else
    {
        tmp.direction = 'l';
    }

    tmp.limit = 0;
    for(int i = 20; i < 30; i++)
    {
        if(toparse.chrome[i] == 1)
        {
            tmp.limit = tmp.limit | 1<<(29-i);
        }
    }

    int tmpThresh = 0;
    for(int i = 30; i < 40; i++)
    {
        if(toparse.chrome[i] == 1)
        {
            tmpThresh = tmpThresh | 1<<(39-i);
        }
    }
    tmp.threshold = (float)tmpThresh/100;

    tmp.delay = 0;
    for(int i = 40; i < 50; i++)
    {
        if(toparse.chrome[i] == 1)
        {
            tmp.delay = tmp.delay | 1<<(49-i);
        }
    }

    tmp.ttl = 0;
    for(int i = 50; i < 60; i++)
    {
        if(toparse.chrome[i] == 1)
        {
            tmp.ttl = tmp.ttl | 1<<(59-i);
        }
    }

    //return the decoded struct
    return tmp;
}
