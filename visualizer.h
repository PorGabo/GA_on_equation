class GeneticVisualizer {
private:
    std::vector<generation> &generations;
    int windowWidth;
    int windowHeight;
    float barWidth;
    float maxFunctionValue;

public:
    GeneticVisualizer(std::vector<generation> &gens) : generations(gens) {
        // Inicializar GLUT aquí
        int argc = 1;
        char *argv[1] = {(char*)"GeneticVisualizer"};
        glutInit(&argc, argv);

        windowWidth = glutGet(GLUT_SCREEN_WIDTH) * 0.8;  // 80% del ancho de la pantalla
        windowHeight = glutGet(GLUT_SCREEN_HEIGHT) * 0.8;  // 80% del alto de la pantalla
        barWidth = static_cast<float>(windowWidth) / gens.size();

        maxFunctionValue = pow(xlimit - 1, 2) + 2 * (xlimit - 1) * (ylimit - 1) + pow(ylimit - 1, 2);
    }


    static void displayCallback() {
        glClear(GL_COLOR_BUFFER_BIT);
        GeneticVisualizer *visualizer = static_cast<GeneticVisualizer *>(glutGetWindowData());
        visualizer->draw();
        glutSwapBuffers();
    }

    void draw() {
        glPointSize(5.0);

        float margin = 50.0f;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(-margin, windowWidth + margin, -margin, windowHeight + margin);

        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        // Eje X
        glVertex2f(0, 0);
        glVertex2f(windowWidth, 0);
        // Eje Y
        glVertex2f(0, 0);
        glVertex2f(0, windowHeight);
        glEnd();

        // Etiquetas para los ejes
        void *font = GLUT_BITMAP_HELVETICA_12;
        const char *xLabel = "Generations";
        const char *yLabel = "f(a, b)";
        glColor3f(0.0, 0.0, 0.0);
        glRasterPos2f(windowWidth / 2, -margin / 1.5);
        for (int i = 0; xLabel[i] != '\0'; i++) {
            glutBitmapCharacter(font, xLabel[i]);
        }
        glRasterPos2f(-margin / 1.5, windowHeight / 2);
        for (int i = 0; yLabel[i] != '\0'; i++) {
            glutBitmapCharacter(font, yLabel[i]);
        }

        // Etiquetas para el eje Y
        int segments = 10;
        float segmentHeight = static_cast<float>(windowHeight) / segments;
        for (int i = 0; i <= segments; i++) {
            float yValue = (maxFunctionValue / segments) * i;
            float yPosition = segmentHeight * i;
            std::string label = std::to_string(static_cast<int>(yValue));

            glColor3f(0.0, 0.0, 0.0); // Color negro para el texto
            glRasterPos2f(-40, yPosition - 5); // Ajustar la posición según el tamaño de la etiqueta
            for (char c : label) {
                glutBitmapCharacter(font, c);
            }
        }

        glColor3f(0.5, 0.5, 0.5); // Color gris para las líneas de separación
        for (size_t i = 1; i < generations.size(); i++) {
            float x = i * barWidth;
            glBegin(GL_LINES);
            glVertex2f(x, 0);
            glVertex2f(x, windowHeight);
            glEnd();
        }
        // Dibujar puntos
            for (size_t i = 0; i < generations.size(); i++) {
                float x = i * barWidth + barWidth / 2; // Centramos los puntos en el medio de cada barra

                for (int j = 0; j < t; j++) {
                    float y = (static_cast<float>(generations[i].function[j]) / maxFunctionValue) * windowHeight;

                    if (generations[i].function[j] == generations[i].maximum) {
                        glColor3f(1.0, 0.0, 0.0); // Color rojo para el valor máximo
                    } else {
                        glColor3f(0.0, 0.0, 1.0); // Color azul para otros valores
                    }

                    glBegin(GL_POINTS);
                    glVertex2f(x, y);
                    glEnd();
                }
            }
        }

    void run() {
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
        glutInitWindowSize(windowWidth, windowHeight);
        glutCreateWindow("Genetic Algorithm Visualization");
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Fondo blanco
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, windowWidth, 0, windowHeight);  // Establecer una proyección ortográfica
        glutDisplayFunc(displayCallback);
        glutSetWindowData(this);
        glutMainLoop();
    }
};
