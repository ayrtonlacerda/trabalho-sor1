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
  // minha variaveis e constantes
  int testSend = 1;
  int compare = 0;
  char *letter1;
  char *letter2;
  char *letter3;
  char stringCompare[1024] = "JOB";
  int filled = 1;
  // char arrayCompare[];
  // ate aqui

  struct sockaddr_in address;
  int sock = 0, valread;
  struct sockaddr_in serv_addr;

  char *hello = "GET_JOB";
  char *resultJob = "RESULT_JOB";

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
  printf("Mensagem recebida do servidor:\n"); // em tese a msg 'connect'
  printf("---%s\n\n", buffer);

  // recebe mensagem do servidor connec dizendo que a uma comunicação entre os dois
  if (strcmp(buffer, "connect") == 0)
  {
    while (1)
    {
      // enviar mensagem pedindo um job --> hello = "GET_JOB"
      if (1)
      {
        send(sock, hello, strlen(hello), 0);
        printf("Msg que estou enviando:\n--%s\n\n", hello);
        // filled = 0;
      }

      valread = read(sock, buffer, 1024);
      printf("Mensagem recebida do servidor:\n");
      printf("---%s\n\n", buffer);

      if (strcmp(buffer, "JOB") == 0)
      {
        printf("Executa o job: %s\n", resultJob);
        send(sock, resultJob, strlen(resultJob), 0);
        printf("Finish job\n");
      }
      else if (strcmp(buffer, "CLOSE") == 0)
      {
        printf("finaliza conexao");
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

// #########################################################
/*

    if (1)
    {
      for (int i = 0; i < 10000000; i++)
      {
      }
      send(sock, hello, strlen(hello), 0);
      printf("Msg enviada do worker1:\n--%s\n\n", hello);
    }


    if (0)
    {
      send(sock, hello, strlen(hello), 0);
      printf("Hello message sent\n\n");
    }

    valread = read(sock, buffer, 1024);
    printf("%d\n\n", valread);
    printf("%s\n", buffer);

*/