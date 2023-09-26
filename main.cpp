#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <bitset>
#include <thread>


using namespace std;

const int t = 4; //debe ser par
const int bits = 8;


struct generation_node
{   
    bitset<bits> old_binary_values[t];
    int partner[t];
    int crossing_position[t];
    bitset<bits> new_binary_values[t];
    int population[t];
    
    double function[t];
    double selected[t];
    double spected[t];
    double actual[t];
    
    double sumation; 
    double average;  
    double maximum;  
    
    int next_population[t]; //
    
    generation_node* next = nullptr;
	
	
	
	
	//1st generation
    generation_node()
    {
        srand(static_cast<unsigned>(time(nullptr)));

        for (int i = 0; i < t; i++)
            population[i] = rand() % 256;


        sumation = 0.0;
        maximum = function[0]; // Inicializar el máximo con el primer valor de la función


        for (int i = 0; i < t; i++)
        {
        	// EQUATION HERE!
            function[i] = pow(population[i], 2); //x**2
            sumation += function[i];
            
            if (function[i] > maximum)
                maximum = function[i];
        }

        average = sumation / t;


        thread threads[t];
        for (int i = 0; i < t; i++) {
            threads[i] = thread(&generation_node::calculate, this, i);
        }
        for (int i = 0; i < t; i++) {
            threads[i].join();
        }
        
        adjust_actual();
        calculate_next_population();
    }


	// 2+ generations
	generation_node(int tmp_population[t])
	{
		for (int i = 0; i < t; i++)
		{
		    old_binary_values[i] = bitset<bits>(tmp_population[i]);

		    // Inicializa 'partner' y 'crossing_position'
		    partner[i] = -1; // Inicialmente no tiene pareja
		    crossing_position[i] = -1; // Posición de cruce inicial
		}

		vector<int> indices(t);
		for (int i = 0; i < t; i++)
		    indices[i] = i;

		// Inicializa la semilla de generación de números aleatorios
		srand(static_cast<unsigned>(time(nullptr)));

		// Baraja los índices aleatoriamente
		for (int i = t - 1; i > 0; i--)
		{
		    int j = rand() % (i + 1);
		    swap(indices[i], indices[j]);
		}

		// Asigna parejas de cruzamiento y posiciones de cruce
		for (int i = 0; i < t; i += 2)
		{
		    int row1 = indices[i];
		    int row2 = indices[i + 1];
		    
		    partner[row1] = row2;
		    partner[row2] = row1;
		    
		    // Genera una posición de cruce aleatoria
		    crossing_position[row1] = rand() % (bits + 1); // Puede ser cualquier posición de bit
		    crossing_position[row2] = crossing_position[row1];
		}

		print_binary_and_partners();
		// Resto del código para generar y mostrar las parejas de cruzamiento...
	}


	void print_binary_and_partners()
	{
		cout<<"---------------------------------------------------------------------"<<endl;
		cout<<"|  Old Popul. | Partner | "<<endl;
		cout<<"---------------------------------------------------------------------"<<endl;

		for (int i = 0; i < t; i++)
		{
		    //int partner_index = partner[i];
		    cout << "|  " << old_binary_values[i] << "   |";	 
	        cout << "    " << partner[i] +1 << "    |" << endl;
	        
		    cout << "--------------------------------------------------------" << endl;
		}
	}




    void calculate(int index)
    {
        // Calcular selected, spected y actual
        selected[index] = function[index] / sumation;
        spected[index] = function[index] / average;
        actual[index] = static_cast<int>(round(spected[index]));
    }

    void print_generation()
    {
        cout << "--------------------------------------------------------------------------------------" << endl;
        cout << "|  Population  |  Function  |    Selected    |    Spected    |  Actual  |  Next Pop  |" << endl;
        cout << "--------------------------------------------------------------------------------------" << endl;

        for (int i = 0; i < t; i++) 
        {
            cout << "|      " << population[i] << "      |    " << function[i] << "    |     ";
            cout << selected[i] << "    |     " << spected[i] << "    |     " << actual[i] << "  |  "<< next_population[i]<< endl;
            cout << "-----------------------------------------------------" << endl;
        }
        cout << "Sumation: " << sumation << "\n";
        cout << "Maximum: " << maximum << "\n";
        cout << "Average: " << average << "\n\n\n";
    }
    
    void calculate_next_population()
	{
		int next_index = 0;
		for (int i = 0; i < t; i++) 
		    for (int j = 0; j < actual[i]; j++) 
		        next_population[next_index++] = population[i];	
	}
	
	void adjust_actual() //si falla
	{
		double current_sum = 0.0;
		int best_candidate = -1;
		int worst_candidate = -1;

		for (int i = 0; i < t; i++)
		{
		    current_sum += actual[i];

		    if (actual[i] == 0 && spected[i] > spected[best_candidate])
		        best_candidate = i;
		    if (actual[i] == 1 && spected[i] < spected[worst_candidate])
		        worst_candidate = i;
		}

		if (current_sum == t)
		    return;

		if (current_sum < t && best_candidate != -1)
		    actual[best_candidate] = 1;
		else if (worst_candidate != -1)
		    actual[worst_candidate] = 0;
	}


	


};

struct queue_of_life
{


};

int main()
{
    generation_node node;
    node.print_generation();
    
    cout<<"\n\n";
    
    generation_node node2(node.next_population); // Utiliza node.next_population como argumento

    return 0;
}


