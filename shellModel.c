/****************PRACTICA_1******************/
/*******************SHELL********************/
/********************************************
* AUTOR_1: Sebastian Alfredo Castro Rampersad
* LOGIN_1: s.castro.rampersad
* DNI_1:  54938797-P
*********************************************
* AUTOR_2: Joel Sanchez Ruas
* LOGIN_2: joel.sanchez.ruas
* DNI_2:  34289568-H
*********************************************/

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ftw.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>

//LINKED LIST WITH HEAD NODE
typedef struct tNode {
  char strL[409600];
  struct tNode *sig;
} Node;

void CreateList(Node * head);
void InsertElement(Node * head, char * str);
int RemoveElement(Node * head);
int IsEmptyList (Node * head);
void PrintList(Node * head);
//LINKED LIST WITH HEAD NODE

// COMANDOS
int CommAutores(int numP, char * pieces[]);
int CommFecha(int numP, char * pieces[]);
int CommHora(int numP, char * pieces[]);
int CommCdir(int numP, char * pieces[]);
int CommPid(int numP, char * pieces[]);
int CommHis(int numP, char * pieces[]);
int CommCrear(int numP, char * pieces[]);
int CommBorrar(int numP, char * pieces[]);
int CommInfo(int numP, char * pieces[]);
int CommListar(int numP, char * pieces[], int opts);
// COMANDOS

void ListarRecursivo(char * ruta, int lng, int opV);
int VerificarOpListar(int numP, char * pieces[]);
void ListarAux(int numP, char * ruta, int opts);
char * ConvierteModo(mode_t m, char * permisos);
void AuxListaNoReclng(char * ruta, int opV);
void AuxListaNoRec(char * ruta, int opV);
void BorradoDirectAbsoluto(char *ruta);
char TipoFichero (mode_t m);
int CountSlash (char * str);

void PrintPrompt(void);
void ReadEnter(char * str);
int  SplitString(char * str, char * pieces[]);
int CommandPros(char * str, Node * head);
void PrintError(char str[]);

/*****************************************************************************/
int main(){
  int fin = 0;
  char  * strCommand;
  strCommand = (char *) malloc (409600*sizeof(char));

  Node * cabecera = (Node *) malloc (sizeof(Node));
  CreateList(cabecera);

  while (!fin) {
    PrintPrompt();
    ReadEnter(strCommand);
    fin = CommandPros(strCommand, cabecera);
    if (fin == 1) { free(cabecera); free(strCommand); }
  }
  return 0;
}

/*****************************************************************************/

/******************************FUNCIONES LISTA********************************/
void CreateList(Node * head) {
  head->sig = NULL;
  strcpy(head->strL, "");
}

void InsertElement(Node * head, char * str) {
  Node * toInsert = (Node *) malloc (sizeof(Node));

  if (head->sig == NULL) {
    toInsert->sig = NULL;
    strcpy(toInsert->strL, str);
    head->sig = toInsert;

  } else {
      Node * aux;
      aux = head->sig;
      while (aux->sig != NULL) {
        aux = aux->sig;
      }
      toInsert->sig = NULL;
      strcpy(toInsert->strL, str);
      aux->sig = toInsert;
    }
}

int RemoveElement(Node * head) {
  Node *toDel;
  //ELIMINA EL PRIMER NODO DE LA LISTA.

  if (head->sig == NULL) return 1;
    else {
      toDel = head->sig;
      if (toDel->sig != NULL) head->sig = toDel->sig;
        else head->sig = NULL;
      free(toDel);
      return 0;
    }
}

int IsEmptyList (Node * head){
  if (head->sig == NULL) return 0;
    else return 1;
}

void PrintList(Node * head) {
  Node *toPrint = head;
  while (toPrint->sig != NULL) {
    printf("%s\n", toPrint->sig->strL);
    toPrint = toPrint->sig;
  }
}
/******************************FUNCIONES LISTA********************************/

/*****************************************************************************/
void PrintPrompt(void) {
  printf("\n");
  printf("Sebas-&-Joel: ");
}

/*****************************************************************************/
void ReadEnter(char * str) {
  fgets(str, 409600, stdin);
}

/*****************************************************************************/
void PrintError(char str[]) {
  printf("-shell: %s: comando no valido \n", str);
}

