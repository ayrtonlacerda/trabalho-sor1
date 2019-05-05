//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>    //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE 1
#define FALSE 0
#define PORT 8888

int main(int argc, char *argv[])
{
  int control = 0;
  int opt = TRUE;
  int master_socket, addrlen, new_socket, client_socket[30],
      max_clients = 30, activity, i, valread, sd;
  int max_sd;
  struct sockaddr_in address;

  char buffer[1025]; //data buffer of 1K
  char *word;

  //set of socket descriptors
  fd_set readfds;

  //a message
  char *message = "connect";
  char *sendJob = "JOB";
  char *messageCloseConnect = "CLOSE";
  char *messageCloseConnectClient = "CLOSE_PROCESS";

  //initialise all client_socket[] to 0 so not checked
  for (i = 0; i < max_clients; i++)
  {
    client_socket[i] = 0;
  }

  /*

    Inicializa todos os clientes com 0 (FALSE)
    Máximo de clientes possível = 30

  */

  //create a master socket
  if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  /*

    Cria um novo socket:
      int domain = AF_INET (família de endereços IPv4)
      int type = SOCK_STREAM (comunicação TCP)
      int protocol = 0 (protocolo IP)

    Se houver um erro com a criação do socket:
      Exibe mensagem de erro "socket failed"
      Encerra o programa

  */

  //set master socket to allow multiple connections ,
  //this is just a good habit, it will work without this
  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                 sizeof(opt)) < 0)
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  //type of socket created
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  /*

    Permite que o socket master realize múltiplas conexões
      int level = SOL_SOCKET (itens são procurados dentro do próprio socket)
      int option_name = SO_REUSEADDR (permite que endereços sejam reutilizados)
      const char *option_value = (char *)&opt (aponta para opt = TRUE)
      int option_len = sizeof(opt) (tamanho de opt = 4)

    Se ocorrer um erro com a configuração do socket:
      Exibe mensagem de erro "setsockopt"
      Encerra o programa

  */

  //bind the socket to localhost port 8888
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Listener on port %d \n", PORT);



  /*

    Liga o socket à porta 8888
      const struct sockaddr *addr = (struct sockaddr *)&address (utiliza estrutura padrão address, do tipo sockaddr)
      int addr_len = sizeof(address) (tamanho do endereço)

    Se der certo, printa a PORT em que o servidor está escutando (8888)

  */

  //try to specify maximum of 3 pending connections for the master socket
  if (listen(master_socket, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  /*

    O servidor fica em modo passivo, esperando o cliente requisitar uma conexão
    Limita fila de espera para 3 conexões pendentes
    Se um cliente tentar se conectar e a fila estiver cheia, recebe uma mensgem de erro

  */

  //accept the incoming connection
  addrlen = sizeof(address);
  puts("Waiting for connections ...");

  /*

    readfds é do tipo fd_set
    É o conjunto de sockets a serem monitorados (pelo select)
    FD_ZERO: limpa o readfds
    FD_ISSET: checa se uma porta específica está dentro de readfds
    FD_SET: adiciona uma porta específica à readfds
    FD_CLR: remove uma porta específica de readfds

  */

  while (TRUE)
  {
    //clear the socket set
    FD_ZERO(&readfds);                  // Limpa readfds

    //add master socket to set
    FD_SET(master_socket, &readfds);   // Adiciona master_socket à readfds
    max_sd = master_socket;            // Porta mais alta = porta do master

    //add child sockets to set
    for (i = 0; i < max_clients; i++)
    {
      //socket descriptor
      sd = client_socket[i];

      //if valid socket descriptor then add to read list
      if (sd > 0)
        FD_SET(sd, &readfds);

      //highest file descriptor number, need it for the select function
      if (sd > max_sd)
        max_sd = sd;
    }

    /*

      Percorre o array de 30 clientes possíveis em busca de algum cliente
      Se encontrar algum (client_socket[i] > 0 = TRUE),
        Adiciona o cliente á readfds

      Obs: max_sd armazena o valor do socket (porta) mais alto
      Se algum cliente estiver em uma porta mais alta que o master:
        max_sd = porta do cliente

    */

    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR))
    {
      printf("select error");
    }

    /*

      O método select() monitora a readfds (lista de sockets) em busca de uma atividade
      Enquanto uma atividade não ocorrer, processador fica bloqueado nessa instrução
      Quando ocorrer, indica que chegou alguma solicitação de conexão:
        O processador sai da instrução e segue a execução

    */

    //If something happened on the master socket ,
    //then its an incoming connection
    if (FD_ISSET(master_socket, &readfds))
    {
      if ((new_socket = accept(master_socket,
                               (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
      {
        perror("accept");
        exit(EXIT_FAILURE);
      }

      /*

        Se o master_socket está no readfds:
          Extrai a primeira solicitação de conexão da fila de conexões pendentes no socket ouvinte
          Cria um novo socket conectado e retorna um arquivo referenciando esse socket
          Assim, a conexão é estabelecida entre o cliente e o servidor e os dados podem ser enviados

      */

      //inform user of socket number - used in send and receive commands
      printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      //send new connection greeting message
      if (send(new_socket, message, strlen(message), 0) != strlen(message))
      {
        perror("send");
      }

      puts("Welcome message sent successfully");

      //add new socket to array of sockets
      for (i = 0; i < max_clients; i++)
      {
        //if position is empty
        if (client_socket[i] == 0)
        {
          client_socket[i] = new_socket;
          printf("Adding to list of sockets as %d\n", i);

          break;
        }
      }
    }

    //else its some IO operation on some other socket
    for (i = 0; i < max_clients; i++)
    {
      sd = client_socket[i];

      if (FD_ISSET(sd, &readfds))
      {
        //Check if it was for closing , and also read the
        //incoming message
        if ((valread = read(sd, buffer, 1024)) == 0)
        {
          //Somebody disconnected , get his details and print
          getpeername(sd, (struct sockaddr *)&address, \
                        (socklen_t *)&addrlen);
          printf("Host disconnected , ip %s , port %d \n",
                 inet_ntoa(address.sin_addr), ntohs(address.sin_port));

          //Close the socket and mark as 0 in list for reuse
          close(sd);
          client_socket[i] = 0;
        }

        //Echo back the message that came in
        else
        {
          //set the string terminating NULL byte on the end
          //of the data read
          printf("\nRecebido do socket:\n");
          printf("-- %s\n\n", buffer);
          // client comunication
          if (strstr(buffer, "PROCESS_CLIENT") == buffer)
          {
            control = 1;
          }

          // worker communication
          if (control)
          {
            if (strcmp(buffer, "GET_JOB") == 0)
            {
              printf("Enviando o job...\n\n", buffer);
              send(sd, sendJob, strlen(buffer), 0);
            }
            else if (strstr(buffer, "RESULT_JOB") == buffer)
            {
              printf("Fechando conexao...\n\n", buffer);
              send(sd, messageCloseConnect, strlen(buffer), 0);
            }
          }
        }
      }
    }
  }

  return 0;
}
