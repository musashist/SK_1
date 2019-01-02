#ifndef HEADER1_HEADERS
#define HEADER1_HEADERS

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#define REQ_COMMAND_LENGTH 20
#define REQ_ARG_LENGTH 20
#define MAX_NR_ARG 20

int dlugosc(char a[],char b[])
{
    size_t aw,bw;
    aw=0;bw=0;
    aw=strlen(a);
    bw=strlen(b);
    if(aw==bw)return 0;
    else if(aw>bw)return 1;
    else return -1;
}

void parse(char line[],char *in[10])
{
   const char *ptr = line;
  // char **wyn=0;
   char field [ 32 ];
   int n,licz,p;
   licz=0; 
   const char *j=line; 
   //char **wyn=NULL;    
    //printf("My character: %d\n",(int)line[0]);
   //policz ile jest delim 
   while(*j){
        if(*j == ' ')licz++;
        j++;
    }     
   // wyn = malloc(sizeof(char*)*(licz+1));
   // printf("%zu licz: %d %s\n",strlen(line),licz,ptr);
    p=0;
   if(*ptr=='\n')printf("hey");
    p=0;
   //przesun sie do przodu jesli sa jakies spacje na poczatku
 //  while(*ptr == ' ')ptr++; 
   while ( sscanf(ptr, "%31[^ \n]%n", field, &n) == 1 )
   { 
     // printf("\n >%d%s\n",p,field);  
     // wyn[p]=malloc(sizeof(char)*(n+1));
      in[p]=NULL;
      in[p]=malloc(sizeof(char)*(n+2));
      //memset(&in[p], 0, sizeof(in[p]));       
   //   for(int u=0;u<=n;u++)  
     //   in[p][u]='\0';
      strcpy(in[p],field);      
      printf("%d wyn[%d] =%s-\n",n,p,in[p]);
      p++;
      ptr += n; /* advance the pointer by the number of characters read */
      
      if ( *ptr != ' ')
      {
         break; /* didn't find an expected delimiter, done? */
      }
     // ++ptr; /* skip the delimiter */
       while(*ptr==' ')ptr++;   // omin rozdzielacz, czyli spacje    
        printf("nic");  
   }
 //   return wyn;
}

int ile(char *w[10])
{
    int wyn=0;
    while(w[wyn])wyn++;
    return wyn;
}

char* concat(const char *s1, const char *s2){ //ewentualne dodawanie stringow
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
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
    while(*rest) //iterujemy po wszystkich znakach
    {
        if (s_delim== *rest){licz++;lastsep=rest;}
        rest++;
    }     
    
    // dodaj miejsce na zapasowe zetony. 
    licz += lastsep < (in + strlen(in) - 1);

    // miejsce na null string
     //  aby wiedziec kiedy koniec. 
    licz++;

    wynik= malloc(sizeof(char*)*(licz));
    while((token=strtok_r(tmp,delim,&tmp))) //iterujemy po wszystkich wyrazach
    {
        *(wynik+idx)=strdup(token);//zapisujemy do tablicy wyjsciowej       
        idx++;
    }
    return wynik;
}

void freeMemory(char *tokens[10])
{
    int i,licz;
    licz=0;    
    licz=ile(tokens); //licze ile jest tokenow    
    printf("Tyle usunę: %d\n",licz);
    //usuwam z pamieci 
    if(tokens){    
    for(i=0;i<licz;i++)
    {
        //memset((tokens+ile),0,sizeof(char *));
        free(tokens[i]);
    }    
//    free(tokens);
    }
}
/*
void freeMemory2(char **tokens)
{
    int i,licz;
    licz=0;    
    licz=ile(tokens); //licze ile jest tokenow    
    printf("Tyle usunę: %d\n",licz);
    //usuwam z pamieci 
    if(tokens){    
    for(i=0;i<licz;i++)
    {
        //memset((tokens+ile),0,sizeof(char *));
        free(tokens[i]);
    }    
    free(tokens);}
}*/

void removeFile(const char *filepath)
{
    int ret;    
    FILE *fp;
    fp= fopen(filepath,"r");
    //jesli moge otworzyc plik , to znaczy ze jest i moge go usunac    
    if(fp)
    {
        fclose(fp);
        ret = remove(filepath);
        if (ret==0)
           printf("File deleted successfully\n"); 
    }
    else 
        printf("Can't open a file\n"); 
}
int removeFolder(const char *path)
{
    printf("%s \n",path);
    char *peln_sciezka;
    DIR *dir;
    struct stat stat_sciez,stat_entry;
    struct dirent *ent;
    //statystyki dla sciezki
    stat(path,&stat_sciez);
    //jesli to nie jest sciezka folderu    
    if (S_ISDIR(stat_sciez.st_mode) == 0 || (dir = opendir(path)) == NULL)
    {
        fprintf(stderr, "%s: %s\n", "Can`t open directory", path);
        return 1;    
    }   
    printf("jest \n"); 
    //przechodzimy po grafie i usuwamy wszystkie podkatalogi i pliki 
    while((ent = readdir (dir)) != NULL)
    {
        // pomin wejscia "." and ".."
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;


        peln_sciezka=malloc(strlen(path)+strlen(ent->d_name)+1);        
        strcpy(peln_sciezka,concat(path,"/"));
        printf("jest 2\n");        
        strcat(peln_sciezka,ent->d_name);
        //statystyki
        stat(peln_sciezka,&stat_entry);

        //rekurencja
        // jesli mamy kolejny folder to wchodzimy do niego
        if (S_ISDIR(stat_entry.st_mode) != 0) {
            removeFolder(peln_sciezka);
            continue;
        }
        //usuwamy plik
         if (unlink(peln_sciezka) == 0)
            printf("Usunieto plik: %s\n", peln_sciezka);
        free(peln_sciezka);
    }
    //usuwamy pusty folder
    if(rmdir(path)==0)
       printf("Usunieto folder: %s\n",path);

    closedir(dir);
    return 0;
}

#endif
