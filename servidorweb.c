#include <io.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")
#define PORT 80
#define MESSAGESIZE 1024
#define MAXSOCKETS 100

SOCKET new_socket[MAXSOCKETS];
int qtdsockets = 0;
char ft[80];

int arquivoExiste(char *nomeArquivo){
    FILE *arquivo = fopen(nomeArquivo, "r");
    if(arquivo == NULL)
        return 0;
    fclose(arquivo);
    return 1;
}

char *lerArquivo(char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);

    char *conteudo = (char *)malloc(tamanho + 1);

    fread(conteudo, 1, tamanho, arquivo);
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    return conteudo;
}

void getcurrenttimeformated(char *ft)
{
    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = gmtime(&rawtime);
    strftime(ft, 80, "%a, %d %b %Y %X GMT", info);
}

void pegarCaminho(char caminho[], char message[])
{
    if (strncmp(message, "GET / ", 6) == 0)
        strcpy(caminho, "index");
    else
        sscanf(message, "GET /%s ", caminho);
    strcat(caminho, ".html");
}

void getdata(int pos) {
  int len;
  char request[MESSAGESIZE];
  char header[MESSAGESIZE];
  char caminho[MESSAGESIZE];
  char *page;
  while(1) {
    request[0]='\0';
    len=recv(new_socket[pos],request,MESSAGESIZE,0);
    if(len>0) {
      len=0;
      pegarCaminho(caminho,request);
      getcurrenttimeformated(ft);
      if(arquivoExiste(caminho)){
        page = lerArquivo(caminho);
        sprintf(header,"HTTP/1.1 200 OK\r\nDate: %s\r\nContent-Length: %d\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n",ft,strlen(page)+1);
      }
      else{
         page = lerArquivo("error404.html");
         sprintf(header,"HTTP/1.1 404 Not Found\r\nDate: %s\r\nContent-Length: %d\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n",ft,strlen(page)+1);
      }
      strcat(page,"\r\n\n");
      send(new_socket[pos], header, strlen(header)+1, 0);
      send(new_socket[pos], page, strlen(page)+1, 0);
    }
  }
}

int main(int argc , char *argv[]) {
  WSADATA wsa;
  SOCKET s;
  struct sockaddr_in server , client;
  int c, pos;
  char errormessage[MESSAGESIZE];

  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
    printf("\nFalha na inicializacao da biblioteca Winsock: %d",WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
    printf("\nNao e possivel inicializar o socket: %d" , WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( PORT );
  if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR) {
    printf("\nNao e possivel construir o socket: %d" , WSAGetLastError());
    exit(EXIT_FAILURE);
  }

  listen(s,3);
  c = sizeof(struct sockaddr_in);
  printf("*** SERVER ***\n\nAguardando conexoes...\n\n");

  while(1) {
    pos=qtdsockets;
    new_socket[qtdsockets++] = accept(s, (struct sockaddr *)&client, &c);
    if (new_socket[pos] == INVALID_SOCKET) {
      printf("\nConexao não aceita. Codigo de erro: %d" , WSAGetLastError());
    }
    else {
      puts("\nConexao aceita.");
      printf("\nDados do cliente - IP: %s -  Porta: %d\n",inet_ntoa(client.sin_addr),htons(client.sin_port));

      if (qtdsockets <= MAXSOCKETS) {
        _beginthread(getdata,NULL,pos);
      }
      else {
        puts("\nNumero maximo de conexoes excedido!");
        strcpy(errormessage,"HTTP/1.1 429\r\nToo Many Requests\r\nContent-Type: text/html\r\nRetry-After: 60\r\n\r\n<HTML><H1>Numero maximo de conexoes excedido!</H1></HTML>\r\n");
        send(new_socket[pos] , errormessage , strlen(errormessage)+1, 0);
        closesocket(new_socket[pos]);
        qtdsockets--;
      }
    }
  }
  closesocket(s);
  WSACleanup();
}
