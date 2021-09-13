#include "list.h"

// variables globales declaradas exclusivamente para -show-vars
int global1 = 10, global2 = 420, global3 = 69;

void createMemoryList( memoryList *feed ) {

    feed -> lastPos = NULLH;

}

void insertMemoryList ( memoryList *feed, internalList *example ) {

    internalList *tmp = example;

    if (feed->lastPos == MAX-1) {

        return;

    } else {

    feed -> lastPos++;
    feed -> allocated[feed->lastPos] = tmp;

    }

}

void showMemoryList( memoryList *feed, char type[] ) {

    if ( strcmp(type, "all") == 0 ){

        showMemoryList(feed, "malloc");
        showMemoryList(feed, "mmap");
        showMemoryList(feed, "shared");

    } else if ( strcmp(type, "malloc") == 0 ) {

        for ( int i = 0; i <= feed->lastPos; i++ ){

            if ( feed -> lastPos < 0 ) {

                return;

            }

            if ( strcmp(feed->allocated[i]->type, "malloc") == 0) {

                time_t raw_time = feed->allocated[i]->time;
                struct tm *timeInfo;

                timeInfo = localtime( &raw_time );

                printf("%p : size:%10d %8s %16s", feed->allocated[i]->address, feed->allocated[i]->size, feed->allocated[i]->type,asctime(timeInfo));

            }

        }

    } else if ( strcmp(type, "mmap") == 0 ) {

        for ( int i = 0; i <= feed->lastPos; i++ ){

            if ( strcmp(feed->allocated[i]->type, "mmap") == 0) {

                time_t raw_time = feed->allocated[i]->time;
                struct tm *timeInfo;

                timeInfo = localtime( &raw_time );

                printf("%p : size:%10d %8s %8s (fd:%d) %16s", feed->allocated[i]->address, feed->allocated[i]->size, feed->allocated[i]->type, feed->allocated[i]->file_name, feed->allocated[i]->var, asctime(timeInfo));

            }

        }

    } else if ( strcmp(type, "shared") == 0 ) {

        for ( int i = 0; i <= feed->lastPos; i++ ) {

            if ( strcmp(feed->allocated[i]->type, "shared memory") == 0) {


                time_t raw_time = feed->allocated[i]->time;
                struct tm *timeInfo;

                timeInfo = localtime( &raw_time );

                printf("%p : size:%10d %8s (key %d) %16s", feed->allocated[i]->address, feed->allocated[i]->size, feed->allocated[i]->type, feed->allocated[i]->var, asctime(timeInfo));

            }

        }

    }

}

void deallocMalloc ( memoryList *feed, int var, char type[] ) {

    if ( strcmp( type, "malloc" ) == 0 ) {

        for ( int i = 0; i <= feed->lastPos; i++ ) {

            if ( (feed -> allocated[i] -> size) == var ) {

                printf("deallocated %d at %p \n",feed -> allocated[i] -> size, feed -> allocated[i] -> address );

                free ( feed -> allocated[i] );
                feed -> lastPos--;
                return;

            }

        }

        printf("There is no block of that size assigned with malloc\n");

    }
}

void show_vars_addresses () {

    int local1 = 10, local2 = 20, local3 = 30;

    printf("Variables locales: \t %p, \t %p, \t %p \n", &local1, &local2, &local3 );
    printf("Variables globales: \t %p, \t %p, \t %p \n", &global1, &global2, &global3 );

}

void show_function_addresses () {

    printf("Funciones programa: \t %p, \t %p, \t %p \n", &TrocearCadena, &show_vars_addresses, &show_function_addresses );
    printf("Funciones libreria: \t %p, \t %p, \t %p \n", &printf , &fgets, &remove );

}

void empty_show () {

    printf("Funciones programa: \t %p, \t %p, \t %p \n", &TrocearCadena, &show_vars_addresses, &show_function_addresses );
    show_vars_addresses();

}

void doRecursiva (int n) {

    char automatico[AUTO];
    static char estatico[AUTO];

    printf ("parametro n:%d en %p \t",n,&n);
    printf ("array estatico en:%p \t",estatico);
    printf ("array automatico en %p \n",automatico);
    n--;
    if (n>=0)
        doRecursiva(n);

}

// Functions from pdf

void Cmd_AllocateMmap (char *file, char *permissions, memoryList *feed) { /*file is the file name and permissions are the permissions*/

    char *perm;
    void *p;
    int protection=0;
    if (file==NULL)
        { showMemoryList( feed, "mmap" ); return;}
    if ((perm=permissions)!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }

    if ((p=MmapFichero(file,protection, feed))==NULL)
        perror ("Imposible mapear fichero");
    else
        printf ("fichero %s mapeado en %p\n", file, p);
}

