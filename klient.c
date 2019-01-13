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


/// argv[1] = 1111 argv[2] = 1112 argv[3] = address serwera
int main (int argc, char *argv[])
{ 
   int sock1,sock2;
   int sock1_int,sock2_int;//,ilePushy;ilePushy=0; 
   struct sockaddr_in klient,klient2;
   DIR *dir;
   struct dirent *ent; 
   char bufor[100],path[100],path_curr2[100];
   char buf2[100]; 
   getcwd(path, sizeof(path));
   strcpy(path_curr2,concat(path,"/"));
 
   if (argc != 4)
   {
     fprintf(stderr, "Sposób użycia: %s port1_number port2_number address\n", argv[0]);
     exit(1);
   }


   sock1 = socket(AF_INET, SOCK_STREAM, 0);
   sock2 = socket(AF_INET, SOCK_STREAM, 0);
   if (sock1 < 0 || sock2<0)
   {
      fprintf(stderr, "%s: Błąd przy probie utworzenia gniazda_3.\n", argv[0]);
      exit(1);
   }

   memset(&klient, 0, sizeof(struct sockaddr));
   memset(&klient2, 0, sizeof(struct sockaddr)); 

   klient.sin_family = AF_INET;
   inet_pton(AF_INET,argv[3],&(klient.sin_addr));
   klient.sin_port = htons(atoi(argv[1]));

   klient2.sin_family = AF_INET;
   inet_pton(AF_INET,argv[3],&(klient2.sin_addr));
   klient2.sin_port = htons(atoi(argv[2])); 
   int one = 1;
   setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));   //ustawianie ponownego uzywania gniazd, zeby nam wszystkich nie zablokowalo lul
   setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));


   sock1_int = connect(sock1, (struct sockaddr*)&klient, sizeof(struct sockaddr)); //write and read
   sock2_int = connect(sock2, (struct sockaddr*)&klient2, sizeof(struct sockaddr)); //commands
   sleep(2);

   if (sock1_int < 0 || sock2_int<0)
   {
      fprintf(stderr, "%s: Błąd przy próbie połączenia z serwerem2 (%s:%i).\n", argv[0], argv[1], atoi(argv[2]));
      exit(1);
   }

    char *buf_temp[10];
    memset(&buf_temp, 0, sizeof(buf_temp));
    while(1)    
    {
        puts("wpisz komende\n");
        fgets(bufor,30, stdin);
        //send(sock2,bufor,strlen(bufor),0);        
        Write(sock2,bufor,strlen(bufor));
        parse(bufor,buf_temp);
        
        if(buf_temp[0]!=NULL){        
        if(strcmp(buf_temp[0], "lcd") == 0 && buf_temp[1]!=NULL){
        if(strcmp(buf_temp[0],"lcd") == 0 && (strcmp(buf_temp[1],"..")==0 ||strcmp(buf_temp[1],"../")==0))
        {
            char curpath[100];
                char pom_path[100];               
                strcpy(curpath,path_curr2);
                curpath[strlen(curpath)-1]=0;
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
        else /*if(strcmp(buf_temp[0], "lcd") == 0 && buf_temp[1]!=NULL)*/{
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
        }
        else if(strcmp(buf_temp[0], "cd") == 0 && buf_temp[1]!=NULL)
        {
              //odczytuje to co serwer przeslal
             // char b[2];
              char c;
             // char bufferpwd[200];
              printf("nowy folder: ");              
              while ((read(sock1, &c, 1)) > 0 && c != (char)('!')){ 
                  //printf("zaczeto jazde\n");   
                  printf("%c",c);
              }
              printf("\n");    
        }
        else if (strcmp(buf_temp[0], "lls") == 0){             //dios mio, to listuje pliki z danego folderu
                    
            if ((dir = opendir (path_curr2)) != NULL) {

              while ((ent = readdir (dir)) != NULL) {
                  strcpy(buf2,ent->d_name);
                    printf ("%s\n", buf2);
                }
                closedir (dir);
          } 
              else {
                perror ("");
                return EXIT_FAILURE;
              }
        }
        else if (strcmp(buf_temp[0], "ls") == 0){
          printf("zaczeto lsa\n");
          char c;
         // write(sock1,buf_temp[0],strlen(buf_temp[0]));
          //read(sock1,bufls,100);
          //printf("%s\n",bufls);
          while ((read(sock1, &c, 1)) > 0 && c != (char)('!')){ 
              //printf("zaczeto jazde\n");   
              printf("%c",c);                
          //  bufls[l] = c;
             // l++;

          }
        //  printf("%s\n",bufls);
        }
        else if (strcmp(buf_temp[0], "push") == 0 && buf_temp[1]!=NULL){
          
          char buffer[100];char nic[1]="w"; 
          int c,l;
         // n = 0;
          l=0;
          //int len = strlen(buf_temp[1]);
          printf("plik: %s \n",buf_temp[1]);  
          FILE *file = fopen(buf_temp[1], "r");
          if (file == NULL){ 
            printf("blad odczytu");
            //send(sock1,nic, 1, 0);
            sendSize(sock1,1);
            Write(sock1,nic,1);
           // Write(sock1,EoF,1);//koniec przesylania
          }else if(file!=NULL)
          {   
            uint32_t p,k,rozmiar;
            fseek(file,0,SEEK_END);k=ftell(file);
            fseek(file,0,SEEK_SET);p=ftell(file);
            rozmiar=k-p;
            if(k!=p){
            sendSize(sock1,rozmiar);
            printf("Rozmiar pliku: %u\n",rozmiar);
            while ((c = getc(file)) != EOF){
                //printf("%c\n",c);
                buffer[l%100] = c;
                l+=1;
                if(l==100){
                        //send(sock1, buffer, l, 0);
                        Write(sock1,buffer,l);                            
                        memset(&buffer, 0, sizeof(buffer));
                        l=0;
                    }
              }
            if(l<100){
                //send(sock1, buffer, l, 0);    
                Write(sock1,buffer,l);
            }
          //  Write(sock1,EoF,1);//koniec przesylania
            fclose(file);
            printf("the file was sent successfully\n");   
            }
            else {
                   printf("plik jest pusty!\n");
                   //wysylam znak, zeby serwer nie czekal w nieskonczonosc                      
                    //send(sock1,nic, 1, 0);
                    sendSize(sock1,1);     
                    Write(sock1,nic,1);  
            //        Write(sock1,EoF,1);   //koniec przesylania 
            }  
          }
        }
        else if (strcmp(buf_temp[0], "pull") == 0){

          char buffer[100];
          uint32_t rozmiar;  
          int amount,ileczytac;

          FILE *fp;
          fp=fopen(buf_temp[1],"w");
          if(fp)
          {
                printf("zaczynam pobierac dane\n");
                readSize(sock1,&rozmiar);
                printf("Rozmiar pliku: %u\n",rozmiar);
                do
                {
                      if(rozmiar>100)ileczytac=100;
                      else ileczytac=rozmiar;  //blisko konca       odbierania       
                      amount=Read(sock1, buffer,ileczytac);
                      //printf("%d\n",amount);  
                      fwrite(buffer,1,amount,fp);//zapis do pliku
                      rozmiar=rozmiar-amount;  
                 }
                 while(rozmiar>0);                                   
                 fclose(fp);
          }  
          printf("pull ended\n");
        }
        else if (strcmp(buf_temp[0], "pwd") == 0){
          char c;
          while ((read(sock1, &c, 1)) > 0 && c != (char)('!')){ 
              printf("%c",c);
          }
          printf("\n");
        }
        else if (strcmp(buf_temp[0],"lpwd")==0){
            printf("Lokalny folder: %s \n",path_curr2);                       
        }
        else if (strcmp(buf_temp[0], "quit") == 0){ 
          break;
        }
       // else{       
          // Write(sock1,buf_temp[0],strlen(buf_temp[0]));  
       //    printf("Brak dopasowania");            
      //  }
        
        }
        memset(&buf_temp, 0, sizeof(buf_temp));
        memset(&bufor, 0, sizeof(bufor));   
    }

   freeMemory(buf_temp); 
   printf("KONIEC\n"); 
   close(sock1);
   close(sock2); 
   return 0;

} 
