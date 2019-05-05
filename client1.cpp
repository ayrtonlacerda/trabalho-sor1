// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8888

int main(int argc, char const *argv[])
{
  int filled = 1;

  struct sockaddr_in address;
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
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
      // enviar mensagem pedindo um job --> hello = "GET_JOB"
      if (filled)
      {
        send(sock, hello, strlen(hello), 0);
        printf("Msg enviada do client1:\n--%s\n\n", hello);
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