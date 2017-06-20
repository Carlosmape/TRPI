#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>	
#include <sys/socket.h> //socket para comunicación wifi
#include <stdlib.h>

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
			pacaSocket = socket(AF_INET,SOCK_STREAM,0);
		  connect(pacaSocket, (struct sockaddr *)&paca, sizeof(paca));
		}
		int sendPaca(char* mensaje){ //envía el mensaje a PACA
			printf("Enviando a paca");
			if(write(pacaSocket,mensaje,strlen(mensaje))){
				return 0;
			}
			else
				return -1;
		}
		int recivePaca(){
			printf("Recibiendo de paca");
			char buffer[1024];
			int bytes = recv(pacaSocket, buffer, 1024, 0);
			printf("%d bytes: %d\n", bytes, atoi(buffer));
			return atoi(buffer);
		}
		
	
};
