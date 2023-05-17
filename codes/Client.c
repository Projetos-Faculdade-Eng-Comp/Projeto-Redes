#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  if (argc == 3)
  {
    printf("Iniciando conexao no endereço IP %s na porta %s...\n", argv[1], argv[2]);
  }
  else if (argc > 3)
  {
    printf("Muitos argumentos fornecidos. Apenas o endereço IP e a porta do servidor sao necessarios.\n");
    exit(1);
  }
  else if (argc < 2)
  {
    printf("O endereço IP e o numero da porta do servidor precisam ser fornecidos.\n");
    exit(1);
  }

  int client_FD, port_number;
  struct sockaddr_in server_address;
  char server_message[2000], client_message[2000];

  client_FD = socket(AF_INET, SOCK_STREAM, 0);

  if (client_FD < 0)
  {
    printf("Erro ao criar socket.\n");
    exit(1);
  }
  printf("Socket criado com sucesso.\n");

  port_number = atoi(argv[2]);

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_number);
  server_address.sin_addr.s_addr = inet_addr(argv[1]);

  if (connect(client_FD, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
  {
    printf("Nao foi possivel realizar a conexao.\n");
    exit(1);
  }
  printf("Conexao com o servidor realizada com sucesso.\n");
  printf("Digite %s para encerrar a conexao\n\n", "Bye");

  while (1)
  {
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    printf("Cliente: ");
    fgets(client_message, sizeof(client_message), stdin);

    if (send(client_FD, client_message, strlen(client_message), 0) < 0)
    {
      printf("Nao foi possivel enviar a mensagem.\n");
      exit(1);
    }

    if (recv(client_FD, server_message, sizeof(server_message), 0) < 0)
    {
      printf("Nao foi possivel receber a mensagem.\n");
      exit(1);
    }
    printf("Servidor: %s", server_message);

    if (strncmp("Bye", server_message, 3) == 0)
    {
      break;
    }
  }

  close(client_FD);

  return 0;
}
