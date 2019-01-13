/*FTP server*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include "header1.h"

pthread_mutex_t mutex;

struct thread_data_t
{
    int sockinout;
    int sockcmd;
};

int pull(int sock1,char *buf_temp){
  char buffer[100];
          char nic[1]="w";
          int c,l;
          l=0;
          FILE *file = fopen(buf_temp, "r");
          if (file == NULL){
            printf("blad odczytu");
             sendSize(sock1,1);     
             Write(sock1,nic,1);             
            }
          else
            printf("ok\n");
            
          if(file){
                //pobierz rozmiar pliku, jesli wiekszy od 0 to zacznij przesylac
            uint32_t p,k,rozmiar;
            fseek(file,0,SEEK_END);k=ftell(file);
            fseek(file,0,SEEK_SET);p=ftell(file);
            rozmiar=k-p;
            if(k!=p){
            sendSize(sock1,rozmiar);//wysylam info ile bajtow danych bede wysylal     
            printf("Rozmiar pliku: %u\n",rozmiar);  
            while ((c = getc(file)) != EOF){
                buffer[l%100] = c;
                l+=1;
                //jesli zapelnilem bufor, to wysylam pierwsza paczke
                if(l==100){
                        Write(sock1,buffer,l);                        
                        //zeruje licznik i buffer
                        memset(&buffer, 0, sizeof(buffer));
                        l=0;
                    }
              }
            //jesli l jest mniejsze od 100 i byl EOF to dosylam
            if(l<100){ 
                Write(sock1,buffer,l);              
            }
            fclose(file);
            printf("the file was sent successfully\n");   
            }
            else {
                   printf("plik jest pusty!\n");
                //wysylam znak, zeby serwer nie czekal w nieskonczonosc                      
                    //send(sock1,nic, 1, 0);
                    sendSize(sock1,1);
                    Write(sock1,nic,1); 
                   // Write(sock1,EoF,1); 
                    printf("wyslalem prawie nic\n");         
            } 
          
          } 
          return 3;
}

int push(int sock1,char *buf_temp,char *path_curr2){
  char buffer[100];
  int amount,ileczytac;
  uint32_t rozmiar=0;  
  FILE *f1;
  f1=NULL;  
  if(buf_temp!=NULL && strcmp(buf_temp,"")!=0)
  f1=fopen(concat(path_curr2 ,buf_temp),"w");
  if(f1){
        printf("zaczynam pobierac dane\n");
        //czytam ile danych zostanie mi przeslane
        readSize(sock1,&rozmiar);
        printf("Rozmiar pliku: %u\n",rozmiar);
        do
        {
              if(rozmiar>100)ileczytac=100;
              else ileczytac=rozmiar;  //blisko konca odbierania       
              amount=Read(sock1, buffer,ileczytac);
           //   printf("%d\n",amount);  
              fwrite(buffer,1,amount,f1);//zapis do pliku
              rozmiar=rozmiar-amount;  
        }
        while(rozmiar>0);                                        
        fclose(f1);
  }

  printf("push_zakonczono\n");
  return 3;
}



void *connection_handler(void *t_data)//*socket_desc)
{
  struct thread_data_t *th_data=(struct thread_data_t*)t_data;  
  int sock1 = (*th_data).sockinout; //read and write
  int sock2 = (*th_data).sockcmd;    //commands
    printf("Gniazda-> in/out: %d command: %d\n",sock1,sock2);
  char buf[10000];
  char buf2[10000];
  char buf4[100];
  DIR *dir;
  struct dirent *ent;
  char path_curr2[100],path[100];
  getcwd(path, sizeof(path));
  strcpy(path_curr2,concat(path,"/"));

  char c;
  int lic = 0;//,cmp;
  ssize_t data_read;//,l2,l1;
  char *buf_temp[10];//l2=0;
  memset(&buf_temp, 0, sizeof(buf_temp));   
  memset(&buf2, 0, sizeof(buf2));
  while(1){
   lic = 0;
    //odczyt komendy
    while ((data_read = read(sock2, &c, 1)) > 0 && c != '\n'){    
      buf[lic] = c;
      lic++; 
  }
  printf("buf: %s||buf2: %s\n",buf,buf2);  
  if(data_read<=0)return 0;		
 
    //jesli klient zrobil ctrl+c	
    if(data_read!=1)return 0;		
    memset(&buf_temp, 0, sizeof(buf_temp));
    parse(buf,buf_temp);    
    printf("buf_temp[0] %s\n",buf_temp[0]);    
    if(buf_temp[1]!=NULL)printf("buf_temp[1]=%s\n",buf_temp[1]);
        
    //aktualizuje lokalizacje
    getcwd(path, sizeof(path));
    strcpy(path_curr2,concat(path,"/"));


    if(lic>0 && buf_temp[0]!=NULL )//&& strlen(buf3)!=0) 
    {
        printf("Jaka komenda: %s\n",buf_temp[0]);        
        if(strcmp(buf_temp[0], "cd") == 0 && buf_temp[1]!=NULL){
			
          if (strcmp(buf_temp[0],"cd") == 0 && (strcmp(buf_temp[1],"..")==0 ||strcmp(buf_temp[1],"../")==0)){
                  
                  char curpath[100];
                  char pom_path[100];               
                  strcpy(curpath,path_curr2);
                  //na samym koncu path_curr2 jest '/' wiec c znalezc przed ostatni '/'
                  curpath[strlen(curpath)-1]=0;
                  printf("%s\n",curpath);
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
                  if(chdir(pom_path)==0){
                        memset(&path_curr2[0], 0, sizeof(path_curr2));
                        strcpy(path_curr2,concat(pom_path,"/"));
                        //wyslij obecny folder do klienta
                        memset(&curpath[0], 0, sizeof(curpath));                        
                        char b[2];
                        strcpy(b,"!");
                        strcpy(curpath,path_curr2);
                        strcat(curpath,b);
			            
                        Write(sock1,curpath,strlen(curpath));
                        printf("New serwer 1 folder : %s \n",curpath);     
                        printf("New serwer 1 folder : %s \n",path_curr2);                     
                  }
                  else //nie udalo sie zmienic
                    {
                        memset(&curpath[0], 0, sizeof(curpath));                        
                        char b[2];
                        strcpy(b,"!");
                        strcpy(curpath,path_curr2);
                        strcat(curpath,b);
                        Write(sock1,curpath,strlen(curpath));
                    }  
        }
        else {
            //zmien folder
            char pom[100],pom2[100];
            strcpy(pom,path_curr2);
            int wyn=chdir(concat(pom,buf_temp[1]));
            printf("desired path: %s\n",pom);
            if(wyn==0){
                memset(&path_curr2[0], 0, sizeof(path_curr2));
                strcpy(pom,concat(pom,buf_temp[1]));
                strcpy(path_curr2,concat(pom,"/"));
                //wyslij obecny folder do klienta
                memset(&pom2[0], 0, sizeof(pom2));                        
                char b[2];
                strcpy(b,"!");
                strcpy(pom2,path_curr2);
                strcat(pom2,b);
                Write(sock1,pom2,strlen(pom2));
                printf("New serwer 2 folder : %s \n",pom2);  
                printf("New serwer 2 folder : %s \n",path_curr2);  
            }   
            else //nie udalo sie zmienic
                    {
                        //memset(&pom2[0], 0, sizeof(pom2));                        
                        char b[2];
                        strcpy(b,"!");
                        strcat(pom,b);
                        Write(sock1,pom,strlen(pom));
                        printf("Bez zmian: %s\n",pom);
                    }         
            
           }
         printf("Folder cd: %s \n",path_curr2);  
        }
        else if (strcmp(buf_temp[0], "ls") == 0){ //listuje pliki i wysylam do klienta
          char b[2];
          printf("Folder ls: %s \n",path_curr2);  
          if ((dir = opendir (path_curr2)) != NULL) {
              while ((ent = readdir (dir)) != NULL) {
                    strcat(buf4,ent->d_name);
                    strcat(buf4,"\n");
                    
                }
                strcpy(b,"!");
                strcat(buf4,b);
                Write(sock1,buf4,strlen(buf4));
                closedir (dir);
                memset(&buf4, 0, sizeof(buf4));    //czyscimy bufor
          } 
              else {
                perror ("");
                return NULL;
              }
            printf("wyslano lsa\n");
        }

        else if (strcmp(buf_temp[0], "pwd") == 0){
          char path_temp[200];
          char b[2];
          strcpy(b,"!");
          strcpy(path_temp,path_curr2);
          strcat(path_temp,b);
          printf("%s\n", path_temp);
          Write(sock1,path_temp,strlen(path_temp));  
        }

        else if (strcmp(buf_temp[0], "mkdir")== 0 && buf_temp[1]!=NULL){
          mkdir(buf_temp[1], S_IRWXU);
        }
        else if (strcmp(buf_temp[0], "push") == 0 ){

          pthread_mutex_lock(&mutex);
          push(sock1,buf_temp[1],path_curr2);
          pthread_mutex_unlock(&mutex);
        }
        else if (strcmp(buf_temp[0], "pull") == 0 && buf_temp[1]!=NULL){
          pthread_mutex_lock(&mutex);
          int len = strlen(buf_temp[1]);
          buf_temp[1][len] = '\0';
          pull(sock1,buf_temp[1]);
          pthread_mutex_unlock(&mutex);  
        }

        else if (strcmp(buf_temp[0], "touch") == 0 && buf_temp[1]!=NULL){
            if(strlen(buf_temp[1])>0){
                FILE *fp;            
                fp=fopen(concat(path_curr2 ,buf_temp[1]),"w");
                if(fp){
                      printf("The file was created successfully\n");fclose(fp);                      
                }
            }
        }
        else if (strcmp(buf_temp[0], "rmdir") == 0 && buf_temp[1]!=NULL){   
            pthread_mutex_lock(&mutex);           
            char *sciezka=malloc(strlen(path_curr2)+strlen(buf_temp[1])+1);
            strcpy(sciezka,concat(path_curr2,buf_temp[1]));           
            removeFolder(sciezka);
            free(sciezka);
            pthread_mutex_unlock(&mutex);
        }
        else if (strcmp(buf_temp[0], "rm") == 0 && buf_temp[1]!=NULL){
            pthread_mutex_lock(&mutex);
            char *sciezka=malloc(strlen(path_curr2)+strlen(buf_temp[1])+1);
            strcpy(sciezka,concat(path_curr2,buf_temp[1]));           
            removeFile(sciezka);
            free(sciezka);        
            pthread_mutex_unlock(&mutex);
        }
        else if(strcmp(buf_temp[0], "quit") == 0){
            return 0;
        }
        else{
          printf("nieprawidlowa komenda\n");
        }
      printf("%s\n",buf);
  } 
              
      memset(&buf, 0, sizeof(buf));    //czyscimy bufor
  }
}

  

int main(int argc,char *argv[])
{
  struct sockaddr_in server,server2;  //struktura na adres
  int sock1,sock1_int,sock2,sock2_int;                  //gniazda
  int k,k2;
  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  sock2 = socket(AF_INET, SOCK_STREAM, 0);  
  char path_curr2[100],path[100];
  getcwd(path, sizeof(path));
  strcpy(path_curr2,concat(path,"/"));
  
  if (argc != 3)
   {
     fprintf(stderr, "Sposób użycia serwera: %s port1_number port2_number \n", argv[0]);
     exit(1);
   }  
  
  if(sock1 == -1 || sock2 ==-1)
    {
      printf("Socket creation failed");
      exit(1);
    }
  memset(&server,0,sizeof(struct sockaddr));
  memset(&server2,0,sizeof(struct sockaddr));  

  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[1]));//wejscie np. port 1111
  server.sin_addr.s_addr = INADDR_ANY;

  server2.sin_family = AF_INET;
  server2.sin_port = htons(atoi(argv[2]));//wyjscie np. port 1112
  server2.sin_addr.s_addr = INADDR_ANY;  

  int one = 1;
  setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));   //ustawianie ponownego uzywania gniazd, zeby nam wszystkich nie zablokowalo lul
  //setsockopt(sock1_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

  setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));   //ustawianie ponownego uzywania gniazd, zeby nam wszystkich nie zablokowalo lul
 // setsockopt(sock2_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));  

  k = bind(sock1,(struct sockaddr*)&server,sizeof(server));
  k2 = bind(sock2,(struct sockaddr*)&server2,sizeof(server2));  
    printf("Bind result %d i %d\n",k,k2);
  k = listen(sock1,3);
  k2= listen(sock2,3);  
  if(k == -1 || k2==-1)
    {
      printf("Listen failed");
      exit(1);
    }

  pthread_t thread_id;
  while((sock1_int = accept(sock1,NULL, NULL)) && (sock2_int = accept(sock2,NULL,NULL))){
    if (sock1_int < 0)
     {
        fprintf(stderr, "Błąd przy 1 próbie połączenia z klientem.\n");
        exit(1);
     }
    if (sock2_int < 0)
     {
        fprintf(stderr, "Błąd przy 2 próbie połączenia z klientem.\n");
        exit(1);
     }
    puts("Connection accepted");
    //stworz drugie gniazdo i wpisz oba gniazda do struktury
    struct thread_data_t t_data;
    t_data.sockinout=sock1_int;
    t_data.sockcmd=sock2_int;
    if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &t_data) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    puts("Handler assigned");
    pthread_detach(thread_id);
   }

  //freeMemory(buf_temp);  
  close(sock1);close(sock2);
}
