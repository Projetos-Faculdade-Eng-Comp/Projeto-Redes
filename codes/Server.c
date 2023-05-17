#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CLIENTS 2

int client_sockets[MAX_CLIENTS];
pthread_t client_threads[MAX_CLIENTS];
int connected_clients = 0;

void *handle_client(void *arg)
{
  int client_FD = *(int *)arg;
  char client_message[2000];

  while (1)
  {
    memset(client_message, '\0', sizeof(client_message));

    if (recv(client_FD, client_message, sizeof(client_message), 0) < 0)
    {
      printf("Nao foi possivel receber a mensagem.\n");
      break;
    }
    printf("Cliente: %s", client_message);

    int other_client = (client_FD == client_sockets[0]) ? client_sockets[1] : client_sockets[0];

    if (send(other_client, client_message, strlen(client_message), 0) < 0)
    {
      printf("Nao foi possivel enviar a mensagem.\n");
      break;
    }

    if (strncmp(client_message, "Bye", 3) == 0)
    {
      printf("Conexão com cliente encerrada.\n");
      break;
    }
  }

  close(client_FD);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    printf("Iniciando servidor na porta %s...\n", argv[1]);
  }
  else if (argc > 2)
  {
    printf("Muitos parametros fornecidos. Apenas a porta do servidor e necessaria.\n");
    exit(1);
  }
  else
  {
    printf("E necessario definir a porta do servidor.\n");
    exit(1);
  }

  int server_FD, client_FD, port_number, client_size;
  struct sockaddr_in server_address, client_address;

  server_FD = socket(AF_INET, SOCK_STREAM, 0);

  if (server_FD < 0)
  {
    printf("Erro ao criar socket.\n");
    exit(1);
  }
  printf("Socket criado com sucesso.\n");

  port_number = atoi(argv[1]);

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_number);
  server_address.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_FD, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
  {
    printf("Erro ao vincular o socket com a porta.\n");
    exit(1);
  }
  printf("O socket foi vinculado a porta com sucesso.\n");

  if (listen(server_FD, MAX_CLIENTS) < 0)
  {
    printf("Erro ao preparar a fila de conexoes.\n");
    exit(1);
  }
  printf("Aguardando novas conexoes...\n");

  while (connected_clients < MAX_CLIENTS)
  {
    client_size = sizeof(client_address);
    client_FD = accept(server_FD, (struct sockaddr *)&client_address, &client_size);

    if (client_FD < 0)
    {
      printf("Nao foi possivel realizar a conexao.\n");
      continue;
    }
    printf("Cliente conectado no IP: %s com a porta: %i\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    client_sockets[connected_clients] = client_FD;

    if (pthread_create(&client_threads[connected_clients], NULL, handle_client, &client_sockets[connected_clients]) != 0)
    {
      printf("Erro ao criar thread para o cliente.\n");
      continue;
    }

    connected_clients++;
  }

  printf("Numero maximo de clientes conectados.\n");
  printf("Digite '%s' para encerrar a conexao\n", "Bye");
  printf("Aguardando mensagem...\n\n");

  for (int i = 0; i < connected_clients; i++)
  {
    pthread_join(client_threads[i], NULL);
  }

  close(server_FD);

  return 0;
}
