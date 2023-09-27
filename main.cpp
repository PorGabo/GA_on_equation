#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <thread>
#include <random>
#include <algorithm>
#include <mutex>

using namespace std;
mutex mtx;

const int t = 6; //par
const int gens = 100;

const int bits = 8;
const int bits2 = 8;

const int limit_x = 256;
const int limit_y = 256;

struct generation_node
{
    char old_x_bin[t][bits];
    int partner_x[t];
    int cross_pos_x[t];
    char new_x_bin[t][bits];
    int x[t];

    
    double selected_x[t];
    double spected_x[t];
    double actual_x[t];

    double sumation;
    double average;
    double maximum;

    int next_population_x[t]; //

    int checked_x[t];
    
    
    char old_y_bin[t][bits2];
    int partner_y[t];
    int cross_pos_y[t];
    char new_y_bin[t][bits2];
    int y[t];


    int next_population_y[t]; //

    int checked_y[t];
    
    
    
    
    double function[t];

    generation_node()
	{
		random_device rd;
		mt19937 gen(rd());

		
		for (int i = 0; i < t; i++)
		{
		    partner_x[i] = -1; 
		    partner_y[i] = -1; 
		    
		    cross_pos_x[i] = 0; 
		    cross_pos_y[i] = 0; 
		    for (int j = 0; j < bits; j++)
		    {
		        old_x_bin[i][j] = '0';
		        new_x_bin[i][j] = '0';

		    }
		    for (int j = 0; j < bits2; j++)
		    {
		        old_y_bin[i][j] = '0';
		        new_y_bin[i][j] = '0';
		    }
		}

		process();
		adjust_actual();
		calculate_next_population();
	}


    generation_node(int tmp_population[t], int tmp2_population[t])
    {
        for (int i = 0; i < t; i++)
        {
            for (int j = 0; j < bits; j++)
                old_x_bin[i][j] = ((tmp_population[i] >> j) & 1) ? '1' : '0';
			for (int j = 0; j < bits2; j++)
                old_y_bin[i][j] = ((tmp2_population[i] >> j) & 1) ? '1' : '0';
                
            partner_x[i] = -1; 
            cross_pos_x[i] = -1; 
            partner_y[i] = -1; 
            cross_pos_y[i] = -1; 
        }


		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> dist(1, bits-1);
		uniform_int_distribution<int> dist2(1, bits2-1);
		
		vector<int> indices(t);
		vector<int> indices2(t);
		
		iota(indices.begin(), indices.end(), 0);
		shuffle(indices.begin(), indices.end(), gen);
		iota(indices2.begin(), indices2.end(), 0);
		shuffle(indices2.begin(), indices2.end(), gen);

		for (int i = 0; i < t; i += 2)
		{
		    partner_x[indices[i]] = indices[(i + 1) % t];
		    partner_x[indices[(i + 1) % t]] = indices[i];

		    cross_pos_x[indices[i]] = dist(gen);
		    cross_pos_x[indices[(i + 1) % t]] = cross_pos_x[indices[i]];
		    
		    partner_y[indices2[i]] = indices2[(i + 1) % t];
		    partner_y[indices2[(i + 1) % t]] = indices2[i];

		    cross_pos_y[indices2[i]] = dist2(gen);
		    cross_pos_y[indices2[(i + 1) % t]] = cross_pos_y[indices2[i]];
		}


        
        for (int i = 0; i < t; i++)
            checked_x[i] = checked_y[i] = 0;
            

        thread threadsX[t];
        thread threadsY[t];
        
		for (int i = 0; i < t; i++)
			threadsX[i] = thread(&generation_node::perform_crossoverX, this, i);
			
		for (int i = 0; i < t; i++)
			threadsY[i] = thread(&generation_node::perform_crossoverY, this, i);

		for (int i = 0; i < t; i++)
			threadsX[i].join();

		for (int i = 0; i < t; i++)
			threadsY[i].join();
        
        
        
        
        for (int i = 0; i < t; i++)
		{
			string binary_str(new_x_bin[i], bits); // Cadena binaria a partir del array de caracteres
			x[i] = stoi(binary_str, nullptr, 2); // Convertir cadena binaria a entero en base 2

			string binary_str2(new_y_bin[i], bits2); // Cadena binaria a partir del array de caracteres
			y[i] = stoi(binary_str2, nullptr, 2); // Convertir cadena binaria a entero en base 2
		}


        
        
		process();
        adjust_actual();
        calculate_next_population();
		
    }

    void print_generation()
    {
        cout << "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "|  Old Pop X | Old Pop Y | part x | Part y | PosX | PosY |  New Pop X | New Pop Y |   x   |   y   |  function  | Selected  |   Spected  |  Actual  |  Next Pop X | next pop y  |" << endl;
        cout << "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

        for (int i = 0; i < t; i++)
        {
            cout << "|  ";
            for (int j = 0; j < bits; j++)
                cout << old_x_bin[i][j];
            cout << " |  ";
            for (int j = 0; j < bits2; j++)
                cout << old_y_bin[i][j];
            cout << "   |";
            
            cout << "    " << partner_x[i] +1<< "    |";
            cout << "    " << partner_y[i] +1<< "    |";
            
            cout << "  " << cross_pos_x[i] << "  |";
            cout << "  " << cross_pos_y[i] << "  |";
            
            cout << " ";
            
            for (int j = 0; j < bits; j++)
                cout << new_x_bin[i][j];
                
            cout << "  |  ";
            
            for (int j = 0; j < bits2; j++)
                cout << new_y_bin[i][j];
                
                
            
            cout << "  |  " << x[i];
            cout << "  |   " << y[i];
            
            
            cout << "   |   " << function[i];
            
            
            cout << "   |   " << selected_x[i];

            
            cout << "   | " << spected_x[i];

            
			cout << " |     " << actual_x[i];

			
			cout << "     |   " << next_population_x[i];
			cout << "     |   " << next_population_y[i];
            
            cout << "    |\n";

            cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        }
        cout << "sumation: " << sumation << "\n";
        cout << "maximum: " << maximum << "\n";
        cout << "average: " << average << "\n\n";

    }

