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

const int t = 10; //par
const int bits = 8;
const int gens = 20000;

struct generation_node
{
    char old_x_bin[t][bits];
    int partner_x[t];
    int cross_pos_x[t];
    char new_x_bin[t][bits];
    int x[t];
    int y[t];

    double function_x[t];
    double selected_x[t];
    double spected_x[t];
    double actual_x[t];

    double sumation_x;
    double average_x;
    double maximum_x;

    int next_population_x[t]; //

    int checked_x[t];

    generation_node()
	{
		random_device rd;
		mt19937 gen(rd());

		
		for (int i = 0; i < t; i++)
		{
		    partner_x[i] = -1; 
		    cross_pos_x[i] = 0; 
		    for (int j = 0; j < bits; j++)
		    {
		        old_x_bin[i][j] = '0';
		        new_x_bin[i][j] = '0';
		    }
		}

		process();
		adjust_actual();
		calculate_next_population();
	}


    generation_node(int tmp_population[t])
    {
        for (int i = 0; i < t; i++)
        {
            for (int j = 0; j < bits; j++)
                old_x_bin[i][j] = ((tmp_population[i] >> j) & 1) ? '1' : '0';

            partner_x[i] = -1; 
            cross_pos_x[i] = -1; 
        }


		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> dist(1, 6);

		vector<int> indices(t);
		iota(indices.begin(), indices.end(), 0);
		shuffle(indices.begin(), indices.end(), gen);

		for (int i = 0; i < t; i += 2)
		{
		    partner_x[indices[i]] = indices[(i + 1) % t];
		    partner_x[indices[(i + 1) % t]] = indices[i];

		    cross_pos_x[indices[i]] = dist(gen);
		    cross_pos_x[indices[(i + 1) % t]] = cross_pos_x[indices[i]];
		}


        
        for (int i = 0; i < t; i++)
            checked_x[i] = 0;

        thread threads[t];
		for (int i = 0; i < t; i++)
			threads[i] = thread(&generation_node::perform_crossover, this, i);

		for (int i = 0; i < t; i++)
			threads[i].join();
        
        for (int i = 0; i < t; i++) 
        {
		    string binary_str(new_x_bin[i], bits); //cadena binaria a partir del array de caract
		    x[i] = stoi(binary_str, nullptr, 2); //cadena binaria a entero
    	}
        

        
		process();
        adjust_actual();
        calculate_next_population();
		
    }

    void print_generation()
    {
        cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "|  Old Popul. | partner_x | Pos |  New Pop  |  x  |  function_x  |   Selected   |  Spected  |  Actual  |  Next Pop  |" << endl;
        cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

        for (int i = 0; i < t; i++)
        {
            cout << "|  ";
            for (int j = 0; j < bits; j++)
                cout << old_x_bin[i][j];
            cout << "   |";
            cout << "    " << partner_x[i] +1<< "    |";
            cout << "  " << cross_pos_x[i] << "  |";
            cout << " ";
            for (int j = 0; j < bits; j++)
                cout << new_x_bin[i][j];
            cout << "  |    " << x[i];
            cout << "     |    " << function_x[i];
            cout << "    |   " << selected_x[i];
            cout << "   | " << spected_x[i];
			cout << " |     " << actual_x[i];
			cout << "     |   " << next_population_x[i];
            
            cout << "    |\n";

            cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
        }
        cout << "sumation_x: " << sumation_x << "\n";
        cout << "maximum_x: " << maximum_x << "\n";
        cout << "average_x: " << average_x << "\n\n";
    }

    void calculate(int index)
    {
        // Calcular selected, spected y actual
        selected_x[index] = function_x[index] / sumation_x;
        spected_x[index] = function_x[index] / average_x;
        actual_x[index] = static_cast<int>(round(spected_x[index]));
    }

    void process()
    {
    	for (int i = 0; i < t; i++)
    	{
            x[i] = rand() % 256;
            selected_x[i] = 0.0;
            spected_x[i] = 0.0;
            actual_x[i] = 0;
        }

        sumation_x = 0.0;
        maximum_x = function_x[0]; 

        for (int i = 0; i < t; i++) 
        {
            // EQUATION HERE!
            int a = x[i];
            function_x[i] = pow(a, 2) + 3*a; // x**2 + 3x
            sumation_x += function_x[i];

            if (function_x[i] > maximum_x)
                maximum_x = function_x[i];
        }

        average_x = sumation_x / t;

        thread threads[t];
        for (int i = 0; i < t; i++)
            threads[i] = thread(&generation_node::calculate, this, i);
        for (int i = 0; i < t; i++)
            threads[i].join();
    }

	void perform_crossover(int i)
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

		int partnerIndex = partner_x[i];

		// Copy the values from old_x_bin to new_x_bin
		for (int j = 0; j < bits; j++)
		{
		    new_x_bin[i][j] = old_x_bin[i][j];
		    new_x_bin[partnerIndex][j] = old_x_bin[partnerIndex][j];
		}

		// Perform crossover
		for (int j = cross_pos_x[i]; j < bits; j++)
		{
		    char temp = new_x_bin[i][j];
		    new_x_bin[i][j] = new_x_bin[partnerIndex][j];
		    new_x_bin[partnerIndex][j] = temp;
		}
	}


    void calculate_next_population()
    {
        int next_index = 0;
        for (int i = 0; i < t; i++) 
            for (int j = 0; j < actual_x[i]; j++) 
                next_population_x[next_index++] = x[i];    
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
		    generations.push_back(generation_node(generations[i - 1].next_population_x));
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




