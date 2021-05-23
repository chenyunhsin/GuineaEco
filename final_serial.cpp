#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>
#include <omp.h>
#include <ctime>

#define SERIAL

using namespace std;

const float TIMOTHY_GROWS_PER_MONTH =     8.0;    //inches
const float ONE_GUINEA_EATS_PER_MONTH =   0.5;

const float AVG_PRECIP_PER_MONTH =      6.0;
const float AMP_PRECIP_PER_MONTH =      6.0;
const float RANDOM_PRECIP =             2.0;    //inches

const float AVG_TEMP =                  50.0;
const float AMP_TEMP =                  20.0;
const float RANDOM_TEMP =               10.0;   //Farhrenheit

const float MIDTEMP =                   40.0;
const float MIDPRECIP =                 10.0;

const int LOOP_NUM =                    1e8;
const int TARGET_YEAR= 2022;
int  NowYear;           // 2014 - 2021
int  NowMonth;          // 0 - 11

float NowPrecip;        // inches of rain per month
float NowTemp;          // temperature this month
float NowHeight;        // grain height in inches
int  NowNumGuinea;        // current guinea population
float PestsRate;


void TimothyGuinea();
void Timothy();
void Watcher();
void Pests();

void incMonth();
void printState();
void getTempPrecip();
float getTimothyHeight();
int getGuineaGrowth();
float getPestsDamage();

float Ranf(float, float, unsigned int*);



int
main(int argc, char* argv[])
{
    time_t begin, end;
    begin = time(NULL);
    cout << begin << endl;

    //initial values
    NowNumGuinea = 6;
    NowHeight =  1.;
    NowMonth =    0;
    NowYear  = 2021;
    PestsRate = 0.;

    
    #ifndef SERIAL
    getTempPrecip();

    omp_set_num_threads(4);
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            TimothyGuinea();
        }
        
        #pragma omp section
        {
            Timothy();
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

    while(NowYear <=  TARGET_YEAR)
    {
        int nextNumDeer = getGuineaGrowth();
        NowNumGuinea = nextNumDeer;
        float nextTimothyHeight = getTimothyHeight();
        NowHeight = nextTimothyHeight;
        float nextPestRate = getPestsDamage();
        PestsRate = nextPestRate;
        printState();
        incMonth();
        getTempPrecip();

    }

    #endif

    end = time(NULL);
    cout << end << endl;
    //cout << "Elapse time: " << float(end - begin) / CLOCKS_PER_SEC << endl;
    cout << "Elapse time: " << (end - begin) << endl;

    return 0;
}

void
Watcher()
{
    while( NowYear <= TARGET_YEAR )
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
TimothyGuinea()
{
    while( NowYear <= 2022 )
    {
        int thread;
        #ifndef SERIAL
        thread = omp_get_thread_num();
        #endif

        int nextNumDeer = getGuineaGrowth();
        //printf("Deer: %d\n", thread);

        // DoneComputing barrier:
        #pragma omp barrier
        
        NowNumGuinea = nextNumDeer;
        
        // DoneAssigning barrier:
        #pragma omp barrier
        
        
        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void
Timothy()
{
    while( NowYear <= 2022 )
    {
        int thread;
        #ifndef SERIAL
        thread = omp_get_thread_num();
        #endif

        float nextTimothyHeight = getTimothyHeight();
        //printf("Timothy: %d\n", thread);

        // DoneComputing barrier:
        #pragma omp barrier
        
        NowHeight = nextTimothyHeight;
        
        // DoneAssigning barrier:
        #pragma omp barrier
        
        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void
Pests()
{
    while( NowYear <= 2022 )
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
getTimothyHeight()
{
    float nextTimothyHeight = NowHeight;
    
    float tempFactor = expf(-pow((NowTemp - MIDTEMP)/10, 2));
    float precipFactor = expf(-pow((NowPrecip - MIDPRECIP)/10, 2));
    
    nextTimothyHeight += tempFactor * precipFactor * TIMOTHY_GROWS_PER_MONTH;
    nextTimothyHeight -= (float)NowNumGuinea * ONE_GUINEA_EATS_PER_MONTH;
    nextTimothyHeight *= (1 - PestsRate);
    
    if (nextTimothyHeight < 0.0) nextTimothyHeight = 0.0;
    srand(time(NULL));
    int x;
    x=rand();
    for(int i = 0; i < LOOP_NUM; i++)
    {
        x%=2;
    }
    
    return nextTimothyHeight;
}

float
getPestsDamage()
{
    int test;
    float r = (float)rand();
    for(int i = 0; i < LOOP_NUM; i++)
    {
        test=i/2;
        if(test==0){
                r-=test;
        }else{
                continue;
        }
    }	
    if((NowMonth >=3 && NowMonth <= 8) || NowTemp >= 50)
    {
        r = (float) rand();      // 0 - RAND_MAX
        return  r / (float)RAND_MAX/ 2  + (NowMonth - 2)/12;
    }
    else
        return 0.;
    
}


int
getGuineaGrowth()
{
    int nextNumDeer = NowNumGuinea;
    float height = NowHeight;
    if(nextNumDeer > height)
        nextNumDeer--;
    else if(nextNumDeer < height)
        nextNumDeer++;

    int test;
    for(int i = 0; i < LOOP_NUM; i++)
    {
        test=i/2;
	if(test==0){
		nextNumDeer+=test;
	}else{
		continue;
	}
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
    
    int test;
    for(int i = 0; i < LOOP_NUM; i++)
    {
        test=i/2;
        if(test==0){
                NowTemp+=test;
        }else{
                continue;
        }
    }
    
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
    
   
    cout << "\tYEAR\tMON\tTEMP\tPRECIP\tTIMOTHY\tGUINEA\tPESTS" << "\n";
  
    int index = (NowYear % (TARGET_YEAR-2));
    int step = index + NowMonth + (13 * (index-1));
    cout << step << "\t";
    cout << NowYear << "\t";
    cout << NowMonth+1 << "\t";
    cout << fixed << setprecision(4) << (5.0/9.0)*(NowTemp-32) << "\t";
    cout << (NowPrecip*2.54) << "\t";
    cout << (NowHeight*2.54) << "\t";
    cout << NowNumGuinea << "\t";
    cout << (int)(PestsRate*100) << "\n";
    
}
