
struct generation
{
    int oldx[t];
    int oldy[t];
    int old_xbin[t][xbits];
    int old_ybin[t][ybits];

    int partner[t];
    int crossposx[t];
    int crossposy[t];

    int xbin[t][xbits];
    int ybin[t][ybits];

    int x[t];
    int y[t];

    int function[t];

    double selected[t];
    double spected[t];
    double actual[t];

    int nextx[t];
    int nexty[t];


    double sumation;
    double average;
    double maximum;


    generation()
    {
        initializer();
        for (int i = 0; i < t; i++)
        {
            x[i] = rand() % xlimit;
            y[i] = rand() % ylimit;
        }
        equation();
        chosse();
        adjust_actual();
        calculate_next_population();

    }

    generation(int A[t], int B[t])
    {
        initializer();
        for (int i = 0; i < t; i++)
        {
            oldx[i] = A[i];
            oldy[i] = B[i];
        }

        oldbins();
        crossover();
        convertBinToDecimal();
        equation();
        chosse();
        adjust_actual();
        calculate_next_population();
    }


    void initializer() //zeros and randoms
    {
        vector<int> indices; //for partner

        for (int i = 0; i < t; i++)
		{
            oldx[i] = oldy[i] = partner[i] = crossposx[i] = crossposy[i] = x[i] = y[i] = function[i] = nextx[i] = nexty[i] = 0;
            selected[i] = spected[i] = actual[i] = sumation = average = maximum = 0.0;

		    for (int j = 0; j < xbits; j++)
		    {
		        old_xbin[i][j] = 0;
		        xbin[i][j] = 0;

		    }
		    for (int j = 0; j < ybits; j++)
		    {
		        old_ybin[i][j] = 0;
		        ybin[i][j] = 0;
		    }
            indices.push_back(i);
		}

        //mezclando vector de indices (partner)
        for (int i = 0; i < t; i++)
        {
            int j = rand() % t;
            swap(indices[i], indices[j]);
        }
        //asignando partners y crosspos
        for (int i = 0; i < t; i += 2)
        {
            int pareja1 = indices[i];
            int pareja2 = indices[i + 1];

            partner[pareja1] = pareja2;
            partner[pareja2] = pareja1;

            // Generar una posición de cruce aleatoria
            int cross_posx = rand() % (xbits-1) + 1;
            int cross_posy = rand() % (ybits-1) + 1;

            crossposx[pareja1] = cross_posx;
            crossposx[pareja2] = cross_posx;
            crossposy[pareja1] = cross_posy;
            crossposy[pareja2] = cross_posy;
        }
    }

    void equation() //the equation and also sumation, average and maximum
    {
        for (int i = 0; i < t; i++) 
        {
            // EQUATION HERE!
            int a = x[i];
            int b = y[i];
            function[i] = pow(a, 2) + 2*a*b + pow(b,2);
            sumation += function[i];
            
            if (function[i] > maximum)
                maximum = function[i];

            average = sumation / t;
        }
    }


    void chosse() 
    {
        thread threads[t];
        for (int i = 0; i < t; i++)
            threads[i] = thread(&generation::calculate_chosse, this, i);
        for (int i = 0; i < t; i++)
            threads[i].join();
    }

    void calculate_chosse(int i)
    {
        selected[i] = function[i] / sumation;
        spected[i] = function[i] / average;
        actual[i] = static_cast<int>(round(spected[i]));
        
    }
    
    void adjust_actual()
    {
        double current_sum = 0.0;

        for (int i = 0; i < t; i++)
            current_sum += actual[i];

        if (current_sum == t) //ajusto?
            return;

        if (current_sum < t)
        {
            // Si la suma es menor que t, ajustar los valores de actual basados en las diferencias de la parte decimal
            while (current_sum < t)
            {
                double max_diff = -1.0; // Almacena la mayor diferencia decimal
                int index_to_increase = -1; // Almacena el índice para aumentar

                // Buscar la mayor diferencia decimal
                for (int i = 0; i < t; i++)
                {
                    double diff = spected[i] - static_cast<int>(actual[i]);
                    if (diff > max_diff)
                    {
                        max_diff = diff;
                        index_to_increase = i;
                    }
                }

                if (index_to_increase != -1)
                {
                    actual[index_to_increase] += 1;
                    current_sum += 1;
                }
            }
        }
        else
        {
            // Si la suma es mayor que t, ajustar los valores de actual basados en las diferencias de la parte decimal
            while (current_sum > t)
            {
                double min_diff = 1.0; // Almacena la menor diferencia decimal
                int index_to_decrease = -1; // Almacena el índice para disminuir

                // Buscar la menor diferencia decimal
                for (int i = 0; i < t; i++)
                {
                    double diff = spected[i] - static_cast<int>(actual[i]);
                    if (diff < min_diff)
                    {
                        min_diff = diff;
                        index_to_decrease = i;
                    }
                }

                if (index_to_decrease != -1)
                {
                    actual[index_to_decrease] -= 1;
                    current_sum -= 1;
                }
            }
        }
    }


