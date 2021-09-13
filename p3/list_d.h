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
#include <ctype.h>

#define SHELL_EXIT_SUCCESS 2
#define SHELL_EXIT_FAILURE 3




typedef struct nodo * list;
typedef struct nodo * pos;



list createList();

int isEmptyList(list l);

void insertElement(void *element,pos p);

int removeE(list l, pos p);

int removeAll(list l);

pos first(list l);

pos nextE(pos p);

int last(pos p);

void *getElement(pos p);

pos lookFor(list l,void *element,int (*comp)(const void *x,const void *y));


