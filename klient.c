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

char** divide2(char* in,const char s_delim)
{
    char **wynik=0;
    char *tmp=in;
    char *rest=in;
    char* token;
    char* lastsep=0;
    size_t licz=0;
    size_t idx=0;
    char delim[2]; //jednoznakowy separator
    delim[0]=s_delim;
    delim[1]=0;
    
    //Policz ile bedzie oddzielonych od siebie wyrazow
    while(*rest) //iterujemy po wszystkich wyrazach
    {
        if (s_delim== *rest){licz++;lastsep=rest;}
        rest++;
    }

    //if(licz == 1){
    //  wynik[0] = lastsep;
    //  return wynik;
    //}

    printf("Wlicz1: %zu \n",licz);        
    
    // Add space for trailing token. 
    licz += lastsep < (in + strlen(in) - 1);

    // Add space for terminating null string so caller
     //  knows where the list of returned strings ends. 
    licz++;

    printf("Wlicz2: %zu \n",licz); 
    wynik= malloc(sizeof(char*)*(licz));
    while((token=strtok_r(tmp,delim,&tmp))) //iterujemy po wszystkich wyrazach
    {
        *(wynik+idx)=strdup(token);//zapisujemy do tablicy wyjsciowej
        //printf("wynik[%d] = %s \n",idx,*(wynik+idx));        
        idx++;
    }

    return wynik;
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
   int connection_socket_descriptor,connection_socket_descriptor2,listen_socket_descriptior;
   int sock1, sock2, sock3;
   int connect_result,connect2_result,connect3_result;
   int sock1_int,sock2_int,sock3_int;
   struct sockaddr_in klient,klient2 ,serwer;
   int k1;
   char bufor[100];

   if (argc != 4)
   {
     fprintf(stderr, "Sposób użycia: %s server_name port_number\n", argv[0]);
     exit(1);
   }


   sock1 = socket(AF_INET, SOCK_STREAM, 0);
   sock2 = socket(AF_INET, SOCK_STREAM, 0);
   sock3 = socket(AF_INET, SOCK_STREAM, 0);

   if (sock3 < 0)
   {
      fprintf(stderr, "%s: Błąd przy probie utworzenia gniazda_3.\n", argv[0]);
      exit(1);
   }

   memset(&klient, 0, sizeof(struct sockaddr));
   memset(&klient2, 0, sizeof(struct sockaddr));
   memset(&serwer, 0, sizeof(struct sockaddr));

   klient.sin_family = AF_INET;
   klient2.sin_family = AF_INET;
   serwer.sin_family = AF_INET;
   serwer.sin_port = htons(atoi(argv[3]));
   serwer.sin_addr.s_addr = INADDR_ANY;

   inet_pton(AF_INET,"127.0.0.1",&(klient.sin_addr));
   inet_pton(AF_INET,"127.0.0.1",&(klient2.sin_addr));


   klient.sin_port = htons(atoi(argv[1]));
   klient2.sin_port = htons(atoi(argv[2]));

   int one = 1;
   setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));   //ustawianie ponownego uzywania gniazd, zeby nam wszystkich nie zablokowalo lul
   setsockopt(sock1_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   setsockopt(sock2_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   setsockopt(sock3, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   setsockopt(sock3_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

   sock1_int = connect(sock1, (struct sockaddr*)&klient, sizeof(struct sockaddr));
   sock2_int = connect(sock2, (struct sockaddr*)&klient2, sizeof(struct sockaddr));

   if (sock2_int < 0)
   {
      fprintf(stderr, "%s: Błąd przy próbie połączenia z serwerem2 (%s:%i).\n", argv[0], argv[1], atoi(argv[2]));
      exit(1);
   }
   
   k1 = bind(sock3,(struct sockaddr*)&serwer,sizeof(serwer));
   k1 = listen(sock3,3);
   sock3_int = accept(sock3,NULL, NULL);

   if (sock3_int < 0)
   {
      fprintf(stderr, "%s: Błąd przy próbie połączenia do przesylu (%s:%i).\n", argv[0], argv[1], atoi(argv[3]));
      exit(1);
   }
   const char a = ' ';
    while(1)    
    {

        fgets(bufor,30, stdin);
        write(sock1,bufor,strlen(bufor));
        char** buf_temp = divide2(bufor,a);
        //printf("%s\n",buf_temp[0]);
        if (strcmp(buf_temp[0], "push") == 0){ //uprasza sie o korzystanie z push2 zamiast z tego
          printf("wszedl push przynajmniej\n");
        }
        
        else if (strcmp(buf_temp[0], "push2") == 0){

          printf("bobobo\n");
          
          char buffer[100];
          int n,c,l;
          n = 0;
          l=0;
          char d;

          int len = strlen(buf_temp[1]);
          buf_temp[1][len-1] = '\0';

          FILE *file = fopen(buf_temp[1], "r");
          if (file == NULL)
            printf("blad odczytu");
          else
            printf("ok\n");

          while ((c = getc(file)) != EOF){
            printf("%c\n",c);
            buffer[l] = c;
            l+=1;
          }
          printf("%s\n",buffer);

          send(sock2, buffer, l, 0);
          
          printf("the file was sent successfully");   

        
        }
        else if (strcmp(buf_temp[0], "pull") == 0){
          //write(sock1,bufor,strlen(bufor));
          char buffer[100];

          int amount;
          amount = read(sock3_int,buffer,10);

          int len = strlen(buf_temp[1]);
          buf_temp[1][len-1] = '\0';

          FILE *fp;
          fp=fopen(buf_temp[1],"w");
          printf("%s\n", "buffer");
          fwrite(buffer,1,amount,fp); 
          //fwrite("bobo",1,amount,fp); 

          printf("pull ended\n");
          fclose(fp);



        }
        else{
          printf("nie pasuje\n");          
           write(sock1,buf_temp[0],strlen(buf_temp[0]));
        }
        memset(&bufor[0], 0, sizeof(bufor));
        
    }
  

   close(sock1);
   return 0;

} 

//gcc -WALL klient.c a.out