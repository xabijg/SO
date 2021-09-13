#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#define NULLH -1
#define MAX 100
#define AUTO 200
#define LEERCOMPLETO ((ssize_t)-1)

typedef struct {

    char file_name[MAX];
    long int *address;
    int size;
    time_t time;
    char type[MAX];
    int var;


} internalList;

typedef struct {

    internalList *allocated[MAX];
    int lastPos;

}memoryList;

void insertMemoryList ( memoryList *feed, internalList *example);
void createMemoryList( memoryList *feed );
void showAllocateMallocList ( memoryList *feed );
void showMemoryList( memoryList *feed, char type[] );
void * MmapFichero (char * fichero, int protection, memoryList *feed);
void Cmd_AllocateMmap (char *file, char *permissions, memoryList *feed);
void Cmd_AlocateCreateShared (char *key, char *size, memoryList *feed);
void * ObtenerMemoriaShmget (key_t clave, size_t tam, memoryList *feed);
void deallocMalloc (memoryList *feed, int var, char type[]);
void show_vars_addresses ();
void show_function_addresses ();
void empty_show ();
void doRecursiva (int n);
void Cmd_dopmap ();
void Cmd_deletekey (char llave[]);
void Cmd_AllocateShared (char * key, memoryList *feed);
void Cmd_AllocateMalloc ( char *size, memoryList *feed );
ssize_t LeerFichero (char *fich, void *p, ssize_t n);
void Cmd_Memfill(void *p,ssize_t *c,unsigned char b);
int TrocearCadena ( char * cadena, char * trozos [] );