    void calculate_next_population()
    {
        int next_index = 0;

        for (int i = 0; i < t; i++) 
        {
            for (int j = 0; j < actual[i]; j++) 
            {
                nextx[next_index] = x[i];
                nexty[next_index] = y[i];
                next_index++;
            }
        }
    }

    void oldbins()
    {
        thread x_thread[t];
        thread y_thread[t];

        for (int i = 0; i < t; i++)
        {
            x_thread[i] = thread(&generation::convertToBinary, this, oldx[i], old_xbin[i], xbits);
            y_thread[i] = thread(&generation::convertToBinary, this, oldy[i], old_ybin[i], ybits);
        }

        for (int i = 0; i < t; i++)
        {
            x_thread[i].join();
            y_thread[i].join();
        }
    }

    void convertToBinary(int value, int* binaryArray, int numBits)
    {
        for (int j = 0; j < numBits; j++)
            binaryArray[j] = (value >> (numBits - 1 - j)) & 1;
    }

    void crossover()
    {
        // Copiar old_xbin a xbin
        for (int i = 0; i < t; i++)
        {
            for (int j = 0; j < xbits; j++)
            {
                xbin[i][j] = old_xbin[i][j];
            }
        }

        // Copiar old_ybin a ybin
        for (int i = 0; i < t; i++)
        {
            for (int j = 0; j < ybits; j++)
            {
                ybin[i][j] = old_ybin[i][j];
            }
        }
        
        int checked[t];
        for (int i = 0; i < t; i++)
            checked[i] = 0;
        
        for (int i = 0; i < t; i++)
        {
            if(checked[i] == 0)
            {
                int j = partner[i];
                checked[i] = checked[j] = 1;

                //cruzan la segunda parte de x y la primera de y
                for(int k=crossposx[i]; k<xbits; k++)
                {
                    int tmp = xbin[i][k];
                    xbin[i][k] = xbin[j][k];
                    xbin[j][k] = tmp;
                }
                for(int k=0; k<crossposy[i]; k++)
                {
                    int tmp = ybin[i][k];
                    ybin[i][k] = ybin[j][k];
                    ybin[j][k] = tmp;
                }
            }
        }
    }

    void convertBinToDecimal()
    {
        for (int i = 0; i < t; i++)
        {
            for (int j = xbits-1, k=1; j >= 0; j--, k=k*2)
            {
                if(xbin[i][j]==1)
                    x[i] += k;
            }

            for (int j = ybits-1, k=1; j >= 0; j--, k=k*2)
            {
                if(ybin[i][j]==1)
                    y[i] += k;
            }
        }
    }

    void print()
    {
        cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "| OldX | OldY |   Old_Xbin   |   Old_Ybin   | Partner | CrossPosX | CrossPosY |   Xbin   |   Ybin   |  X  |  Y  | Equation |   Selected   |   Spected   |   Actual   |   NextGen   |" << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

        for (int i = 0; i < t; i++)
        {
            cout << "  " << oldx[i] << "  |  " << oldy[i] << "  ||  ";
            for (int j = 0; j < xbits; j++)
                cout << old_xbin[i][j];
            cout << "  |  ";
            for (int j = 0; j < ybits; j++)
                cout << old_ybin[i][j];
            cout << "  ||   " << partner[i] << "   ||  " << crossposx[i] << "  |  " << crossposy[i] << "  ||  ";
            for (int j = 0; j < xbits; j++)
                cout << xbin[i][j];
            cout << "  |  ";
            for (int j = 0; j < ybits; j++)
                cout << ybin[i][j];
            cout << "  ||  ";
            cout << x[i] << "  |  " << y[i] << "  ||  " << function[i] << "  ||  " << selected[i] << "  |  " << spected[i] << "  |  " << actual[i] << "  ||  " << nextx[i] << "  |  " << nexty[i] << "\n";
            
        }
        cout << "Sumation: " << sumation << "\nAverage: " << average << "\nMaximum: " << maximum << "\n\n";
    }

};
