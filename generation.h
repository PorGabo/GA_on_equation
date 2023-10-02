
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

    int nextgen[t];

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
        
        thread threads[t];
        for (int i = 0; i < t; i++)
            threads[i] = thread(&generation_node::calculate, this, i);
        for (int i = 0; i < t; i++)
            threads[i].join();
    }

    void initializer() //zeros and randoms
    {
        vector<int> indices; //for partner

        for (int i = 0; i < t; i++)
		{
            oldx[i] = oldy[i] = partner[i] = crossposx[i] = crossposy[i] = x[i] = y[i] = function[i] = nextgen[i] = 0;
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
            threads[i] = thread(&generation::calculate, this, i);
        for (int i = 0; i < t; i++)
            threads[i].join();
    }

    void calculate(int i)
    {
        // Calcular selected, spected y actual
        selected[i] = function[i] / sumation;
        spected[i] = function[i] / average;
        actual[i] = static_cast<int>(round(spected[i]));
        
    }
    
    void print()
    {
        cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "|  OldX  |  OldY  |   Old_Xbin   |   Old_Ybin   | Partner | CrossPosX | CrossPosY |   Xbin   |   Ybin   |  X  |  Y  |  Equation  |   Selected   |   Spected   |   Actual   |   NextGen   |" << endl;
        cout << "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;

        for (int i = 0; i < t; i++)
        {
            cout << oldx[i] << " | " << oldy[i] << " | ";
            for (int j = 0; j < xbits; j++)
                cout << old_xbin[i][j];
            cout << " | ";
            for (int j = 0; j < ybits; j++)
                cout << old_ybin[i][j];
            cout << " | ";
            cout << partner[i] << " | " << crossposx[i] << " | " << crossposy[i] << " | ";
            for (int j = 0; j < xbits; j++)
                cout << xbin[i][j];
            cout << " | ";
            for (int j = 0; j < ybits; j++)
                cout << ybin[i][j];
            cout << " | ";
            cout << x[i] << " | " << y[i] << " | " << function[i] << " | " << selected[i] << " | " << spected[i] << " | " << actual[i] << " | " << nextgen[i] << "\n";
            
        }
        cout << "Sumation: " << sumation << "\nAverage: " << average << "\nMaximum: " << maximum << "\n\n";
    }

};