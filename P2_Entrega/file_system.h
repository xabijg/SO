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
