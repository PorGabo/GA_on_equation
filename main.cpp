
#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <thread>

using namespace std;

const int t = 6; //par
const int gens = 100;

const int xbits = 3;
const int ybits = 4;

const int xlimit = 8;
const int ylimit = 16;

#include "generation.h"
#include "life.h"

int main()
{
    srand(time(0));
    generation gen;
    gen.print();

    generation gen2;
    gen2.print();
    return 0;
}







