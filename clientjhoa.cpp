#include <stdio.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFSIZE 512
#define PACKETSIZE sizeof(MSG)
using namespace std;
typedef struct MSG
{
	int type;
	int priority;
	int sender;
	int message[BUFSIZE];
}MSG;

void serialize(MSG* msgPacket, int *data);

void printMsg(MSG* msgPacket);

int main(int argc, char const *argv[])
{
  int filled = 1;

  struct sockaddr_in address;
  int sock = 0, valread;
  struct sockaddr_in serv_addr;

	int array[]={10,20,33,44,55};
	MSG* newMsg = new MSG;
	newMsg->type = 1;
	newMsg->priority = 9;
	newMsg->sender = 2;
	for(int i;i<6;i++){
	newMsg->message[i] = array[i];
	}
	int data[PACKETSIZE];
	
	serialize(newMsg, data);
  char *hello = "PROCESS_CLIENT";
  char buffer[1024] = {0};

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }

  //  ler mensagem recebida do servidor apos fazer a conexão
  valread = read(sock, buffer, 1024);
  printf("Mensagem recebida do servidor:\n");
  printf("---%s\n\n", buffer);

  // recebe mensagem do servidor connec dizendo que a uma comunicação entre os dois
  if (strcmp(buffer, "connect") == 0)
  {
    while (1)
    {
      // enviar mensagem pedindo um job --> hello = "PROCESS_CLIENT"
      if (filled)
      {
        send(sock, data, PACKETSIZE, 0);
        //printf("Msg enviada do client1:\n--%s\n\n", hello);
	printMsg(newMsg);
	filled = 0;
      }

      valread = read(sock, buffer, 1024);
      printf("Mensagem recebida do servidor:\n");
      printf("---%s\n\n", buffer);

      if (strcmp(buffer, "CLOSE_PROCESS") == 0)
      {
        printf("Fecha cliente: \n");
        break;
      }
    }
  }
  else
  {
    printf("não houve conexao por msg connect");
  };

  return 0;
}

void printMsg(MSG* msgPacket)
{
	cout << msgPacket->type << endl;
	cout << msgPacket->priority << endl;
	cout << msgPacket->sender << endl;
	cout << msgPacket->message << endl;
}

void serialize(MSG* msgPacket, int *data)
{
	int *q = (int*)data;	
	*q = msgPacket->type;		q++;	
	*q = msgPacket->priority;	q++;	
	*q = msgPacket->sender;		q++;
	
	char *p = (char*)q;
	int i = 0;
	while (i < BUFSIZE)
	{
		*p = msgPacket->message[i];
		p++;
		i++;
	}
}
