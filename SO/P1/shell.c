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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#define MAX 100
#define NULLH -1


int TrocearCadena( char * cadena, char * trozos[]);

void fAuthors (char *trozos[]);
void fChdir (char *trozos[]);

void showList();
void fGetPid ();
void fGetPpid ();
void fDate ();
void fTime ();
void fPwd ();
bool fEnd ();

int exists(char *fname);
int remove_directory(const char *path);

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

void listDirShort(const char * path, bool hid);
int TrocearCadena (char * cadena, char * trozos []);
void listDirWithVariables( char * flags , char * argDir );
void listFileWithVariables ( char * flags, char * files );
char * ConvierteModo (mode_t m);
char LetraTF (mode_t m);

void listFilShort (const char * file);
void listFilLong (const char * file);

void listDirLong(const char * path, bool hid);
void listDirShort(const char * path, bool hid);
void listDirRecShort(const char *name, bool hid);
void listDirRecLong(const char *name, bool hid);

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

      } else if(strcmp(trozos[0],"time")==0){

        fTime();

      } else if (strcmp(trozos[0],"historic") == 0) {

        fHistoric(tam, trozos, feed, c);

      } else if ( strcmp(trozos[0], "create") == 0 ) {

        if ( trozos[1] == NULL ) {

          showList();

        } else if ( strcmp(trozos[1], "-dir") == 0 ) {

          if (trozos[2] == NULL){

            showList();

          } else {

            int check;
            char *dirname = trozos[2];

            check = mkdir(dirname, 0775);

            if (check) {

              printf("Unable to create directory\n");
              perror("Following error ocurred");
            }
          }
        }
      } else if ( strcmp(trozos[0], "delete") == 0 ) {

        if (trozos[1] == NULL){

          showList();

        } else if ( strcmp(trozos[1], "-rec") == 0 ){

          if (trozos[2] == NULL) {

            showList();

          }

          int check;

          check = remove_directory(trozos[2]);

          if (check) {

            check = remove(trozos[2]);

            if (check) {

              printf("Unable to delete %s \n", trozos[2]);
              perror("Following error occurred");

            }

          }

        } else {

          int check;

          check = remove(trozos[1]);

          if (check){

            printf("Unable to delete %s \n", trozos[1]);
            perror("Following error occurred");
          }

        }


      } else if ( strcmp(trozos[0], "list") == 0 ){

        if ( trozos[1] == NULL ) {

          listDirShort(".", false);

        } else if (strcmp (trozos[1], "-dir") == 0) {

          int i = 2;

          while (trozos[i] != NULL) {

            listDirShort(trozos[i], false);
            i++;

          }

        } else {

          int i = 1;
          char flags[6] = {};
          char argDir[20] = {};
          int posDir = -1;
          strcpy(flags, "---- ");

          while (trozos[i] != NULL) {

              if (strcmp(trozos[i], "-dir")==0) {flags[0] = 'd'; posDir = i+1; if ( trozos[posDir] != NULL ) strcpy(argDir ,trozos[posDir]);}
              else if (strcmp(trozos[i], "-hid")==0) flags[1] = 'h';
              else if (strcmp(trozos[i], "-long")==0) flags[2] = 'l';
              else if (strcmp(trozos[i], "-rec")==0) flags[3] = 'r';
              else{};
              i++;

          }

          listDirWithVariables( flags, argDir );


        }

      } else if (strcmp(trozos[0], "quit") == 0 || strcmp(trozos[0], "end") == 0 || strcmp(trozos[0], "exit") == 0 ){

        terminado = fEnd();

      } else {

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

}

bool fEnd (){

    bool terminado = true;
    return terminado;


}

