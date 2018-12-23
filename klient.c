#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>

#define BUF_SIZE 1024
#define NUM_THREADS     5

//uchwyt na mutex
pthread_mutex_t example_mutex = PTHREAD_MUTEX_INITIALIZER;
char bufor[5];//[3]={'k','e','k'};
char alfabet[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r',
's','t','u','v','w','x','y','z'};

//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t
{
//TODO
  char buf[100];
  int socketdes;
};

//wskaźnik na funkcję opisującą zachowanie wątku
void *ThreadBehavior(void *t_data)
{
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;
    //dostęp do pól struktury: (*th_data).pole
    //TODO (przy zadaniu 1) klawiatura -> wysyłanie albo odbieranie -> wyświetlanie 
  //pthread_mutex_lock(&example_mutex);
  while(1){    
    fgets(th_data->buf,100, stdin);
      printf("dlugosc str: %zu",strlen(th_data->buf));
      write(th_data->socketdes,th_data->buf,strlen(th_data->buf));    
  }
      pthread_exit(NULL);

}

/*
//funkcja obsługująca połączenie z serwerem
void handleConnection(int connection_socket_descriptor) {
    //wynik funkcji tworzącej wątek
    int create_result = 0;

    //uchwyt na wątek
    pthread_t thread1;

    //dane, które zostaną przekazane do wątku
    struct thread_data_t t_data;
  t_data.socketdes =  connection_socket_descriptor;
    //TODO
    create_result = pthread_create(&thread1, NULL, ThreadBehavior, (void *)&t_data);
    if (create_result){
       printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
       exit(-1);
    } 


  while(1)
  {  
    int ile;
    ile=read(connection_socket_descriptor,bufor,100);
    
  //zajęcie
    int i;
    for(i=0;i<ile;i++)
  {
    printf("%c",bufor[i]);
  }
  printf("\n");
} 
  //zwolnienie
    //pthread_mutex_unlock(&example_mutex); 

}
*/


int main (int argc, char *argv[])
{ 
   int connection_socket_descriptor,connection_socket_descriptor2;//
   int connect_result,connect2_result;
   struct sockaddr_in server_address, client_address;
   //struct hostent* server_host_entity;
   char bufor[10];

   if (argc != 3)
   {
     fprintf(stderr, "Sposób użycia: %s server_name port_number\n", argv[0]);
     exit(1);
   }


   connection_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
   connection_socket_descriptor2 = socket(AF_INET, SOCK_STREAM, 0);

   if (connection_socket_descriptor2 < 0)
   {
      fprintf(stderr, "%s: Błąd przy probie utworzenia gniazda.\n", argv[0]);
      exit(1);
   }

   memset(&server_address, 0, sizeof(struct sockaddr));
   memset(&client_address, 0, sizeof(struct sockaddr));

   server_address.sin_family = AF_INET;
   client_address.sin_family = AF_INET;

   inet_pton(AF_INET,"127.0.0.1",&(server_address.sin_addr));
   inet_pton(AF_INET,"127.0.0.1",&(client_address.sin_addr));


   //memcpy(&server_address.sin_addr.s_addr, server_host_entity->h_addr, server_host_entity->h_length);
   server_address.sin_port = htons(atoi(argv[1]));
   //printf("port2 = %s\n", argv[2]);
   client_address.sin_port = htons(atoi(argv[2]));

   connect_result = connect(connection_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   connect2_result = connect(connection_socket_descriptor2, (struct sockaddr*)&client_address, sizeof(struct sockaddr));
   if (connect2_result < 0)
   {
      fprintf(stderr, "%s: Błąd przy próbie połączenia z serwerem2 (%s:%i).\n", argv[0], argv[1], atoi(argv[2]));
      exit(1);
   }

  // handleConnection(connection_socket_descriptor);
    while(1)    
    {
        fgets(bufor,10, stdin);
        if (strcmp(bufor, "push\n") == 0){ //czy przed rozpoczeciem wysylania czekamy na akceptacje czy cos
          write(connection_socket_descriptor,bufor,strlen(bufor));
          //read()
          char buffer[100];
          int n,c,l;
          l,n = 0;
          char d;
          FILE *file = fopen("add.txt", "r");
          if (file == NULL)
            printf("blad odczytu");
          else
            printf("ok\n");
          //int m = read(file, buffer, 10);
          while ((c = getc(file)) != EOF){
            buffer[l] = c;
            l+=1;
          }
          printf("%s\n",buffer);
          /*
          while ((c = fgetc(file)) != EOF)
          {
              d = c;
              printf("%c\n",d);

              write(connection_socket_descriptor,d,sizeof(d));
          }
          */
          sleep(2);
          send(connection_socket_descriptor2, buffer, l, 0);
          printf("the file was sent successfully");   
          
        }

        if (strcmp(bufor, "push2\n") == 0){
          char buffer[100];
          int n,c;
          n = 0;
          char d;
          FILE *file = fopen("add.txt", "r");
          if (file == NULL)
            printf("blad odczytu");
          else
            printf("ok\n");
          int m = read(file, buffer, 100);

          send(connection_socket_descriptor2, buffer, m, 0);
          printf("the file was sent successfully");   


        }
        else{          
           write(connection_socket_descriptor,bufor,strlen(bufor));
        }
    }


   close(connection_socket_descriptor);
   return 0;

} 

//gcc -WALL klient.c a.out