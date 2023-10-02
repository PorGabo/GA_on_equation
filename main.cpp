
#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <thread>

using namespace std;

const int t = 10; //par
const int gens = 100;

const int xbits = 6;
const int ybits = 8;

const int xlimit = pow(2,xbits);
const int ylimit = pow(2,ybits);

#include "generation.h"
#include "life.h"

int main()
{
    srand(time(0));
    life a(gens);

    for (int i = 0; i < gens; i++)
    {
        cout << "Generation " << i + 1 << ":\n";
        a.generations[i].print();
        cout << "\n";
    }

    return 0;
}







