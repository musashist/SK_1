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


char* concat(const char *s1, const char *s2){ //ewentualne dodawanie stringow
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* divide (char * str, char * temp_buf) {
  char *ptr;
  strtok_r (str, " ", &ptr);
  int i = 0;
  printf ("%s\n",ptr);
  for (ptr; *ptr != '\0';++ptr)
    {
         temp_buf[i] = *ptr;
         i+=1;
    }

  return temp_buf;
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


int main(int argc,char *argv[])
{
  struct sockaddr_in server, server2,klient1;  //struktura na adres
  int sock1,sock1_int,sock2,sock2_int,sock3,sock3_int;                    //gniazda
  int k,k2,i,wynik;
  char buf[10000];
  char buf2[30];
  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  sock2 = socket(AF_INET, SOCK_STREAM, 0);
  sock3 = socket(AF_INET, SOCK_STREAM, 0);


  DIR *dir;
  struct dirent *ent;
  char path_curr[100] = "/home/seba/Documents/github/SK_1/";
  char path_curr2[100] = "/home/seba/Documents/github/";
  //getcwd(path_curr, sizeof(path_curr));

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

  server2.sin_family = AF_INET;
  server2.sin_port = htons(atoi("1112"));
  server2.sin_addr.s_addr = INADDR_ANY;

  klient1.sin_family = AF_INET;
  inet_pton(AF_INET,"127.0.0.1",&(klient1.sin_addr));
  klient1.sin_port = htons(atoi("1113"));



  int one = 1;
  setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));   //ustawianie ponownego uzywania gniazd, zeby nam wszystkich nie zablokowalo lul
  setsockopt(sock1_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  setsockopt(sock2_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  setsockopt(sock3, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  setsockopt(sock3_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

  k = bind(sock1,(struct sockaddr*)&server,sizeof(server));
  k2 = bind(sock2,(struct sockaddr*)&server2,sizeof(server2)); //adres do gniazda

 //adres do gniazda
  if(k2 == -1)
    {
      printf("Binding error");
      exit(1);
    }

  k = listen(sock1,3);
  k2 = listen(sock2,3);
  if(k2 == -1)
    {
      printf("Listen failed");
      exit(1);
    }

  i = 1;
  sock1_int = accept(sock1,NULL, NULL);
  sock2_int = accept(sock2,NULL,NULL);
  
  sock3_int = connect(sock3, (struct sockaddr*)&klient1, sizeof(struct sockaddr));
  if (sock3_int < 0)
   {
      fprintf(stderr, "%s: Błąd przy próbie połączenia z klientem (1113).\n");
      exit(1);
   }
  

  char litera,c;
  int l = 0;
  char buffer[100];
  FILE *fp;
  ssize_t data_read;
  const char a = ' ';

  while(1){
    l = 0;
    while ((data_read = recv(sock1_int, &c, 1, 0)) > 0 && c != '\n'){
      buf[l] = c;
      l++;
    }
    char** buf_temp = divide2(buf,a);
    printf("%s\n", buf_temp[0]);
    //printf("%s\n", buf_temp[1]);




        if(strcmp(buf, "cd cd") == 0){      //*sprawdzic czy buf3 i buf4 sie czyszcza, dodac sprawdzanie czy to directory i dodac slash
          char buf3[10] = "cd ikono";
          char buf4[20];
          divide(buf3,buf4);
          strcpy(buf3,concat(path_curr,buf4));
          printf("buf3 %s\n",buf3);
        }


        else if (strcmp(buf_temp[0], "ls") == 0){             //dios mio, to listuje pliki z danego folderu
          if ((dir = opendir (path_curr)) != NULL) {

              while ((ent = readdir (dir)) != NULL) {
                  strcpy(buf2,ent->d_name);
                  char *dot = strrchr(buf2, '.');
                  if (dot && !strcmp(dot, ".c")){
                    printf ("%s\n", buf2);
                  }
                }

                closedir (dir);
          } 
              else {
                perror ("");
                return EXIT_FAILURE;
              }
        }

        else if (strcmp(buf_temp[0], "pwd") == 0){          //printuje obecny folder
          printf("%s\n", path_curr);
        }

        else if (strcmp(buf_temp[0], "mkdir")== 0){          //printf("%s\n",buf_temp[1]);
          mkdir(buf_temp[1], S_IRWXU);
        }

        else if (strcmp(buf_temp[0], "push2") == 0){         //odbiera plik od klienta
          int amount;
          printf("push_zaczeto\n");

          amount = read(sock2_int,buffer,10);
          printf("read zakonczono\n" );

          fp=fopen(concat(path_curr2 ,buf_temp[1]),"w");
          printf("%s\n", buffer);
          fwrite(buffer,1,amount,fp); 
          printf("push_zakonczono\n");
          fclose(fp);
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

          send(sock3, buffer, l, 0);
          printf("the file was sent successfully\n");  


        }
        else{

          printf("nieprawidlowa komenda\n");

        }
      printf("%s\n",buf);
      memset(&buf[0], 0, sizeof(buf));    //czyscimy bufor
      printf("podaj komende meeen\n");


    
  }

}