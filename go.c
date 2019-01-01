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
int freds = 0;
struct arg_struct {
    int socket_desc;
    int socket_desc2;
    int socket_desc3;
};

void *connection_handler(void *socket_desc)
{
  int sock1 = *(int*)socket_desc; /// gwiazdka int gwiazdka
  int sock2, sock3,k2;
  int sock2_int, sock3_int,k;
  //sock2 = socket(AF_INET, SOCK_STREAM, 0);
  //sock3 = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server2,klient1;
  memset(&server2,0,sizeof(struct sockaddr));
  memset(&klient1,0,sizeof(struct sockaddr));

  server2.sin_family = AF_INET;
  server2.sin_port = htons(1112);
  server2.sin_addr.s_addr = INADDR_ANY;

  klient1.sin_family = AF_INET;
  inet_pton(AF_INET,"127.0.0.1",&(klient1.sin_addr));
  klient1.sin_port = htons(1113);
  int one = 1;

  //setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  //setsockopt(sock2_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  //setsockopt(sock3, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  //setsockopt(sock3_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

  //k2 = bind(sock2,(struct sockaddr*)&server2,sizeof(server2)); //adres do gniazda
  //k2 = listen(sock2,3);

  //sock2_int = accept(sock2,NULL,NULL);
 // sock3_int = connect(sock3, (struct sockaddr*)&klient1, sizeof(struct sockaddr));

  char buf[10000];
  char buf2[10000];
  char buf3[10000]; 
  char buf4[100];
  DIR *dir;
  struct dirent *ent;
  char path_curr[100] = "/home/seba/Documents/github/SK_1/";
  char path_curr2[100],path[100];
  getcwd(path, sizeof(path));
  strcpy(path_curr2,concat(path,"/"));

  char litera,c;
  int lic = 0,cmp;
  char buffer[100];
  FILE *fp;
  ssize_t data_read,l2,l1;
  const char a = ' ';
  char *buf_temp[10];l2=0;
  memset(&buf_temp, 0, sizeof(buf_temp));   
  memset(&buf2, 0, sizeof(buf2));
  puts("bobobobob");

  while(1){
    lic = 0;
    printf("strlen: %d\n",strlen(buf));
    while ((data_read = read(sock1, &c, 1)) > 0 && c != '\n'){    
      buf[lic] = c;
      lic++; 
   
    printf("buf: %s||buf2: %s\n",buf,buf2);

  }
  if(strlen(buf2)==0)
    {
        printf("start!\n"); 
        strcpy(buf3,buf);
        strcpy(buf2,buf);     
    }
    else{

    cmp=dlugosc(buf,buf2);    
    printf("wyn_cmp: %d \n",cmp);  

    if(cmp==0)
    {
            printf("podalem tylko enter\n");
            memset(&buf3, 0, sizeof(buf3));    
            printf("Dlugosc %zu\n",strlen(buf3));
    } 
    else if(cmp>0){printf("więcej w buforze 1 %zu > %zu \n",strlen(buf),strlen(buf2));
        //odczytaj dlugosc buf2 
        l2=strlen(buf2);
        //odczytaj dlugosc buf1
        l1=strlen(buf);
        memset(&buf3, 0, sizeof(buf3)); 
        memset(&buf2, 0, sizeof(buf2));
        //przepisz do buf3 // l1 > l2
        int ind=(int)l2;
        while(ind<(int)l1){buf3[ind-(int)l2]=buf[ind];ind++;}
        //kopiuj do buf2 z buf dopóki nie ma ' '
        ind=(int)l2;
        while(ind<(int)l1 && buf[ind]!=' '){buf2[ind-(int)l2]=buf[ind];ind++;}
    }
    else {//jakos sie to stalo ?
            memset(&buf3, 0, sizeof(buf3)); 
            memset(&buf2, 0, sizeof(buf2));
            strcpy(buf2,buf);
            printf("jak to sie stalo nie wiem\n");
        }
    
    }
    parse(buf3,buf_temp);
    printf("buf_temp[0] %s|| cmp: %d || siz: %zu\n",buf_temp[0],cmp,strlen(buf3));

    if(lic>0 && buf_temp[0]!=NULL && strlen(buf3)!=0) 
    {
        //buf_temp = divide2(buf,a);
        printf("Jaka komenda: %s\n",buf_temp[0]);        
        //printf("%s-\n",buf_temp[1]);
        if(strcmp(buf_temp[0], "cd") == 0 && buf_temp[1]!=NULL){
          if (strcmp(buf_temp[0],"cd") == 0 && (strcmp(buf_temp[1],"..")==0 ||strcmp(buf_temp[1],"../")==0)){
                  printf("wszedlem \n");
                  char curpath[100];//=malloc(strlen(path_curr2)*sizeof(char));
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
                  //cast to const char (maybe)    
                  if(chdir(pom_path)==0){
                        memset(&path_curr2[0], 0, sizeof(path_curr2));
                        strcpy(path_curr2,concat(pom_path,"/"));
                        printf("Zmieniłem folder na: %s \n",path_curr2);                       
                  }

        }
        else {
            //zmien folder
            char pom[100];
            strcpy(pom,path_curr2);
            if(chdir(concat(pom,buf_temp[1]))==0){
                memset(&path_curr2[0], 0, sizeof(path_curr2));
                strcpy(pom,concat(pom,buf_temp[1]));
                strcpy(path_curr2,concat(pom,"/"));
                printf("Zmieniłem folder na: %s \n",path_curr2);
            }            
            
           }
        }
        else if (strcmp(buf_temp[0], "ls") == 0){             //dios mio, to listuje pliki z danego folderu
          if ((dir = opendir (path_curr2)) != NULL) {

              while ((ent = readdir (dir)) != NULL) {
                  strcpy(buf4,ent->d_name);
                //  char *dot = strrchr(buf2, '.');
                //  if (dot && !strcmp(dot, ".c")){
                    printf ("%s\n", buf4);
                //  }
                }
                closedir (dir);
          } 
              else {
                perror ("");
                return EXIT_FAILURE;
              }
        }

        else if (strcmp(buf_temp[0], "pwd") == 0){          //printuje obecny folder
          printf("%s\n", path_curr2);
        }

        else if (strcmp(buf_temp[0], "mkdir")== 0 && buf_temp[1]!=NULL){          //printf("%s\n",buf_temp[1]);
          mkdir(buf_temp[1], S_IRWXU);
        }
        else if (strcmp(buf_temp[0], "push2") == 0 ){//&& buf_temp[1]!=NULL){         //odbiera plik od klienta
          int amount;
          char a[2];   
          FILE *f1;
          f1=NULL;  
          if(buf_temp[1]!=NULL && strcmp(buf_temp[1],"")!=0)
          f1=fopen(concat(path_curr2 ,buf_temp[1]),"w");
          if(f1){
                printf("zaczynam pobierac dane\n");
                do
                {
                      amount=read(sock1, buffer,100);  
                      printf("%d\n",amount);  
                      fwrite(buffer,1,amount,f1);
                }
                while(amount==100);                                    
                fclose(f1);
          }

          printf("push_zakonczono\n");
        }
        else if (strcmp(buf_temp[0], "pull") == 0){
          printf("pull zaczeto\n");
          char buffer[100];
          int n,c,l;
          n = 0;
          l=0;
          char d;
          FILE *file = fopen(concat(path_curr2, buf_temp[1]), "r");
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

          send(sock1, buffer, l, 0);
          printf("the file was sent successfully\n");  


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
            char *sciezka=malloc(strlen(path_curr2)+strlen(buf_temp[1])+1);
            strcpy(sciezka,concat(path_curr2,buf_temp[1]));           
            removeFolder(sciezka);
            free(sciezka);
        }
        else if (strcmp(buf_temp[0], "rm") == 0 && buf_temp[1]!=NULL){
            char *sciezka=malloc(strlen(path_curr2)+strlen(buf_temp[1])+1);
            strcpy(sciezka,concat(path_curr2,buf_temp[1]));           
            removeFile(sciezka);
            free(sciezka);        
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
  struct sockaddr_in server, server2,klient1;  //struktura na adres
  int sock1,sock1_int,sock2,sock2_int,sock3,sock3_int;                    //gniazda
  int k,k2,i,wynik;
  char buf[10000];
  char buf2[10000];
  char buf3[10000]; //pomocnicze  
  char buf4[100];
  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  sock2 = socket(AF_INET, SOCK_STREAM, 0);
  sock3 = socket(AF_INET, SOCK_STREAM, 0);


  DIR *dir;
  struct dirent *ent;
  char path_curr[100] = "/home/seba/Documents/github/SK_1/";
  char path_curr2[100],path[100];//= "/home/seba/Documents/github/";
  getcwd(path, sizeof(path));
  strcpy(path_curr2,concat(path,"/"));

  printf("%s\n",path_curr2);
  if(sock3 == -1)
    {
      printf("Socket creation failed");
      exit(1);
    }

  memset(&server,0,sizeof(struct sockaddr));
  memset(&server2,0,sizeof(struct sockaddr));
  memset(&klient1,0,sizeof(struct sockaddr));

  server.sin_family = AF_INET;
  server.sin_port = htons(atoi("1111"));
  server.sin_addr.s_addr = INADDR_ANY;

  int one = 1;
  setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));   //ustawianie ponownego uzywania gniazd, zeby nam wszystkich nie zablokowalo lul
  setsockopt(sock1_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

  k = bind(sock1,(struct sockaddr*)&server,sizeof(server));


  k = listen(sock1,3);
  if(k == -1)
    {
      printf("Listen failed");
      exit(1);
    }

  i = 1;

  char litera,c;
  int lic = 0,cmp;
  char buffer[100];
  FILE *fp;
  ssize_t data_read,l2,l1;
  const char a = ' ';
  //char** buf_temp;
  char *buf_temp[10];l2=0;
  memset(&buf_temp, 0, sizeof(buf_temp));   
  memset(&buf2, 0, sizeof(buf2));
  pthread_t thread_id;
  while((sock1_int = accept(sock1,NULL, NULL))){
    freds +=1;
    if (sock1_int < 0)
     {
        fprintf(stderr, "%s: Błąd przy próbie połączenia z klientem (1111).\n");
        exit(1);
     }
    puts("Connection accepted");

    if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &sock1_int) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    puts("Handler assigned");
    pthread_detach(thread_id);
   }

  freeMemory(buf_temp);  
  close(sock1);
  //close(sock2);
}
