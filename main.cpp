#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "interfazPaca.hpp"
#include "trpi.hpp"

using namespace cv;
using namespace std;

const char* IPADRESS = "192.168.4.1"; //Indica la ip del receptor del mensaje (puerto 8888)

const int H_MIN = 0;
const int H_MAX = 16;
const int S_MIN = 170;		//indica el rango de colores HSV 
const int S_MAX = 256;		//que TRPI interpretará como guía
const int V_MIN = 233;
const int V_MAX = 256;


int main(int argc, char** argv) {
	cout<<"Initializing"<<endl;
	char* orden; //almacena las ordenes a enviar a PACA
	//definimos los valores de color HSV para reconocer al guía (Pelota ping pong naranja)
	clock_t timeAnt = clock()/10000;
	clock_t timeAct = clock()/10000;
	struct HSVrange guia;
	guia.H_MIN = H_MIN;
	guia.H_MAX = H_MAX;
	guia.S_MIN = S_MIN;
	guia.S_MAX = S_MAX;
	guia.V_MIN = V_MIN;
	guia.V_MAX = V_MAX;
	
	cout<<"Creating objects..."<<endl;
	cout<<"- Creating PacaConection"<<endl; PacaConection paca(IPADRESS);
	cout<<"- Creating TRPI raspberry"<<endl; Trpi raspberry(guia);
	cout<<"Done."<<endl;
	
	while (1){ //bucle principal
		/*timeAct = clock()/10000;
		if((timeAct-timeAnt) > 5){ //comprobamos que han pasado almenos 5 milisegundos para no ahogar a la raspberry
			*/
			string strorden = raspberry.seguirGuia(); //capturamos la orden de TRPI a enviar
			
			if (strorden != ""){
				orden =new char[strorden.size() + 1];			//la guardamos en un char* para enviarla
				strcpy(orden, strorden.c_str());

				if(paca.sendPaca(orden) == 0)					//enviamos la posición del joystick
					cout<<"Orden enviada"<<endl;
				else
					cout<<"x ERROR: sendPaca"<<endl;
			}
			delete orden;
			sleep(1);
			/*timeAnt=timeAct;
		}*/
	}
}
