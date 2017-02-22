#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>	
#include <sys/socket.h> //socket para comunicación wifi

using namespace std;

class PacaConection{
	private:
		struct sockaddr_in paca; //struct con la información del server
		int pacaSocket;
		int timeAnt;//tiempo de envío del último mensaje
		int timeAct;//tiempo de envío del mensaje actual, se intenta no saturar al mbed
		
	public:
		PacaConection(const char* ipaddress){ //constructor
			//timeAnt = clock();
			paca.sin_family = AF_INET;
			paca.sin_port = htons(8888);
			paca.sin_addr.s_addr = inet_addr(ipaddress);
			if(pacaSocket = socket(AF_INET,SOCK_STREAM,0));
				connect(pacaSocket, (struct sockaddr *)&paca, sizeof(paca)); 
		}
		int sendPaca(char* mensaje){ //envía el mensaje a PACA
			timeAct = clock();
			if(timeAct-timeAnt > 300){ //comprobamos que han pasado almenos 3 milisegundos para no ahogar a PACA
				if (send(pacaSocket,mensaje,strlen(mensaje)+1,MSG_OOB)){
					//continuarEnvioDatos = false;
					//mensajeAnterior = mensaje;
					timeAnt = timeAct;
					return 0;
				}
			}
			return -1;
		}
	
};
