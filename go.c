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
  //strcpy(temp_buf,ptr);
  return temp_buf;
}



int main(int argc,char *argv[])
{
  struct sockaddr_in server, server2;  //struktura na adres
  int sock1,sock1_int,sock2,sock2_int;                    //gniazda
  int k,k2,i,wynik;
  char buf[100];
  char buf2[30];
  sock1 = socket(AF_INET, SOCK_STREAM, 0);
  sock2 = socket(AF_INET, SOCK_STREAM, 0);


  DIR *dir;
  struct dirent *ent;
  char path_curr[100] = "/home/pener/";
  //getcwd(path_curr, sizeof(path_curr));

  if(sock2 == -1)
    {
      printf("Socket creation failed");
      exit(1);
    }

  memset(&server,0,sizeof(struct sockaddr));
  memset(&server2,0,sizeof(struct sockaddr));

  server.sin_family = AF_INET;
  server.sin_port = htons(atoi("1111"));
  server.sin_addr.s_addr = INADDR_ANY;

  server2.sin_family = AF_INET;
  server2.sin_port = htons(atoi("1112"));
  server2.sin_addr.s_addr = INADDR_ANY;

  int one = 1;
  setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));   //ustawianie ponownego uzywania gniazd, zeby nam wszystkich nie zablokowalo lul
  setsockopt(sock1_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  setsockopt(sock2_int, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));



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
  char litera;
  int l = 0;
  char buffer[100];
  FILE *fp;
  while(1){
    //l = 0;
    read(sock1_int,buf,10);
    /*
    while(litera = getc(sock1_int) != ("q" || NULL)) {
      printf("%c\n",litera );
      buf[l] = litera;
      l+=1;
    }
  */
    //printf("Przed wyczyszczeniu %s\n",buf);

        if(strcmp(buf, "cd cd\n") == 0){      //*sprawdzic czy buf3 i buf4 sie czyszcza, dodac sprawdzanie czy to directory i dodac slash
          char buf3[10] = "cd ikono";
          char buf4[20];
          divide(buf3,buf4);
          strcpy(buf3,concat(path_curr,buf4));
          printf("buf3 %s\n",buf3);
          //printf("%s\n",buf4);
          //memset(&buf3[0], 0, sizeof(buf3));
          //memset(&buf4[0], 0, sizeof(buf4));
          //printf("cd\n");
        }


        else if (strcmp(buf, "ls\n") == 0){             //dios mio, to listuje pliki z danego folderu
          if ((dir = opendir (path_curr)) != NULL) {

              while ((ent = readdir (dir)) != NULL) {
                  strcpy(buf2,ent->d_name);
                  //buf2 = ent->d_name;
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

        else if (strcmp(buf, "pwd\n") == 0){          //printuje obecny folder
          printf("%s\n", path_curr);
        }

        else if (strcmp(buf, "push\n") == 0){         //odbiera plik od klienta
          //char bufor2[5] = "tak\n";
          //write(sock1,bufor2,strlen(bufor2));
          printf("push_zaczeto\n");
          //printf("zmienne zakonoczno\n" );

          /*
          while(read(sock2_int,litera,1)>0){
            printf("%c\n",litera);
            buffer[l] = litera;
            l+=1;
          }
          */
          //printf("read zaczeto" );
          read(sock2_int,buffer,10);
          printf("read zakonczono\n" );
          //printf("%s\n",buffer );
          //read(sock1_int,buffer,100);
          fp=fopen("add1.txt","w");
          printf("%s\n", buffer);
          fwrite(buffer,1,5,fp); 
          printf("push_zakonczono\n");
          fclose(fp);
        }
        else{
          //printf("%s\n",buf );
          printf("nieprawidlowa komenda\n");
          //sleep(10);
        }
      memset(&buf[0], 0, sizeof(buf));    //czyscimy bufor
      //printf("Po wyczyszczeniu %s\n",buf);

    
  }

}