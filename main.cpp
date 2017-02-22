#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

#include "interfazPaca.hpp"
#include "trpi.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	cout<<"Initializing"<<endl;
	char* orden; //almacena las ordenes a enviar a PACA
	//definimos los valores de color HSV para reconocer al guía (Pelota ping pong naranja)
	struct HSVrange guia;
	guia.H_MIN = 0;
	guia.H_MAX = 16;
	guia.S_MIN = 170;
	guia.S_MAX = 256;
	guia.V_MIN = 233;
	guia.V_MAX = 256;
	
	cout<<"Creating objects..."<<endl;
	cout<<"- Creating PacaConection"<<endl;
	PacaConection paca("127.0.0.1");
	
	cout<<"- Creating TRPI raspberry"<<endl;
	Trpi raspberry(guia);
	
	cout<<"Done."<<endl;
	
	while (1){ //bucle de funcionamiento
		string strorden = raspberry.seguirGuia(); //capturamos la orden a enviar
		orden =new char[strorden.size() + 1];			//la guardamos en un char* para enviarla
		strcpy(orden, strorden.c_str());

		//ahora convertimos la salida de Trpi a valores que devolvería un joystick
		if(paca.sendPaca(orden) == 0){
			cout<<"Orden enviada"<<endl;
		}else{
			cout<<"x ERROR: sendPaca no funciona"<<endl;
		}
		delete orden;
	}
}
