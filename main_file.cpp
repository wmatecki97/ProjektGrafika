/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"

using namespace glm;

float aspect=1;
float speed=0; //prędkość kątowa obrotu w radianach na sekundę

//Framebuffer size change event processing
void windowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); //Generate images in this resolution
	aspect=(float)width/(float)height; //Compute aspect ratio of width to height of the window
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void speed_callback(GLFWwindow* window, int key,
int scancode, int action, int mods){
        if (key == GLFW_KEY_LEFT)
            speed=3.14;
        if (key == GLFW_KEY_RIGHT)
            speed=-3.14;
        if(action==GLFW_RELEASE)
                speed=0.0;


}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************

	glfwSetFramebufferSizeCallback(window, windowResize);//Zarejestruj procedurę obsługi zmiany rozmiaru ekranu.

    glClearColor(0,0,0,1); //Ustaw kolor czyszczenia bufora kolorów na czarno
   // glEnable(GL_LIGHTING); //Włącz cieniowanie
    glEnable(GL_LIGHT0); //Włącz świało numer 0
    glEnable(GL_DEPTH_TEST); //Włącz bufor głębokości
    glEnable(GL_COLOR_MATERIAL); //Włącz ustawianie koloru materiału przez polecenia glColor
}




//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window,float angle) {
	//************Tutaj umieszczaj kod rysujący obraz******************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor kolorów i bufor głębokości

	//Przygotuj macierze rzutowania i widoku dla renderowanego obrazu
	mat4 P=perspective(50.0f*PI/180.0f, aspect, 1.0f, 50.0f); //Wylicz macierz rzutowania

	mat4 V=lookAt( //Wylicz macierz widoku
	vec3(0.0f,0.0f,-15.0f),
	vec3(0.0f,0.0f,0.0f),
	vec3(0.0f,1.0f,0.0f));

	glMatrixMode(GL_PROJECTION); //Włącz tryb modyfikacji macierzy rzutowania
	glLoadMatrixf(value_ptr(P)); //Skopiuj macierz rzutowania
	glMatrixMode(GL_MODELVIEW); //Włącz tryb modyfikacji macierzy model-widok. UWAGA! Macierz ta będzie ładowana przed narysowaniem każdego modelu



    //Rysowanie pojedynczego modelu
    //1. Oblicz i załaduj macierz modelu
	mat4 M=mat4(1.0f); //Wylicz macierz modelu
	M =translate(M, vec3(-1.0f, 0.0f, 0.0f));
	M=rotate(M,angle,vec3(0.0f,1.0f,0.0f));
	glLoadMatrixf(value_ptr(V*M)); //Załaduj macierz model-widok
/*	//2. Narysuj obiekt
	glColor3d(0,1,0);
	Models::torus.drawSolid();
    //Stop

    //drugi item
    M=mat4(1.0f); //Wylicz macierz modelu
	M =translate(M, vec3(1.0f, 0.0f, 0.0f));
	M=rotate(M,-angle,vec3(0.0f,0.0f,1.0f));
	glLoadMatrixf(value_ptr(V*M)); //Załaduj macierz model-widok
	//2. Narysuj obiekt
	glColor3d(1,1,0);
	Models::torus.drawSolid();
//stop
*/
float geomVertices[]={
   0,4.08,0, 0,0,2.88, -2.5,0,-4.44,
   0,4.08,0, 0,0,2.88, 2.5,0,-1.44,
 //  0,4.08,0, 2.5,0,-1.44,-2.5,0,-1.44,
 //  2.5,0,-1.44, -2.5,0,-1.44, 0,0,2.88
};

float geomColors[]={
   1,0,0, 1,0,1, 1,1,1,
   1,1,0, 0,1,1, 0,1,0,
 //  0,0,1, 0,0,1, 0,0,1,
 //  1,1,0, 1,1,0, 1,1,0
};

int geomVertexCount=12;
glEnableClientState( GL_VERTEX_ARRAY );
glEnableClientState( GL_COLOR_ARRAY );
glVertexPointer( 3, GL_FLOAT, 0, geomVertices);
glColorPointer( 3, GL_FLOAT, 0, geomColors);
glDrawArrays(GL_TRIANGLES,0, geomVertexCount-6);
glDisableClientState( GL_VERTEX_ARRAY );
glDisableClientState( GL_COLOR_ARRAY );
	//Koniec
	glfwSwapBuffers(window);

}

int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów



	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	glfwSetKeyCallback(window, speed_callback);

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	float angle=0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Wyzeruj licznik czasu
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
	    angle+=speed*glfwGetTime(); //Powiększ kąt obrotu o szybkość obrotu razy czas, który minął od poprzedniej klatki
	    glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window,angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