/*****************************************************************************/
int SplitString(char * str, char * pieces[]) {
  int i=1;

  if ((pieces[0]=strtok(str," \n\t"))==NULL){ return 0; }
  while ((pieces[i]=strtok(NULL," \n\t"))!=NULL) { i++; }

  return i;
}

/*****************************************************************************/
int CommAutores(int numP, char * pieces[]) {
  int verif;

  if ((numP == 2) && (strncmp(pieces[1],"-n\0",3) == 0)) {
    printf("Sebastian Alfredo Castro Rampersad\n");
    printf("Joel Sanchez Ruas\n");
    verif = 0;

  } else if ((numP == 2) && (strncmp(pieces[1],"-l\0",3) == 0)){
      printf("s.castro.rampersad\n");
      printf("joel.sanchez.ruas\n");
      verif = 0;

    } else if (numP == 1) {
        printf("Sebastian Alfredo Castro Rampersad : s.castro.rampersad\n");
        printf("Joel Sanchez Ruas : joel.sanchez.ruas\n");
        verif = 0;

      } else verif = 1;
      return verif;
}

/*****************************************************************************/
int CommFecha(int numP, char * pieces[]) {
  int verif;

  if (numP == 1) {
    time_t tiempo = time(NULL);
    struct tm t = *localtime(&tiempo);
    printf("%d / %d / %d\n",  t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
    verif = 0;
  } else verif = 1;

  return verif;
}

/*****************************************************************************/
int CommHora(int numP, char * pieces[]) {
  int verif;

  if (numP == 1) {
    time_t tiempo = time(NULL);
    struct tm t = *localtime(&tiempo);
    printf("%dh : %dm : %ds\n",  t.tm_hour, t.tm_min, t.tm_sec);
    verif = 0;
  } else verif = 1;
  return verif;
}

/*****************************************************************************/
int CommPid(int numP, char * pieces[]) {
  int verif;

    if ((numP == 2) && (strncmp(pieces[1],"-p\0",3) == 0)) {
      printf ( "Pid padre del shell: %d\n", getppid());
      verif = 0;

    } else if (numP == 1) {
        printf("Pid del shell: %d\n", getpid());
        verif = 0;
      } else verif = 1;

  return verif;
}

/*****************************************************************************/
int CommCdir(int numP, char * pieces[]) {
  int verif;
  int vChdir = 0;
  char  * d;
  d = (char *) malloc (4096*sizeof(char));
  verif = 1;

    if (numP == 1) {
      getcwd(d, 4096);
      printf("%s\n", d);
      verif = 0;
      } else if (numP == 2) {
        vChdir = chdir(pieces[1]);
          if (vChdir != 0) {
            perror("Imposible cambiar de directorio");
          }
          verif = 0;
        } else verif = 1;
    free(d);
    return verif;
}

/*****************************************************************************/
int CommHist(int numP, char * pieces[], Node * head) {
  int verif;
  if (numP == 1) {
    PrintList(head);
    verif = 0;
  } else if ((numP == 2) && (strncmp(pieces[1],"-c\0",3) == 0)) {
    int control = 0;
      while (control == 0) control = RemoveElement(head);
      printf("Historic borrado\n");
      verif = 0;

  } else verif =1;
  return verif;
}

/*****************************************************************************/
void AuxListaNoRec(char * ruta, int opV) {
  /* opV nos indica si NO listamos los elementos ocultos (1)
  o no (resto de enteros); */
  DIR *d;
  struct dirent *elemento;
  struct stat s;
  int numPasosAtras, i;

  if ((d = opendir(ruta)) == NULL) {
    perror("-shell: Imposible abrir  ");
    return;
  }

  if ((strncmp(ruta,".\0",2) != 0)) {
    numPasosAtras = CountSlash(ruta) + 1;
    chdir(ruta);
  } else numPasosAtras = 0;


  elemento = readdir(d);

  while (elemento != NULL) {
    stat(elemento->d_name, &s);
    if (opV == 1) {
      if (strncmp(elemento->d_name, ".",1) != 0)
        printf("%s   %lu\n", elemento->d_name, s.st_size);
    } else
        printf("%s   %lu\n", elemento->d_name, s.st_size);

    elemento = readdir(d);
  }
  closedir(d);
  for (i = 0; i < numPasosAtras; i++) chdir("..");
}

/*****************************************************************************/
int CommCrear(int numP, char * pieces[]) {

  if (numP == 1) {
    AuxListaNoRec(".",2);
    return 0;

  } else if ((numP == 3) && (strncmp(pieces[1],"-d\0",3) == 0)){
    int fd;
    if (strncmp(pieces[2],".",1) == 0) {
      if ((strncmp(pieces[2],"./",2) == 0)||(strncmp(pieces[2],"../",3) == 0)) {
        fd = mkdir(pieces[2], 0777);
        if (fd == -1) perror("-shell: Imposible crear ");
        return 0;
      }
      printf("-shell: Imposible crear: This name format '.[whatever]' is forbidden\n");
      return 0;
    }  else if (strncmp(pieces[2],"/",1) == 0){
          char  * rutaActual;
          int numSlash, i;

          rutaActual = (char *) malloc (4096*sizeof(char));
          getcwd(rutaActual, 4096);
          numSlash = CountSlash(rutaActual);

          for (i = 0; i < numSlash; i++) chdir("..");

          fd = mkdir(pieces[2], 0777);
          if (fd == -1) perror("-shell: Imposible crear ");
          chdir(rutaActual);
          free(rutaActual);
          return 0;

       } else {
          fd = mkdir(pieces[2], 0777);
          if (fd == -1) perror("-shell: Imposible crear ");
          return 0;
      }

  } else if (numP == 2) {
      FILE *fp;
      if (strncmp(pieces[1],".",1) == 0) {
        if ((strncmp(pieces[1],"./",2) == 0)||(strncmp(pieces[1],"../",3) == 0)) {
          fp = fopen(pieces[1], "r");
          if (fp != NULL) {
            fclose(fp);
            printf("-shell: Imposible crear %s \n", pieces[1]);
          } else fopen(pieces[1], "a+");
          return 0;
        }
        printf("-shell: Imposible crear: This name format '.[whatever]' is forbidden\n");
        return 0;

      } else if (strncmp(pieces[1],"/",1) == 0){
            char  * rutaActual;
            int numSlash, i;

            rutaActual = (char *) malloc (4096*sizeof(char));
            getcwd(rutaActual, 4096);
            numSlash = CountSlash(rutaActual);

            for (i = 0; i < numSlash; i++) chdir("..");

            fp = fopen(pieces[1], "r");
            if (fp != NULL) {
              fclose(fp);
              printf("-shell: Imposible crear %s \n", pieces[1]);
            } else fopen(pieces[1], "a+");

            chdir(rutaActual);
            free(rutaActual);
            return 0;

         } else {
            fp = fopen(pieces[1], "r");
            if (fp != NULL) {
              fclose(fp);
              printf("-shell: Imposible crear %s \n", pieces[1]);
            } else fopen(pieces[1], "a+");
            return 0;
         }
     } else return 1;
}

/*****************************************************************************/
char TipoFichero(mode_t m) {
  switch (m&S_IFMT) {
    case S_IFSOCK:  return 's';
    case S_IFLNK: return 'l';
    case S_IFREG: return '-';
    case S_IFBLK: return 'b';
    case S_IFDIR: return 'd';
    case S_IFCHR:   return 'c';
    case S_IFIFO:   return 'p';
    default: return '?';
  }
}

/*****************************************************************************/
char * ConvierteModo(mode_t m, char * permisos) {
  strcpy (permisos,"---------- ");

  permisos[0] = TipoFichero(m);

  if (m&S_IRUSR) permisos[1]='r';
  if (m&S_IWUSR) permisos[2]='w';
  if (m&S_IXUSR) permisos[3]='x';
  if (m&S_IRGRP) permisos[4]='r';
  if (m&S_IWGRP) permisos[5]='w';
  if (m&S_IXGRP) permisos[6]='x';
  if (m&S_IROTH) permisos[7]='r';
  if (m&S_IWOTH) permisos[8]='w';
  if (m&S_IXOTH) permisos[9]='x';
  if (m&S_ISUID) permisos[3]='s';
  if (m&S_ISGID) permisos[6]='s';
  if (m&S_ISVTX) permisos[9]='t';
  return (permisos);
}

/*****************************************************************************/
int BorradoAux(const char *fRuta, const struct stat *s, int flag, struct FTW *buf) {
    int verif = remove(fRuta);

    if (verif) perror("-shell: Imposible eliminar ");

    return verif;
}

/*****************************************************************************/
void BorradoDirectAbsoluto(char *ruta) {
    nftw(ruta, BorradoAux, 64, FTW_DEPTH | FTW_PHYS);
}

/*****************************************************************************/
int CommBorrar(int numP, char * pieces[]) {
  if ((numP == 1) || ((numP ==2) && (strncmp(pieces[1],"-r",2) == 0))) {
    AuxListaNoRec(".",2);
    return 0;

  } else if ((numP == 2) && (!(strncmp(pieces[1],"-r\0",3) == 0))) {
      if (remove(pieces[1]) == -1) perror("-shell: Imposible eliminar ");
      return 0;
  } else if ((numP == 3) && (strncmp(pieces[1],"-r\0",3) == 0)) {
      struct stat fileStatus;
      char ficheroT;
      stat(pieces[2], &fileStatus);
      ficheroT = TipoFichero(fileStatus.st_mode);
      if (ficheroT != 'd') {
        if (remove(pieces[2]) == -1) perror("-shell: Imposible eliminar ");
      } else {
          BorradoDirectAbsoluto(pieces[2]);
          return 0;
      }
  }
  return 0;
}

/*****************************************************************************/
void PrintInfo(char * ruta){
  struct stat statRuta;
  struct passwd *user;
  struct group *group;
  char perm[12];
  int fd;

  lstat(ruta, &statRuta);

  if ((fd = open(ruta,O_RDONLY)) == -1) {
    perror("-shell: Imposible abrir ");
  } else {

    user = getpwuid(statRuta.st_uid);
    group = getgrgid(statRuta.st_gid);

    time_t time = statRuta.st_mtime;
    struct tm tm = *localtime(&time);

    ConvierteModo(statRuta.st_mode, perm);

    printf("%lu  ",statRuta.st_ino);

    printf("%s ",perm);

    printf("%lu  ",statRuta.st_nlink);
    printf("%s  %s  %ld  ", user->pw_name, group->gr_name, statRuta.st_size);
    printf("%d ", tm.tm_mday);

    if (tm.tm_mon == 0) printf("Jan");
    else if (tm.tm_mon == 1) printf("Feb");
    else if (tm.tm_mon == 2) printf("Mar");
    else if (tm.tm_mon == 3) printf("Apr");
    else if (tm.tm_mon == 4) printf("May");
    else if (tm.tm_mon == 5) printf("Jun");
    else if (tm.tm_mon == 6) printf("Jul");
    else if (tm.tm_mon == 7) printf("Aug");
    else if (tm.tm_mon == 8) printf("Sep");
    else if (tm.tm_mon == 9) printf("Oct");
    else if (tm.tm_mon == 10) printf("Nov");
    else if (tm.tm_mon == 11) printf("Dec");

  int year;
  if (tm.tm_year >= 100) {
    year = (tm.tm_year % 100);
    if (tm.tm_year < 110) printf(" 0%d ", year);
      else printf(" %d ", year);
  } else printf(" %d ", tm.tm_year);

  printf("%d:%d ", tm.tm_hour, tm.tm_min);

  if ((perm[0] != 'd') && (strrchr(ruta,'/') != NULL)) {
    printf("%s", strrchr(ruta,'/')+1);
  } else printf("%s", ruta);

  if (perm[0] == 'l') {
     char buffer[4096];
     if (readlink(ruta, buffer, sizeof(buffer)) < 0) {
	      printf(" -> NO_ENCONTRADO");
     } else {
	      char bufferRP[4096];
	      realpath(buffer,bufferRP);
	      printf(" -> %s", bufferRP);
       }
  }
   printf("\n");
   close(fd);
  }
}

/*****************************************************************************/
int CountSlash (char * str) {
  int i;
  int largo;
  int conteo = 0;

  largo = strlen(str);

  if (largo == 1) return 0;
    for (i = 0; i < largo; i++) {
      if (str[i] == '/') conteo = conteo + 1;
    }
  return conteo;
}

/*****************************************************************************/
int CommInfo(int numP, char * pieces[]) {
  if (numP == 1) return 1;
    else {
      int k ;
      for (k = 1; k < numP; k++) {
        if ((strncmp(pieces[k],"/",1) == 0)) {
          char  * rutaActual;
          int numSlash, i;

          rutaActual = (char *) malloc (4096*sizeof(char));
          getcwd(rutaActual, 4096);
          numSlash = CountSlash(rutaActual);

          for (i = 0; i < numSlash; i++) chdir("..");

          PrintInfo(pieces[k]);
          chdir(rutaActual);
          free(rutaActual);
        } else PrintInfo(pieces[k]);
      }
    }
    return 0;
}

/*****************************************************************************/
void ListarRecursivo(char * ruta, int lng, int opV) {
  //lng nos indica si listamos en formato largo (1) o no (resto de enteros);
  /* opV nos indica si NO listamos los elementos ocultos (1)
  o no (resto de enteros); */

  struct stat statEntradaDir;
  struct dirent *entradaDir;
  char *rutaCompleta;
  size_t rutaLargo;
  DIR *dir;
  char tipoF_EnDir;

  if ((dir = opendir(ruta)) == NULL){
    perror("-shell: Imposible abrir ");
    return;
  }

  if (lng == 1) AuxListaNoReclng(ruta, opV);
    else AuxListaNoRec(ruta, opV);

  while ((entradaDir = readdir(dir)) != NULL) {

    if (!(strcmp(entradaDir->d_name, "..")) || !(strcmp(entradaDir->d_name, ".")))
    continue;

    // Obtengo rutaCompleta para entradaDir
    rutaLargo = strlen(ruta);
    rutaCompleta = calloc(rutaLargo + strlen(entradaDir->d_name)+2, sizeof(char));
    strcpy(rutaCompleta, ruta);
    strcat(rutaCompleta, "/");
    strcat(rutaCompleta, entradaDir->d_name);

    stat(rutaCompleta, &statEntradaDir);
    tipoF_EnDir = TipoFichero(statEntradaDir.st_mode);

    if (tipoF_EnDir == 'd') {
        printf("\n\n%s  %ld\n", rutaCompleta, statEntradaDir.st_size);
        printf("- - - - - - - - - - - - -\n");
        ListarRecursivo(rutaCompleta, lng, opV);
        free(rutaCompleta);
    }
  }
  closedir(dir);
}

/*****************************************************************************/
void AuxListaNoReclng(char * ruta, int opV) {
  /* opV nos indica si NO listamos los elementos ocultos (1)
  o no (resto de enteros); */
  DIR *d;
  struct dirent *elemento;
  struct stat s;
  int numPasosAtras, i;

  if ((d = opendir(ruta)) == NULL) {
    perror("-shell: Imposible abrir  ");
    return;
  }

  if ((strncmp(ruta,".\0",2) != 0)) {
    numPasosAtras = CountSlash(ruta) + 1;
    chdir(ruta);
  } else numPasosAtras = 0;

  elemento = readdir(d);

  while (elemento != NULL) {
    stat(elemento->d_name, &s);
    if (opV == 1) {
      if (strncmp(elemento->d_name, ".",1) != 0)
        PrintInfo(elemento->d_name);
    } else PrintInfo(elemento->d_name);

    elemento = readdir(d);
  }
  closedir(d);
  for (i = 0; i < numPasosAtras; i++) chdir("..");
}
/*****************************************************************************/
int VerificarOpListar(int numP, char * pieces[]) {
  int tieneL;
  int tieneV;
  int tieneR;

  /* l = 1, r = 2, v = 3, rl = 4, vl = 5, rv = 6, lvr = 7
  si no tiene opciones devuelve 0  */

  if (numP >= 5) {
    tieneL = ((strncmp(pieces[1],"-l\0",3)==0) || (strncmp(pieces[2],"-l\0",3)==0) ||
              (strncmp(pieces[3],"-l\0",3)==0));

    tieneV = ((strncmp(pieces[1],"-v\0",3)==0) || (strncmp(pieces[2],"-v\0",3)==0) ||
              (strncmp(pieces[3],"-v\0",3)==0));

    tieneR = ((strncmp(pieces[1],"-r\0",3)==0) || (strncmp(pieces[2],"-r\0",3)==0) ||
              (strncmp(pieces[3],"-r\0",3)==0));

    if (tieneL && tieneV && tieneR) return 7;

    if ((numP == 5) && ((strncmp(pieces[4],"-l\0",3)==0) ||
        (strncmp(pieces[4],"-r\0",3)==0) || (strncmp(pieces[4],"-v\0",3)==0))) return -1;
  }

  if (numP >= 4) {
    tieneL = ((strncmp(pieces[1],"-l\0",3)==0) || (strncmp(pieces[2],"-l\0",3)==0));

    tieneV = ((strncmp(pieces[1],"-v\0",3)==0) || (strncmp(pieces[2],"-v\0",3)==0));

    tieneR = ((strncmp(pieces[1],"-r\0",3)==0) || (strncmp(pieces[2],"-r\0",3)==0));

    if (tieneL && tieneR) return 4;
    if (tieneV && tieneL) return 5;
    if (tieneR && tieneV) return 6;

    if ((numP == 4) && ((strncmp(pieces[3],"-l\0",3)==0) ||
        (strncmp(pieces[3],"-r\0",3)==0) || (strncmp(pieces[3],"-v\0",3)==0))) return -1;
  }

  if (numP >= 3) {
    if ((strncmp(pieces[1],"-l\0",3) == 0)) return 1;
    if ((strncmp(pieces[1],"-r\0",3) == 0)) return 2;
    if ((strncmp(pieces[1],"-v\0",3) == 0)) return 3;

    if ((numP == 3) && ((strncmp(pieces[2],"-l\0",3)==0) ||
        (strncmp(pieces[2],"-r\0",3)==0) || (strncmp(pieces[2],"-v\0",3)==0))) return -1;
  }
  return 0;
}

/*****************************************************************************/
void ListarAux(int numP, char * ruta, int opts) {
  /* OPTS -> l = 1, r = 2, v = 3, rl = 4, vl = 5, rv = 6, lvr = 7
  si no tiene opciones 0  */

  if (opts == 0) { // sin opciones
    printf("\n%s  \n", ruta);
    printf("- - - - - - - - - - - - -\n");
    AuxListaNoRec(ruta, 2);
  }
  else if (opts == 1) { // -l
    printf("\n%s  \n", ruta);
    printf("- - - - - - - - - - - - -\n");
    AuxListaNoReclng(ruta, 2);
  }
  else if (opts == 2)  { // -r
    printf("\n%s  \n", ruta);
    printf("- - - - - - - - - - - - -\n");
    ListarRecursivo(ruta, 2, 2);
    printf("\n\n");
  }
  else if (opts == 3)  { // -v
    printf("\n%s  \n", ruta);
    printf("- - - - - - - - - - - - -\n");
    AuxListaNoRec(ruta, 1);
  }
  else if (opts == 4)  { // -r -l
    printf("\n%s  \n", ruta);
    printf("- - - - - - - - - - - - -\n");
    ListarRecursivo(ruta, 1, 2);
    printf("\n\n");
  }
  else if (opts == 5) { // -v -l
    printf("\n%s  \n", ruta);
    printf("- - - - - - - - - - - - -\n");
    AuxListaNoReclng(ruta, 1);
   }
  else if (opts == 6) { // -r -v
    printf("\n%s  \n", ruta);
    printf("- - - - - - - - - - - - -\n");
    ListarRecursivo(ruta, 2, 1);
    printf("\n\n");
  }
  else if (opts == 7)  { // -l -r -v
    printf("\n%s  \n", ruta);
    printf("- - - - - - - - - - - - -\n");
    ListarRecursivo(ruta, 1, 1);
    printf("\n\n");
  }
}

/*****************************************************************************/
int CommListar(int numP, char * pieces[], int opts) {
    struct stat sT;
    int k, inicio = 1;
    char tFi;

    if (numP == 1) AuxListaNoRec(".",2);

    if (opts == -1) return 1;

    if ((opts == 1) || (opts == 2) || (opts == 3)) inicio = 2;
    else if ((opts == 4) || (opts == 5) || (opts == 6)) inicio = 3;
    else if (opts == 7) inicio = 4;
    else if (opts == 0) inicio = 1;

    for (k = inicio; k < numP; k++) {
      if ((strncmp(pieces[k],"/",1) == 0)) {
        char  * rutaActual;
        int numSlash, i;
        rutaActual = (char *) malloc (4096*sizeof(char));

        getcwd(rutaActual, 4096);
        numSlash = CountSlash(rutaActual);

        for (i = 0; i < numSlash; i++) chdir("..");

        stat(pieces[k], &sT);
        tFi = TipoFichero(sT.st_mode);
        if (tFi != 'd') {
          printf("-shell: Imposible abrir %s : no es un directorio\n",pieces[k]);
        } else ListarAux(numP, pieces[k], opts);

        chdir(rutaActual);
        free(rutaActual);
      } else {
        stat(pieces[k], &sT);
        tFi = TipoFichero(sT.st_mode);
        if (tFi != 'd') {
          printf("-shell: Imposible abrir %s : no es un directorio\n",pieces[k]);
        } else ListarAux(numP, pieces[k], opts);
      }
    }
    return 0;
}

/*****************************************************************************/
int CommandPros(char * str, Node * head){
  char *pieces[100];
  int numW = 0;
  char strReserva[409600];
  int i;

  for (i = 0; i < 100; i++) {
    pieces[i] = (char *) malloc (4096*sizeof(char));
  }

  strcpy(strReserva, str);
  strtok(strReserva,"\n"); // POR FORMATO
  numW = SplitString(str, pieces);

  //DE ESTA MANERA NO HACE COMPROBACION AL SOLO METER ENTER.
  if (numW == 0) return 0;
   else InsertElement(head, strReserva);

  // COMANDO AUTORES (-l, -n).
  if ((strncmp(pieces[0],"autores\0",8) == 0)) {
    if (CommAutores(numW, pieces) == 1 ){
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO AUTORES (-l, -n).

  // COMANDO FECHA.
  if ((strncmp(pieces[0],"fecha\0",6) == 0)) {
    if (CommFecha(numW, pieces) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO FECHA.

  // COMANDO HORA.
  if (strncmp(pieces[0],"hora\0",5) == 0) {
    if (CommHora(numW, pieces) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO HORA.

  // COMANDO PID (-p).
  if (strncmp(pieces[0],"pid\0",4) == 0) {
    if (CommPid(numW, pieces) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO PID (-p).

  // COMANDO CDIR.
  if (strncmp(pieces[0],"cdir\0",5) == 0) {
    if (CommCdir(numW, pieces) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO CDIR.

  // COMANDO HIST (-c).
  if (strncmp(pieces[0],"hist\0",5) == 0) {
    if (CommHist(numW, pieces, head) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO HIST (-c).

  // COMANDO CREAR (-d).
  if (strncmp(pieces[0],"crear\0",6) == 0) {
    if (CommCrear(numW, pieces) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO CREAR (-d).

  // COMANDO BORRAR (-r).
  if (strncmp(pieces[0],"borrar\0",7) == 0) {
    if (CommBorrar(numW, pieces) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO BORRAR (-r).

  // COMANDO INFO.
  if (strncmp(pieces[0],"info\0",5) == 0) {
    if (CommInfo(numW, pieces) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO INFO.

  // COMANDO LISTAR (-l) (-v) (-r).
  if (strncmp(pieces[0],"listar\0",7) == 0) {
    if (CommListar(numW, pieces, VerificarOpListar(numW,pieces)) == 1) {
      PrintError(strReserva);
    }
      //for (i = 0; i < 100; i++) free(pieces[i]);
      return 0;
  }
  // COMANDO LISTAR (-l) (-v) (-r).

  // COMANDOS DE SALIDA.
  if ((numW == 1) && ((strncmp(pieces[0],"fin\0",4) == 0) ||
  (strncmp(pieces[0],"end\0",4) == 0) || (strncmp(pieces[0],"exit\0",5) == 0))){
    // ELIMINO TODOS LOS NODOS MENOS LA CABECERA.
    // QUE SE ELIMINA EN EL MAIN.
    while (IsEmptyList(head) != 0) RemoveElement(head);

    //for (i = 0; i < 100; i++) free(pieces[i]);
    return 1;
  }
  //COMANDOS DE SALIDA.

  PrintError(strReserva);

  //for (i = 0; i < 100; i++) free(pieces[i]);
  return 0;

}
