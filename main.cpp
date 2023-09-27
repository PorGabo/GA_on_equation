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
const int gens = 10000;

struct generation_node
{
    char old_binary_values[t][bits];
    int partner[t];
    int crossing_position[t];
    char new_binary_values[t][bits];
    int population[t];
    int population2[t];

    double function[t];
    double selected[t];
    double spected[t];
    double actual[t];

    double sumation;
    double average;
    double maximum;

    int next_population[t]; //

    int checked[t];

    generation_node()
	{
		random_device rd;
		mt19937 gen(rd());

		
		for (int i = 0; i < t; i++)
		{
		    partner[i] = -1; 
		    crossing_position[i] = 0; 
		    for (int j = 0; j < bits; j++)
		    {
		        old_binary_values[i][j] = '0';
		        new_binary_values[i][j] = '0';
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
                old_binary_values[i][j] = ((tmp_population[i] >> j) & 1) ? '1' : '0';

            partner[i] = -1; 
            crossing_position[i] = -1; 
        }


		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> dist(1, 6);

		vector<int> indices(t);
		iota(indices.begin(), indices.end(), 0);
		shuffle(indices.begin(), indices.end(), gen);

		for (int i = 0; i < t; i += 2)
		{
		    partner[indices[i]] = indices[(i + 1) % t];
		    partner[indices[(i + 1) % t]] = indices[i];

		    crossing_position[indices[i]] = dist(gen);
		    crossing_position[indices[(i + 1) % t]] = crossing_position[indices[i]];
		}


        
        for (int i = 0; i < t; i++)
            checked[i] = 0;

        thread threads[t];
		for (int i = 0; i < t; i++)
			threads[i] = thread(&generation_node::perform_crossover, this, i);

		for (int i = 0; i < t; i++)
			threads[i].join();
        
        for (int i = 0; i < t; i++) 
        {
		    string binary_str(new_binary_values[i], bits); //cadena binaria a partir del array de caract
		    population[i] = stoi(binary_str, nullptr, 2); //cadena binaria a entero
    	}
        

        
		process();
        adjust_actual();
        calculate_next_population();
		
    }

    void print_generation()
    {
        cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "|  Old Popul. | Partner | Pos |  New Pop  | Population |  Function  |   Selected   |  Spected  |  Actual  |  Next Pop  |" << endl;
        cout << "------------------------------------------------------------------------------------------------------------------------" << endl;

        for (int i = 0; i < t; i++)
        {
            cout << "|  ";
            for (int j = 0; j < bits; j++)
                cout << old_binary_values[i][j];
            cout << "   |";
            cout << "    " << partner[i] +1<< "    |";
            cout << "  " << crossing_position[i] << "  |";
            cout << " ";
            for (int j = 0; j < bits; j++)
                cout << new_binary_values[i][j];
            cout << "  |    " << population[i];
            cout << "     |    " << function[i];
            cout << "    |   " << selected[i];
            cout << "   | " << spected[i];
			cout << " |     " << actual[i];
			cout << "     |   " << next_population[i];
            
            cout << "    |\n";

            cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
        }
        cout << "Sumation: " << sumation << "\n";
        cout << "Maximum: " << maximum << "\n";
        cout << "Average: " << average << "\n\n";
    }

    void calculate(int index)
    {
        // Calcular selected, spected y actual
        selected[index] = function[index] / sumation;
        spected[index] = function[index] / average;
        actual[index] = static_cast<int>(round(spected[index]));
    }

    void process()
    {
    	for (int i = 0; i < t; i++)
    	{
            population[i] = rand() % 256;
            selected[i] = 0.0;
            spected[i] = 0.0;
            actual[i] = 0;
        }

        sumation = 0.0;
        maximum = function[0]; 

        for (int i = 0; i < t; i++) 
        {
            // EQUATION HERE!
            int x = population[i];
            function[i] = pow(x, 2) + 3*x; // x**2 + 3x
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

	void perform_crossover(int i)
	{
		mtx.lock();////// no checked problems
		if (checked[i] || checked[partner[i]])
		{
		    mtx.unlock();
		    return;
		}

		checked[i] = 1;
		checked[partner[i]] = 1;
		mtx.unlock();//////

		int partnerIndex = partner[i];

		// Copy the values from old_binary_values to new_binary_values
		for (int j = 0; j < bits; j++)
		{
		    new_binary_values[i][j] = old_binary_values[i][j];
		    new_binary_values[partnerIndex][j] = old_binary_values[partnerIndex][j];
		}

		// Perform crossover
		for (int j = crossing_position[i]; j < bits; j++)
		{
		    char temp = new_binary_values[i][j];
		    new_binary_values[i][j] = new_binary_values[partnerIndex][j];
		    new_binary_values[partnerIndex][j] = temp;
		}
	}


    void calculate_next_population()
    {
        int next_index = 0;
        for (int i = 0; i < t; i++) 
            for (int j = 0; j < actual[i]; j++) 
                next_population[next_index++] = population[i];    
    }

    void adjust_actual()
    {
        double current_sum = 0.0;

        for (int i = 0; i < t; i++)
            current_sum += actual[i];

        if (current_sum == t)
            return; // No es necesario ajustar nada

        if (current_sum < t)
        {
            // Si la suma es menor que t, aumentar los valores de actual de las mejores candidatas
            for (int i = 0; i < t && current_sum < t; i++)
            {
                if (actual[i] == 0)
                {
                    actual[i] = 1;
                    current_sum++;
                }
            }
        }
        else
        {
            // Si la suma es mayor que t, disminuir los valores de actual de las peores candidatas
            for (int i = t - 1; i >= 0 && current_sum > t; i--)
            {
                if (actual[i] == 1)
                {
                    actual[i] = 0;
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
		    generations.push_back(generation_node(generations[i - 1].next_population));
	}
};


int main()
{
    queue_of_life queue(gens);

    for (int i = 0; i < gens; i++)
    {
        cout << "Generation " << i + 1 << ":\n";
        queue.generations[i].print_generation();
        cout << "\n";
    }

    return 0;
}