int remove_directory(const char *path) {
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d) {
      struct dirent *p;

      r = 0;
      while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;

          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;

          len = path_len + strlen(p->d_name) + 2;
          buf = malloc(len);

          if (buf) {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = remove_directory(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }

   if (!r)
      r = rmdir(path);

   return r;}

void showList(){

DIR *folder;
struct dirent *entry;
struct stat filestat;

printf("\n");
folder = opendir(".");
if(folder == NULL)
{
    perror("Unable to read directory");
}

/* Read directory entries */
while( (entry=readdir(folder)) ) {

    stat(entry->d_name,&filestat);
    if( S_ISDIR(filestat.st_mode) )
        printf("%4s: %s \n","Dir",entry->d_name);
    else
        printf("%4s: %s \n","File",entry->d_name);
}
printf("\n");
closedir(folder);}

void listDirWithVariables( char * flags , char * argDir ) {

  char path[50];
  bool hid = false;
  bool lon = false;
  bool rec = false;

  if (flags[0] =='d') {

    strcpy(path, argDir);

  } else {

    strcpy (path, ".");

  }

  if (flags[1] == 'h') { hid = true; }
  if (flags[2] == 'l') { lon = true; }
  if (flags[3] == 'r') { rec = true; }

    if (lon==true) {

      if (rec==true){ listDirRecShort(path, hid); }    // Deber??a ser long pero da un error;
      else { listDirLong (path, hid); }

    } else if (lon==false) {

      if (rec==true){ listDirRecShort(path, hid); }
      else { listDirShort (path, hid); }

    }
    return;
}

void listFileWithVariables ( char * flags, char * files ) {

  bool lon = false;

  if (flags[2] == 'l') { lon = true; }

  if ( lon ) {

    listFilLong( files );

  } else if ( !lon ) {

    listFilShort( files );

  }

}

void listDirShort( const char *path, bool hid ) {

    DIR *folder;
    struct dirent *entry;
    struct stat filestat;

    printf("\n");
    folder = opendir(path);
    if(folder == NULL)
    {
        perror("Unable to read directory");
        return;
    }

    printf("[%s]\n", path);
    while( (entry=readdir(folder)) ) {

      stat(entry->d_name,&filestat);

      if ( hid == true ) {

          printf("\t -: %s \n",entry->d_name);

      } else if (hid == false) {

        if(((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..")!=0)))
            printf("\t - %s \n",entry->d_name);

      }
        }

    printf("\n");
    closedir(folder);
}

void listDirLong( const char *path, bool hid ) {

    DIR *folder;
    struct dirent *entry;
    struct stat filestat;
    time_t tiempo;
    char permisos[12];
    char name [600];

    printf("\n");
    folder = opendir(path);
    if(folder == NULL)
    {
        perror("Unable to read directory");
    }

    while( (entry=readdir(folder)) ) {

        char buff[20];
        lstat(entry->d_name,&filestat);

        tiempo = filestat.st_mtime;
        struct passwd *user = getpwuid(filestat.st_uid);
        struct group *group = getgrgid(filestat.st_gid);
        char type;

        type = LetraTF(filestat.st_mode);
        strftime(buff, 20, "%a %d %H:%M", localtime(&tiempo));

        strcpy(permisos, ConvierteModo(filestat.st_mode));

        if (type == 'l') {

          char buffer[100];
          if (readlink( path ,buffer, sizeof(buffer)) <= 0) {
            sprintf(name, "%s -> No encontrado", entry->d_name);
            printf("%s \n", buffer );
          } else
          sprintf(name ,"%s -> %s", entry->d_name,buffer);

        } else {

          strcpy(name, entry->d_name);

        }
        if ( hid == true ) {

          printf("%6s %ld %6s %6s %s %6ld (%1ld) %s \n",buff, filestat.st_ino, user->pw_name, group->gr_name, permisos, filestat.st_size, filestat.st_nlink, name);

        } else if (hid == false ){

          if(((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..")!=0))){

            printf("%6s %ld %6s %6s %s %6ld (%1ld) %s \n",buff, filestat.st_ino, user->pw_name, group->gr_name, permisos, filestat.st_size, filestat.st_nlink, name);

          }

        }

        }

    printf("\n");
    closedir(folder);
}

void listDirRecShort(const char *basePath, bool hid) {

  char path[1000];
  struct dirent *dp;
  DIR *dir = opendir(basePath);

  // Unable to open directory stream
  if (!dir)
      return;

  printf("\n[%s]\n", basePath);
  while ((dp = readdir(dir)) != NULL) {

      if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
      {
          printf("%s\n", dp->d_name);

          // Construct new path from our base path
          strcpy(path, basePath);
          strcat(path, "/");
          strcat(path, dp->d_name);

          listDirRecShort(path, false);
      }
  }

  printf("\n");
  closedir(dir);

}

// void listDirRecLong(const char * basePath, bool hid) {
//
//   DIR *folder;
//   struct dirent *entry;
//   struct stat filestat;
//   time_t tiempo;
//   char permisos[12];
//   char name [600];
//   char path[1000];
//
//   // Unable to open directory stream
//   if (!basePath)
//       return;
//
//   printf("\n[%s]\n", basePath);
//   while ((entry = readdir(folder)) != NULL) {
//
//   if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
//
//     char buff[200];
//     lstat(entry->d_name,&filestat);
//
//     tiempo = filestat.st_mtime;
//     struct passwd *user = getpwuid(filestat.st_uid);
//     struct group *group = getgrgid(filestat.st_gid);
//     char type;
//
//     type = LetraTF(filestat.st_mode);
//     strftime(buff, 20, "%a %d %H:%M", localtime(&tiempo));
//
//     strcpy(permisos, ConvierteModo(filestat.st_mode));
//
//     if (type == 'l') {
//
//       char buffer[4096];
//       if (readlink( basePath ,buffer, sizeof(buffer)) <= 0) {
//         sprintf(name, "%s -> No encontrado", entry->d_name);
//         printf("%s \n", buffer );
//       } else
//       sprintf(name ,"%s -> %s", entry->d_name,buffer);
//
//     } else {
//
//       strcpy(name, entry->d_name);
//
//     }
//     if ( hid == true ) {
//
//       printf("%6s %ld %6s %6s %s %6ld (%1ld) %s \n",buff, filestat.st_ino, user->pw_name, group->gr_name, permisos, filestat.st_size, filestat.st_nlink, name);
//
//     } else if (hid == false ){
//
//       if(((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..")!=0))){
//
//         printf("%6s %ld %6s %6s %s %6ld (%1ld) %s \n",buff, filestat.st_ino, user->pw_name, group->gr_name, permisos, filestat.st_size, filestat.st_nlink, name);
//
//       }
//
//     }
//       strcpy(path, basePath);
//       strcat(path, "/");
//       strcat(path, entry->d_name);
//
//       listDirRecShort(path, false);
//       }
//   }
//
//   closedir(folder);
//
// }      Actualmente da un error de segmentacion

char LetraTF (mode_t m) {

  switch (m&S_IFMT) {
    case S_IFSOCK: return 's';
    case S_IFLNK: return 'l';
    case S_IFREG: return '-';
    case S_IFBLK: return 'b';
    case S_IFDIR: return 'd';
    case S_IFCHR: return 'c';
    case S_IFIFO: return 'p';

    default: return '?';
  }
}

char * ConvierteModo (mode_t m) {

  static char permisos[12];
  strcpy (permisos,"---------- ");
  permisos[0]=LetraTF(m);
  if (m&S_IRUSR) permisos[1]='r'; /*propietario*/
  if (m&S_IWUSR) permisos[2]='w';
  if (m&S_IXUSR) permisos[3]='x';
  if (m&S_IRGRP) permisos[4]='r'; /*grupo*/
  if (m&S_IWGRP) permisos[5]='w';
  if (m&S_IXGRP) permisos[6]='x';
  if (m&S_IROTH) permisos[7]='r'; /*resto*/
  if (m&S_IWOTH) permisos[8]='w';
  if (m&S_IXOTH) permisos[9]='x';
  if (m&S_ISUID) permisos[3]='s'; /*setuid, setgid y stickybit*/
  if (m&S_ISGID) permisos[6]='s';
  if (m&S_ISVTX) permisos[9]='t';
  return (permisos);
}

void listFilShort (const char * file) {

    //struct stat filestat;
    //lstat( file, &filestat );
    int check = 0;

    if ( check = open(file, O_RDONLY) == -1 ) {

      perror("Following error ocurred: ");

    } else {

      printf("-: %s \n", file);

    }

}

void listFilLong (const char * file) {

    struct dirent *entry;
    struct stat filestat;
    time_t tiempo;
    char permisos[12] = {};
    char name [6000] = {};

    if (!file){

        perror("Following error ocurred: ");
        return;
    }

      char buff[20];
      lstat(file,&filestat);

      tiempo = filestat.st_mtime;
      struct passwd *userf = getpwuid(filestat.st_uid);
      struct group *groupf = getgrgid(filestat.st_gid);
      char type;

      type = LetraTF(filestat.st_mode);
      strftime(buff, 20, "%a %d %H:%M", localtime(&tiempo));

      strcpy(permisos, ConvierteModo(filestat.st_mode));

      if (type == 'l') {

        printf("\n" );

        char buffer[4096];
        if (readlink( file ,buffer, sizeof(buffer)) <= 0) {
          sprintf(name, "%s -> No encontrado", entry->d_name);
          printf("%s \n", buffer );
        } else
        sprintf(name ,"%s -> %s", entry->d_name,buffer);

      } else {

        strcpy(name, file);

      }

      printf("%6s %ld %6s %6s %s %6ld (%1ld) %s \n",buff, filestat.st_ino, userf->pw_name, groupf->gr_name, permisos, filestat.st_size, filestat.st_nlink, name);


  }