void * MmapFichero (char * fichero, int protection, memoryList *feed) {

    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;
    if (protection&PROT_WRITE) modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL;

    internalList *internal;
    internal = ( internalList* ) malloc (409600*sizeof(internalList));

    internal -> address = p;
    strcpy (internal -> file_name, fichero);
    internal -> size = s.st_size;
    strcpy ( internal->type, "mmap" );
    internal -> var = df;
    internal -> time = time(NULL);

    insertMemoryList(feed, internal);

    return p;
}

void Cmd_AlocateCreateShared (char *key, char *size, memoryList *feed) { /*key is the key and size is the size*/

    key_t k;
    size_t tam=0;
    void *p;

    if (key==NULL || size==NULL) { showMemoryList(feed, "createshared"); return;}

    k=(key_t) atoi(key);

    if ( size != NULL ) {
        tam=(size_t) atoll(size);
    }

    if ((p=ObtenerMemoriaShmget(k,tam, feed))==NULL)
        perror ("Imposible obtener memoria shmget");
    else
        printf ("Memoria de shmget de clave %d asignada en %p\n",k,p);
}

void * ObtenerMemoriaShmget (key_t clave, size_t tam, memoryList *feed) {

    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (tam) /*si tam no es 0 la crea en modo exclusivo */
    flags=flags | IPC_CREAT | IPC_EXCL;

    /*si tam es 0 intenta acceder a una ya creada*/
    if (clave==IPC_PRIVATE) { /*no nos vale*/
        errno=EINVAL; return NULL;}

    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);

    if ((p=shmat(id,NULL,0))==(void*) -1){

        aux=errno; /*si se ha creado y no se puede mapear*/
        if (tam) /*se borra */
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }

    shmctl (id,IPC_STAT,&s);

    internalList *internal;
    internal = ( internalList* ) malloc (409600*sizeof(internalList));

    internal -> address = p;
    internal -> size = s.shm_segsz ;
    strcpy ( internal->type, "shared memory");
    internal -> var = clave;
    internal -> time = time(NULL);

    insertMemoryList(feed, internal);

    return (p);
}

void Cmd_dopmap () /*no arguments necessary*/ {

    pid_t pid;
    char elpid[32];
    char *argv[3]={"pmap",elpid,NULL};
    sprintf (elpid,"%d", (int) getpid());
    if ((pid=fork())==-1){
        perror ("Imposible crear proceso");
        return;
    }
    if (pid==0){
        if (execvp(argv[0],argv)==-1)
        perror("cannot execute pmap");
        exit(1);
    }
    waitpid (pid,NULL,0);
}

void Cmd_deletekey (char llave[]) /*arg[0] points to a str containing the key*/ {

    key_t clave;
    int id;
    char *key=llave;
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf (" rmkey clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
    perror ("shmctl: imposible eliminar memoria compartida\n");

}

void Cmd_AllocateShared (char * key, memoryList *feed) {

    key_t k;
    size_t tam=0;
    void *p;

    if (key==NULL){

        showMemoryList(feed, "shared");
        return;
    }

    k=(key_t) atoi(key);

    if ((p=ObtenerMemoriaShmget(k,tam,feed))==NULL)
        perror ("Cannot allocate");
    else
        printf ("Allocated shared memory (key %d) at %p\n",k,p);

}

void Cmd_AllocateMalloc ( char *size, memoryList *feed ) {

    internalList *internal;
    internal = ( internalList* ) malloc (409600*sizeof(internalList));

    if ( size != NULL ) {

        long int *i = malloc (atoi(size));

        internal->address = i;
        internal->size = atoi( size );
        internal->time = time(NULL);
        strcpy( internal->type, "malloc");

        insertMemoryList(feed, internal);

        printf("allocated %d at %p \n", atoi(size), i);

    } else {

        showMemoryList(feed,"malloc");

    }

}

ssize_t LeerFichero (char *fich, void *p, ssize_t n) { /*n=-1 indica que se lea todo*/

    ssize_t nleidos,tam=n;
    int df, aux;
    struct stat s;

    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
        return ((ssize_t)-1);

    if (n==LEERCOMPLETO)
        tam=(ssize_t) s.st_size;

    if ((nleidos=read(df,p, tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return ((ssize_t)-1);
    }

    close (df);
    return (nleidos);

}

void Cmd_Memfill(void *p,ssize_t  c,unsigned char b){
    unsigned char byte = 65;
    int cont=128;
    unsigned char *word;

    if(p==NULL) 
        return;

    if(c!=NULL)
        cont = atoi(c);

    if(b!=NULL)
        byte = strtoul(b,NULL,0); 

    for(int i=0; i<cont; i++){
        word[i] = byte;
    }
}

// void Cmd_Memdump(void* address, size_t tam){
//     int cont=25;
//     size_t i;

//     if(tam!=NULL)
//     cont = atoi(tam);

//     for (i=0;i<cont;i++){
//         printf("Data in [%p..%p): ",data,data+len);
//         printf("%02X ", ((unsigned char*)data)[i] );
//         printf("\n");
//     }


// }