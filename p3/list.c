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

            if (strcmp(feed->allocated[i]->type, "malloc") == 0) {

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

            if ( strcmp(feed->allocated[i]->type, "shared") == 0) {


                time_t raw_time = feed->allocated[i]->time;
                struct tm *timeInfo;

                timeInfo = localtime( &raw_time );

                printf("%p : size:%10d %8s (key %d) %16s", feed->allocated[i]->address, feed->allocated[i]->size, feed->allocated[i]->type, feed->allocated[i]->var, asctime(timeInfo));

            }

        }

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
        perror ("Imposible mapping file");
    else
        printf ("file %s mapped at %p\n", file, p);
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
    internal = ( internalList* ) malloc (4096*sizeof(internalList));

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
        perror ("imposible to get memory from shmget");
    else
        printf ("shmget memory key %d assignet at %p\n",k,p);
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
    internal = ( internalList* ) malloc (4096*sizeof(internalList));

    internal -> address = p;
    internal -> size = s.shm_segsz ;
    strcpy ( internal->type, "shared");
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
        perror ("unable to create proccess");
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
        printf (" rmkey valid_key\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible to get shared memoryt");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
    perror ("shmctl: imposible delete shared memory\n");

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
    internal = ( internalList* ) malloc (4096*sizeof(internalList));

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

void Cmd_DeallocMalloc ( char *tam, memoryList *feed) {

    if ( tam == NULL ) {

        return;

    }

    int size = atoi ( tam );

    for ( int i = 0; i <= feed->lastPos; i++ ) {

        if ( (feed -> allocated[i] -> size) == size ) {

            printf("block at address %p deallocated (%s) \n",feed -> allocated[i] -> address, feed -> allocated[i] -> type );

            free (feed -> allocated[i] -> address);
            deleteElement( i, feed );
            return;

        }

    }

    printf("There is no block of that size assigned with malloc\n");

}

void Cmd_DeallocMmap ( char *file_name, memoryList *feed ) {

    if ( file_name == NULL ) {

        return;

    }

    for ( int i = 0; i <= feed->lastPos; i++ ) {

        if ( strcmp ( feed -> allocated[i] -> file_name, file_name ) == 0 ) {

            printf("block at addres %p deallocated (%s) \n",feed -> allocated[i] -> address, feed -> allocated[i] -> type );

            munmap( feed -> allocated[i] -> address, feed -> allocated[i] -> size );
            deleteElement( i, feed );
            return;

        }

    }

    printf("There is no file %s allocated with Mmap\n", file_name);

}

void Cmd_DeallocShared ( char *key, memoryList *feed ) {

    if ( key == NULL ) {

        return;

    }

    key_t num_key = atoi (key);

    for ( int i = 0; i <= feed->lastPos; i++ ) {

        if ( ( num_key == feed -> allocated[i] -> var ) && ( strcmp ( feed -> allocated[i] -> type, "shared" ) == 0 ) ) {

            printf("block at address %p deallocated (%s) \n",feed -> allocated[i] -> address, feed -> allocated[i] -> type );


            shmdt( feed -> allocated[i] -> address );
            deleteElement( i, feed );
            return;

        }

    }

    printf("There is no block with key %d assigned with shared\n", num_key);

}

void Cmd_DeallocAddress ( char *address, memoryList *feed ) {

    long int *tmp = (long int*) strtoul(address, NULL, 16);
    void *p = (void*) strtoul(address,NULL,16);

    for ( int i = 0; i <= feed->lastPos; i++ ) {

        if ( feed -> allocated[i] -> address == tmp ) {

            printf("block at address %p deallocated (%s) \n",feed -> allocated[i] -> address, feed -> allocated[i] -> type );

            free(p);
            deleteElement( i, feed );
            return;

        }

    }

    printf("address %s not allocated with malloc, shared or mmap\n", address);

}

void deleteElement ( int position, memoryList *feed ) {

    if ( position == feed->lastPos ) {

        free (feed->allocated[position]);
        feed -> lastPos --;

    } else {

        free ( feed -> allocated [position] );

        for (int i = position; i < feed -> lastPos; i++) {

            internalList *aux = feed -> allocated[i+1];
            feed -> allocated[i] = aux;

        }

        feed -> lastPos --;

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

void dump (char *address , char *counter){

    int cont_bytes;
    unsigned char *p;

    if ( address == NULL ) {

        return;

    } else {

        sscanf(address,"%p",&p);

    }

    if ( counter == NULL ){

        cont_bytes = 25;

    } else {

        cont_bytes = atoi ( counter );

    }

    int i, j;
    for(i=0; i<cont_bytes; i=i+25){

        for(j=i; (j<(i+25))&&(j<cont_bytes); j++){//Imprime los caracteres (si se puede)
            if(isprint(p[j])){
                printf(" %c ", p[j]);
            } else {
                printf("   ");
            }
        }
        printf("\n");
        for(j=i; (j<(i+25))&&(j<cont_bytes); j++){//Imprime el valor hex

            printf("%02x ",p[j]);
        }
        printf("\n");
    }
}

void fill (char * address, char * cont, char * byte){

    int cont_bytes;
    unsigned char byte_setter;
    unsigned char *p;

    if(address==NULL) return;

    if ( cont == NULL ) {

        cont_bytes = 128;

    } else {

        cont_bytes = atoi (cont);

    }

    if ( byte == NULL ) {

        byte_setter = 65;

    } else {

        byte_setter = strtoul(byte,NULL,0); //Al poner 0 detecta si empieza o no por 0x

    }

    sscanf(address,"%p",&p);

    for(int i=0; i<cont_bytes; i++){
        p[i] = byte_setter;
    }
}
