#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>
#include <omp.h>
#include <ctime>

//#define SERIAL

using namespace std;

const float GRAIN_GROWS_PER_MONTH =     8.0;    //inches
const float ONE_DEER_EATS_PER_MONTH =   0.5;

const float AVG_PRECIP_PER_MONTH =      6.0;
const float AMP_PRECIP_PER_MONTH =      6.0;
const float RANDOM_PRECIP =             2.0;    //inches

const float AVG_TEMP =                  50.0;
const float AMP_TEMP =                  20.0;
const float RANDOM_TEMP =               10.0;   //Farhrenheit

const float MIDTEMP =                   40.0;
const float MIDPRECIP =                 10.0;

const int LOOP_NUM =                    1e6;

int  NowYear;           // 2014 - 2019
int  NowMonth;          // 0 - 11

float NowPrecip;        // inches of rain per month
float NowTemp;          // temperature this month
float NowHeight;        // grain height in inches
int  NowNumDeer;        // current deer population
float PestsRate;


void GrainDeer();
void Grain();
void Watcher();
void Pests();

void incMonth();
void printState();
void getTempPrecip();
float getGrainHeight();
int getDeerGrowth();
float getPestsDamage();

float Ranf(float, float, unsigned int*);



int
main(int argc, char* argv[])
{
    const clock_t begin_time = clock();

    //initial values
    NowNumDeer = 1;
    NowHeight =  1.;
    NowMonth =    0;
    NowYear  = 2014;
    PestsRate = 0.;

    
    #ifndef SERIAL
    getTempPrecip();

    omp_set_num_threads(3);
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            GrainDeer();
        }
        
        #pragma omp section
        {
            Grain();
        }
        
         #pragma omp section
         {
             Pests();
         }
        
        #pragma omp section
        {
            Watcher();
        }
    }
    // implied barrier: all sections must complete before we get here
    #endif

    #ifdef SERIAL
    getTempPrecip();

    while(NowYear <=  2020)
    {
        int nextNumDeer = getDeerGrowth();
        NowNumDeer = nextNumDeer;
        float nextGrainHeight = getGrainHeight();
        NowHeight = nextGrainHeight;
        float nextPestRate = getPestsDamage();
        PestsRate = nextPestRate;
        printState();
        incMonth();
        getTempPrecip();

    }

    #endif

    cout << "Elapse time: " << float(clock() - begin_time) / CLOCKS_PER_SEC << endl;


    return 0;
}

void
Watcher()
{
    while( NowYear <= 2020 )
    {
        int thread;
        #ifndef SERIAL
        thread = omp_get_thread_num();
        #endif

        // DoneComputing barrier:
        #pragma omp barrier
        //printf("Watcher: %d\n", thread);
        
        // DoneAssigning barrier:
        #pragma omp barrier
        
        // Print results
        printState();
        
        // Increment month/year
        incMonth();
        
        // Calculate new Temperature and Precipitation
        getTempPrecip();
        
        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void
GrainDeer()
{
    while( NowYear <= 2020 )
    {
        int thread;
        #ifndef SERIAL
        thread = omp_get_thread_num();
        #endif

        int nextNumDeer = getDeerGrowth();
        //printf("Deer: %d\n", thread);

        // DoneComputing barrier:
        #pragma omp barrier
        
        NowNumDeer = nextNumDeer;
        
        // DoneAssigning barrier:
        #pragma omp barrier
        
        
        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void
Grain()
{
    while( NowYear <= 2020 )
    {
        int thread;
        #ifndef SERIAL
        thread = omp_get_thread_num();
        #endif

        float nextGrainHeight = getGrainHeight();
        //printf("Grain: %d\n", thread);

        // DoneComputing barrier:
        #pragma omp barrier
        
        NowHeight = nextGrainHeight;
        
        // DoneAssigning barrier:
        #pragma omp barrier
        
        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void
Pests()
{
    while( NowYear <= 2020 )
    {
        int thread;
        #ifndef SERIAL
        thread = omp_get_thread_num();
        #endif

        float nextPestRate = getPestsDamage();
        //printf("Pests: %d\n", thread);

        // DoneComputing barrier:
        #pragma omp barrier
        
        PestsRate = nextPestRate;
        
        // DoneAssigning barrier:
        #pragma omp barrier
        
        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void
incMonth()
{
    if (NowMonth == 11) {
        NowMonth = 0;
        ++NowYear;
    } else
        ++NowMonth;
}

float
getGrainHeight()
{
    float nextGrainHeight = NowHeight;
    
    float tempFactor = expf(-pow((NowTemp - MIDTEMP)/10, 2));
    float precipFactor = expf(-pow((NowPrecip - MIDPRECIP)/10, 2));
    
    nextGrainHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
    nextGrainHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
    nextGrainHeight *= (1 - PestsRate);
    
    if (nextGrainHeight < 0.0) nextGrainHeight = 0.0;

    int test;
    for(int i = 0; i < LOOP_NUM/100; i++)
    {
        int j = i;
    }
    
    return nextGrainHeight;
}

float
getPestsDamage()
{
    if((NowMonth >=3 && NowMonth <= 8) || NowTemp >= 50)
    {
        float r = (float) rand();      // 0 - RAND_MAX
        return  r / (float)RAND_MAX/ 2  + (NowMonth - 2)/12;
    }
    else
        return 0.;

    int test;
    for(int i = 0; i < LOOP_NUM/2; i++)
    {
        int j = i;
    }
    
}


int
getDeerGrowth()
{
    int nextNumDeer = NowNumDeer;
    float height = NowHeight;
    if(nextNumDeer > height)
        nextNumDeer--;
    else if(nextNumDeer < height)
        nextNumDeer++;

    int test;
    for(int i = 0; i < LOOP_NUM; i++)
    {
        int j = i;
    }
    
    return nextNumDeer;
}

void
getTempPrecip()
{
    unsigned int seed = NowYear;
    float ang = (  30.0*(float)NowMonth + 15.0  ) * ( M_PI / 180.0 );
    
    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    NowTemp = temp + Ranf( -RANDOM_TEMP, RANDOM_TEMP, &seed );
    
    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( -RANDOM_PRECIP, RANDOM_PRECIP, &seed );
    
    if( NowPrecip < 0.0 )
        NowPrecip = 0.0;
}

float
Ranf( float low, float high, unsigned int* seed )
{
    float r = (float) rand_r(seed);      // 0 - RAND_MAX
    return( low + r * ( high - low ) / (float)RAND_MAX );
}

void
printState()
{
    
    if (NowYear == 2014 && NowMonth == 0) {
        cout << "\tYEAR\tMON\tTEMP\tPRECIP\tGRAIN\tDEER\tPESTS" << "\n";
    }
    int index = (NowYear % 2013);
    int step = index + NowMonth + (13 * (index-1));
    cout << step << "\t";
    cout << NowYear << "\t";
    cout << NowMonth+1 << "\t";
    cout << fixed << setprecision(4) << (5.0/9.0)*(NowTemp-32) << "\t";
    cout << (NowPrecip*2.54) << "\t";
    cout << (NowHeight*2.54) << "\t";
    cout << NowNumDeer << "\t";
    cout << (int)(PestsRate*100) << "\n";
    
}