    void calculate(int index)
    {
        // Calcular selected, spected y actual
        selected_x[index] = function[index] / sumation;
        spected_x[index] = function[index] / average;
        actual_x[index] = static_cast<int>(round(spected_x[index]));
        
    }

    void process()
    {
    	for (int i = 0; i < t; i++)
    	{
            x[i] = rand() % limit_x;
            y[i] = rand() % limit_y;
            selected_x[i] = 0.0;
            spected_x[i] = 0.0;
            actual_x[i] = 0;
        }

        sumation = 0.0;
        maximum = function[0]; 

        for (int i = 0; i < t; i++) 
        {
            // EQUATION HERE!
            int a = x[i];
            int b = y[i];
            //function_x[i] = pow(a, 2) + 3*a; // x**2 + 3x
            function[i] = pow(a, 2) + 2*a*b + pow(b,2); // x**2 + 3x
            sumation += function[i];
            

            if (function[i] > maximum)
                maximum = function[i];
        }

        average = sumation / t;

        thread threads[t];
        for (int i = 0; i < t; i++)
            threads[i] = thread(&generation_node::calculate, this, i);
        for (int i = 0; i < t; i++)
            threads[i].join();
    }

	void perform_crossoverX(int i)
	{
		mtx.lock();////// no checked_x problems
		if (checked_x[i] || checked_x[partner_x[i]])
		{
		    mtx.unlock();
		    return;
		}

		checked_x[i] = 1;
		checked_x[partner_x[i]] = 1;
		mtx.unlock();//////

		int partnerIndexX = partner_x[i];

		// Copy the values from old_x_bin to new_x_bin
		for (int j = 0; j < bits; j++)
		{
		    new_x_bin[i][j] = old_x_bin[i][j];
		    new_x_bin[partnerIndexX][j] = old_x_bin[partnerIndexX][j];
		}
		

		// Perform crossover
		for (int j = cross_pos_x[i]; j < bits; j++)
		{
		    char temp = new_x_bin[i][j];
		    new_x_bin[i][j] = new_x_bin[partnerIndexX][j];
		    new_x_bin[partnerIndexX][j] = temp;
		}
	}
	
	void perform_crossoverY(int i)
	{
		mtx.lock();////// no checked_x problems
		if (checked_y[i] || checked_y[partner_y[i]])
		{
		    mtx.unlock();
		    return;
		}

		checked_y[i] = 1;
		checked_y[partner_y[i]] = 1;
		mtx.unlock();//////

		int partnerIndexY = partner_y[i];

		// Copy the values from old_x_bin to new_x_bin
		for (int j = 0; j < bits2; j++)
		{
		    new_y_bin[i][j] = old_y_bin[i][j];
		    new_y_bin[partnerIndexY][j] = old_y_bin[partnerIndexY][j];
		}

		// Perform crossover
		for (int j = cross_pos_y[i]; j < bits2; j++)
		{
		    char temp = new_y_bin[i][j];
		    new_y_bin[i][j] = new_y_bin[partnerIndexY][j];
		    new_y_bin[partnerIndexY][j] = temp;
		}
	}


    void calculate_next_population()
    {
        int next_index = 0;
        for (int i = 0; i < t; i++) 
            for (int j = 0; j < actual_x[i]; j++) 
                next_population_x[next_index++] = x[i];    
                
        next_index = 0;
        for (int i = 0; i < t; i++) 
            for (int j = 0; j < actual_x[i]; j++) 
                next_population_y[next_index++] = y[i];    
    }

    void adjust_actual()
    {
        double current_sum = 0.0;
 
        for (int i = 0; i < t; i++)
            current_sum += actual_x[i];

        if (current_sum == t)
            return; // No es necesario ajustar nada

        if (current_sum < t)
        {
            // Si la suma es menor que t, aumentar los valores de actual de las mejores candidatas
            for (int i = 0; i < t && current_sum < t; i++)
            {
                if (actual_x[i] == 0)
                {
                    actual_x[i] = 1;
                    current_sum++;
                }
            }
        }
        else
        {
            // Si la suma es mayor que t, disminuir los valores de actual de las peores candidatas
            for (int i = t - 1; i >= 0 && current_sum > t; i--)
            {
                if (actual_x[i] == 1)
                {
                    actual_x[i] = 0;
                    current_sum--;
                }
            }
        }
    }
    

};

struct queue_of_life
{
    vector<generation_node> generations;

    queue_of_life(int gens)
	{
		generations.push_back(generation_node());

		for (int i = 1; i < gens; i++)
		    generations.push_back(generation_node(generations[i - 1].next_population_x, generations[i - 1].next_population_y));
	}
};


int main()
{
	srand(static_cast<unsigned>(time(nullptr)));
	
    queue_of_life queue(gens);

    for (int i = 0; i < gens; i++)
    {
        cout << "Generation " << i + 1 << ":\n";
        queue.generations[i].print_generation();
        cout << "\n";
    }

    return 0;
}




