#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    printf("Servidor iniciando na porta %s...\n", argv[1]);
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
  char server_message[2000], client_message[2000];

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

  if (listen(server_FD, 2) < 0)
  {
    printf("Erro ao preparar a fila de conexoes.\n");
    exit(1);
  }
  printf("Aguardando novas conexoes...\n");

  client_size = sizeof(client_address);
  client_FD = accept(server_FD, (struct sockaddr *)&client_address, &client_size);

  if (client_FD < 0)
  {
    printf("Nao foi possivel realizar a conexao.\n");
    exit(1);
  }
  printf("Cliente conectado no IP: %s com a porta: %i\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

  while (1)
  {
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    if (recv(client_FD, client_message, sizeof(client_message), 0) < 0)
    {
      printf("Nao foi possivel receber a mensagem.\n");
      exit(1);
    }
    printf("Cliente: %s", client_message);

    fgets(server_message, sizeof(server_message), stdin);

    if (send(client_FD, server_message, strlen(server_message), 0) < 0)
    {
      printf("Nao foi possivel enviar a mensagem.\n");
      exit(1);
    }

    if (strncmp("Bye", client_message, 3) == 0)
    {
      break;
    }
  }

  close(client_FD);
  close(server_FD);

  return 0;
}
