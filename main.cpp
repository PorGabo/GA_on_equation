#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <thread>

using namespace std;

const int t = 4; //debe ser par
const int bits = 8;

struct generation_node
{
    char old_binary_values[t][bits];
    int partner[t];
    int crossing_position[t];
    char new_binary_values[t][bits];
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

    generation_node()
    {
        srand(static_cast<unsigned>(time(nullptr)));

        for (int i = 0; i < t; i++) {
            population[i] = rand() % 256;
            selected[i] = 0.0; // Inicializa selected con 0
            spected[i] = 0.0; // Inicializa spected con 0
            actual[i] = 0; // Inicializa actual con 0
        }

        sumation = 0.0;
        maximum = function[0]; // Inicializar el máximo con el primer valor de la función

        for (int i = 0; i < t; i++) {
            // EQUATION HERE!
            function[i] = pow(population[i], 2); // x**2
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
            for (int j = 0; j < bits; j++)
            {
                old_binary_values[i][j] = ((tmp_population[i] >> j) & 1) ? '1' : '0';
            }

            // Inicializa 'partner' y 'crossing_position'
            partner[i] = -1; // Inicialmente no tiene pareja
            crossing_position[i] = -1; // Posición de cruce inicial
        }

        // Inicializa la semilla de generación de números aleatorios
        srand(static_cast<unsigned>(time(nullptr)));

        // ...

        // Asigna parejas de cruzamiento y posiciones de cruce
        for (int i = 0; i < t; i += 2)
        {
            partner[i] = (i + 1) % t;
            partner[(i + 1) % t] = i;

            // Genera una posición de cruce aleatoria entre 1 y 7
            crossing_position[i] = rand() % 6 + 1; // Valores del 1 al 7
            crossing_position[(i + 1) % t] = crossing_position[i];
        }

        int checked[t];
        for (int i = 0; i < t; i++)
            checked[i] = 0;

        for (int i = 0; i < t; i++)
        {
            if (checked[i] || checked[partner[i]])
                continue;

            checked[i] = 1;
            checked[partner[i]] = 1;

            int partnerIndex = partner[i];

            // Copia los valores de old_binary_values a new_binary_values
            for (int j = 0; j < bits; j++)
            {
                new_binary_values[i][j] = old_binary_values[i][j];
                new_binary_values[partnerIndex][j] = old_binary_values[partnerIndex][j];
            }

            for (int j = crossing_position[i]; j < bits; j++)
            {
                char temp = new_binary_values[i][j];
                new_binary_values[i][j] = new_binary_values[partnerIndex][j];
                new_binary_values[partnerIndex][j] = temp;
            }
        }

        print_binary_and_partners();
        // Resto del código...
    }

    void print_binary_and_partners()
    {
        cout << "---------------------------------------------------------------------" << endl;
        cout << "|  Old Popul. | Partner | Pos |    New Pop   |" << endl;
        cout << "---------------------------------------------------------------------" << endl;

        for (int i = 0; i < t; i++)
        {
            cout << "|  ";
            for (int j = 0; j < bits; j++) {
                cout << old_binary_values[i][j];
            }
            cout << "   |";
            cout << "    " << partner[i] +1<< "    |";
            cout << "  " << crossing_position[i] << "  |";
            cout << "    ";
            for (int j = 0; j < bits; j++) {
                cout << new_binary_values[i][j];
            }
            cout << "    |\n";

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

    void adjust_actual()
    {
        double current_sum = 0.0;

        for (int i = 0; i < t; i++)
        {
            current_sum += actual[i];
        }

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
};

int main()
{
    generation_node node;
    node.print_generation();

    cout << "\n\n";

    generation_node node2(node.next_population); // Utiliza node.next_population como argumento

    return 0;
}
