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
#include <dirent.h>
#include "header1.h"

#define BUF_SIZE 1024
#define NUM_THREADS     5

//uchwyt na mutex
pthread_mutex_t example_mutex = PTHREAD_MUTEX_INITIALIZER;



int main (int argc, char *argv[])
{ 
   int connection_socket_descriptor,connection_socket_descriptor2,listen_socket_descriptior;
   int sock1, sock2, sock3;
   int connect_result,connect2_result,connect3_result;
   int sock1_int,sock2_int,sock3_int;
   struct sockaddr_in klient,klient2 ,serwer;
   int k1;
   DIR *dir;
   struct dirent *ent; 
   char bufor[100],path[100],path_curr2[100];
   char buf2[100]; 
   getcwd(path, sizeof(path));
   strcpy(path_curr2,concat(path,"/"));
 
   if (argc != 4)
   {
     fprintf(stderr, "Sposób użycia: %s server_name port_number\n", argv[0]);
     exit(1);
   }


   sock1 = socket(AF_INET, SOCK_STREAM, 0);
   sock2 = socket(AF_INET, SOCK_STREAM, 0);
   //sock3 = socket(AF_INET, SOCK_STREAM, 0);

   if (sock2 < 0)
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
   //inet_pton(AF_INET,"127.0.0.1",&(serwer.sin_addr)); 
   inet_pton(AF_INET,"127.0.0.1",&(klient2.sin_addr));


   klient.sin_port = htons(atoi(argv[1]));
   klient2.sin_port = htons(atoi(argv[2]));

   int one = 1;
   setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));   //ustawianie ponownego uzywania gniazd, zeby nam wszystkich nie zablokowalo lul
   setsockopt(sock1_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   //setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   //setsockopt(sock2_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   //setsockopt(sock3, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
   //setsockopt(sock3_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));


   sock1_int = connect(sock1, (struct sockaddr*)&klient, sizeof(struct sockaddr));
   printf("polaczono z gniazdem 1\n");
   sleep(2);
   //sock2_int = connect(sock2, (struct sockaddr*)&klient2, sizeof(struct sockaddr));
   //puts('bobobobobbo');

   //k1 = bind(sock3,(struct sockaddr*)&serwer,sizeof(serwer));
 //  k1 = listen(sock3,3);
 //  sock3_int = accept(sock3,NULL, NULL);
//
   if (sock1_int < 0)
   {
      fprintf(stderr, "%s: Błąd przy próbie połączenia z serwerem2 (%s:%i).\n", argv[0], argv[1], atoi(argv[2]));
      exit(1);
   }

   const char a = ' ';
   //char** buf_temp;
    char *buf_temp[10];
    memset(&buf_temp, 0, sizeof(buf_temp));
    while(1)    
    {
        puts("bobobo");
        fgets(bufor,30, stdin);
        printf("%d\n",strlen(bufor));
        send(sock1,bufor,strlen(bufor),0);        
        //printf("klient\n");        
        //write(sock1,bufor,strlen(bufor));
        parse(bufor,buf_temp);
        
        if(buf_temp[0]!=NULL){        
        if (strcmp(buf_temp[0], "push") == 0){ //uprasza sie o korzystanie z push2 zamiast z tego
            printf("wszedl push przynajmniej\n");
        }
        else if(strcmp(buf_temp[0],"lcd") == 0 && (strcmp(buf_temp[1],"..")==0 ||strcmp(buf_temp[1],"../")==0))
        {
            char curpath[100];//=malloc(strlen(path_curr2)*sizeof(char));
                char pom_path[100];               
                strcpy(curpath,path_curr2);
                //na samym koncu path_curr2 jest '/' wiec c znalezc przed ostatni '/'
                curpath[strlen(curpath)-1]=0;
               // printf("%s\n",curpath);
                //wycofuje sie o jeden folder do tylu czyli musze znalezc '/'
                char *slash = strrchr(curpath, '/');
                size_t rozm=strlen(curpath)-strlen(slash)-1;                
                memset(&pom_path[0], 0, sizeof(pom_path));
                //nowa sciezka                
                for(int i=strlen(curpath)-1;i>=0;i--)
                {
                      if(i>rozm)   
                          *(pom_path+i)=0;
                      else
                          *(pom_path+i)=*(curpath+i);
                }
                //cast to const char (maybe)    
                if(chdir(pom_path)==0){
                      memset(&path_curr2[0], 0, sizeof(path_curr2));
                      strcpy(path_curr2,concat(pom_path,"/"));
                      printf("Zmieniłem lokal folder na: %s \n",path_curr2);                       
                }
        }
        else if(strcmp(buf_temp[0], "lcd") == 0 && buf_temp[1]!=NULL){
            //zmien folder
            char pom[100];
            strcpy(pom,path_curr2);
            if(chdir(concat(pom,buf_temp[1]))==0){
                memset(&path_curr2[0], 0, sizeof(path_curr2));
                strcpy(pom,concat(pom,buf_temp[1]));
                strcpy(path_curr2,concat(pom,"/"));
                printf("Zmieniłem lokal folder na: %s \n",path_curr2);
            }            
        }
        else if (strcmp(buf_temp[0], "lls") == 0){             //dios mio, to listuje pliki z danego folderu
                    
            if ((dir = opendir (path_curr2)) != NULL) {

              while ((ent = readdir (dir)) != NULL) {
                  strcpy(buf2,ent->d_name);
                //  char *dot = strrchr(buf2, '.');
                //  if (dot && !strcmp(dot, ".c")){
                    printf ("%s\n", buf2);
                //  }
                }
                closedir (dir);
          } 
              else {
                perror ("");
                return EXIT_FAILURE;
              }
        }
        else if (strcmp(buf_temp[0], "push2") == 0 && buf_temp[1]!=NULL){
          
          char buffer[100];
          int n,c,l;
          n = 0;
          l=0;
          char d;
          int len = strlen(buf_temp[1]);
          //buf_temp[1][len-1] = '\0';
          printf("plik: %s \n",buf_temp[1]);  
          //printf("cos2%d-\n",strlen(buf_temp[1]));  
          FILE *file = fopen(buf_temp[1], "r");
          if (file == NULL){ //&& strlen(buf_temp[1])>0){
            printf("blad odczytu");
          //  write(sock1,"Nk",2); //informacja do serwera by nie tworzyl pliku
           // printf(" i wyslalem N\n");
          }else if(file!=NULL)
          {   
            //pobierz rozmiar pliku, jesli wiekszy od 0 to zacznij przesylac
            long p,k;
            fseek(file,1,SEEK_SET);k=ftell(file);
            fseek(file,0,SEEK_SET);p=ftell(file);
            if(k!=p){
          //  write(sock1,"Yk",2); //informacja do serwera by stworzyl plik
          //  printf(" i wyslalem Y\n");
            while ((c = getc(file)) != EOF){
                printf("%c\n",c);
                buffer[l%100] = c;
                l+=1;
                //jesli zapelnilem bufor, to wysylam pierwsza paczke
                if(l==100){
                        send(sock1, buffer, l, 0);
                        //zeruje licznik i buffer
                        memset(&buffer, 0, sizeof(buffer));
                        l=0;
                    }
              }
            //jesli l jest mniejsze od 100 i byl EOF to dosylam
            if(l<100)send(sock1, buffer, l, 0);    
            fclose(file);
            printf("the file was sent successfully");   
            }
            else {
                   printf("plik jest pusty!\n");
                   char nic[1]="w";  //wysylam znak, zeby serwer nie czekal w nieskonczonosc                      
                    send(sock1,nic, 1, 0);           
            }  
          }
        }
        else if (strcmp(buf_temp[0], "touch") == 0 && buf_temp[1]!=NULL){
            printf("Niech serwer stworzy pusty plik o nazwie %s!\n",buf_temp[1]);
        }
        else if (strcmp(buf_temp[0], "pull") == 0){
          //write(sock1,bufor,strlen(bufor));
          char buffer[100];

          int amount;
          amount = read(sock1,buffer,10);

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
        else if (strcmp(buf_temp[0], "lpwd") == 0){          //printuje obecny folder
          printf("%s\n", path_curr2);
        }
        else if (strcmp(buf_temp[0], "quit") == 0){          //printuje obecny folder
          break;
        }
        else{
         // printf("nie pasuje\n");          
           write(sock1,buf_temp[0],strlen(buf_temp[0]));
        }
        
        }
        
        //if(buf_temp[0]!=NULL)freeMemory(buf_temp);
        memset(&buf_temp, 0, sizeof(buf_temp));
        memset(&bufor, 0, sizeof(bufor));   
    }

   freeMemory(buf_temp); 
   printf("KONIEC\n"); 
   close(sock1);
   return 0;

} 

//gcc -WALL klient.c a.out
