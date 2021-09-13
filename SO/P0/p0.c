/*
*AUTOR 1 Martin Vieites Garcia martin.vieites
*Autor 2 Xabier Jimenez Gomez xabier.jimenez.gomez
*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#define MAX 100
#define NULLH -1


int TrocearCadena( char * cadena, char * trozos[]);

void fAuthors (char *trozos[]);
void fChdir (char *trozos[]);

void fGetPid ();
void fGetPpid ();
void fDate ();
void fTime ();
void fPwd ();
bool fEnd ();

typedef struct
{
    char *historic[MAX];
    int lastH;

}typeHistoric;

bool fHistoric(int tam, char *trozos[],typeHistoric *feed,int *counter);
void createHistoric(typeHistoric *historic);
bool insertH(typeHistoric *historic,char *trozos);
void inicializarCount(int *counter);
void printH(typeHistoric *historic,int *counter);
bool clearH(typeHistoric *historic);



int main() {

    char cadena[50];
    char *trozos[100];
    bool terminado = false;

    typeHistoric *feed;
    feed=(typeHistoric*) malloc (409600*sizeof(typeHistoric));
    int *c;
    c=(int*) malloc (409600*sizeof(int));
    int tam = 0;

    createHistoric(feed);
    inicializarCount(c);

    while (!terminado) {

        printf("$ ");
        fgets(cadena, 50, stdin);

        tam = TrocearCadena(cadena, trozos);

        if (trozos[0] == NULL){

            terminado = false;

        }else{

            insertH(feed,cadena);
            *c = *c+1;

            if (strcmp(trozos[0], "authors") == 0) {

                fAuthors(trozos);

            } else if (strcmp(trozos[0], "getpid") == 0) {

                fGetPid();

            } else if (strcmp(trozos[0], "getppid") == 0) {

                fGetPpid();

            } else if(strcmp(trozos[0], "pwd") == 0) {

                fPwd();

            } else if(strcmp(trozos[0], "chdir") == 0) {

                fChdir(trozos);

            } else if(strcmp(trozos[0],"date")==0){

                fDate();

            }else if(strcmp(trozos[0],"time")==0){

                fTime();

            }else if (strcmp(trozos[0],"historic") == 0) {

                fHistoric(tam, trozos, feed, c);

            }else if (strcmp(trozos[0], "quit") == 0 || strcmp(trozos[0], "end") == 0 || strcmp(trozos[0], "exit") == 0 ){

                terminado = fEnd();

            }else {

                printf("Comando desconocido \n");
            }
        }
    }
    free(feed);
    free(c);

    return 0;

}

int TrocearCadena (char * cadena, char * trozos []){

    int i = 1;

    if ((trozos[0] = strtok(cadena," \n\t")) == NULL)
        return 0;

    while ((trozos[i] = strtok(NULL," \n\t")) != NULL)
        i++;

    return i;
}

/************************/

void createHistoric(typeHistoric *feed){

    feed->lastH=NULLH;
}

bool insertH(typeHistoric *feed,char *trozos){

    char *cadena2=strdup(trozos);
    if(feed->lastH==MAX-1)
        return false;
    else{
        feed->lastH++;
        feed->historic[feed->lastH]=cadena2;
        return true;
    }

}

void inicializarCount(int *counter){
    counter=0;
}

bool clearH(typeHistoric *feed){

    feed->lastH=NULLH;
    return true;
}

/************************/


void fAuthors(char *trozos[]){

    if (trozos[1] == NULL) {

        printf("Martin Vieites \t martin.vieites \n");
        printf("Xabier Jimenez \t xabier.jimenez.gomez \n");


    } else if (strcmp(trozos[1], "-l") == 0 && trozos[2] == NULL) {

        printf("martin.vieites , xabier.jimenez.gomez \n");

    } else if (strcmp(trozos[1], "-n") == 0 && trozos[2] == NULL) {

        printf("Martin Vieites , Xabier Jimenez \n");

    } else {

        printf("Argumentos incorrectos \n");

    }

}

void fGetPid(){

    pid_t process_id;
    process_id = getpid();

    printf("The process id: %d\n",process_id);

}

void fGetPpid() {

    pid_t p_process_id;
    p_process_id = getppid();

    printf("The process id of parent function: %d\n",p_process_id);

}

void fPwd (){

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    printf("Current working dir: %s\n", cwd);

}

void fChdir (char *trozos[]) {

    char cwdcmp[1024];

    if (trozos[1] == NULL) {

        char cwd[1024];
        getcwd(cwd, sizeof(cwd));

        printf("Current working dir: %s\n", cwd);

    } else if (strcmp(trozos[1], "/root") == 0){

        printf("Cannot change dir /root: permission denied \n");

    } else {

        char dir[1024];
        strcpy(dir, trozos[1]);
        chdir(dir);

        if (strcmp(getcwd(cwdcmp, sizeof(cwdcmp)), dir) == 0){

            printf("Current working directory changed to: %s\n", dir);

        } else {

            printf("Imposible cambiar directorio: No such file or directory \n");

        }

    }

}

void fDate (){

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("%02d-%02d-%02d\n",tm.tm_mday,tm.tm_mon + 1,tm.tm_year + 1900);


}

void fTime (){

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("%02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

}

bool fHistoric(int tam, char *trozos[],typeHistoric *feed,int *counter){

    if(tam == 1){

        for(int i=0; i < *counter; i++){
            printf("%d->%s\n", i, feed->historic[i]);
        }

    }else if(strcmp(trozos[1],"-c")==0){

      if(tam==2){
  if (strncmp(trozos[1],"-0", 2)==0){

    printf("%d->%s\n", 0, feed->historic[0]);
    return true;
  }else{

    int num=atoi(trozos[1]);

    if(num <0){
      int to = num*(-1);
      if(to<=*counter){
        for(int i=0; i <=to; i++){
          printf("%d->%s\n", i, feed->historic[i]);
        }
        return true;
      }else return false;

    }else return false;
    return false;
  }

}else return false;

    }
return true;
}

bool fEnd (){

    bool terminado = true;
    return terminado;


}
