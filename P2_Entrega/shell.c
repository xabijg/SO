/*
*AUTOR 1 Martin Vieites Garcia martin.vieites
*Autor 2 Xabier Jimenez Gomez xabier.jimenez.gomez
*/

#include "file_system.h"
#include "list.h"

int main() {

    char cadena[50];
    char *trozos[100];
    bool terminado = false;

    typeHistoric *historicFeed;
    historicFeed=(typeHistoric*) malloc (409600*sizeof(typeHistoric));
    int *c;
    c=(int*) malloc (409600*sizeof(int));
    int tam = 0;

    memoryList *memoryFeed;
    memoryFeed = ( memoryList* ) malloc (409600*sizeof(memoryList));

    createHistoric(historicFeed);
    createMemoryList(memoryFeed);
    inicializarCount(c);

    while (!terminado) {

        printf("$ ");
        fgets(cadena, 50, stdin);

        tam = TrocearCadena(cadena, trozos);

        if (trozos[0] == NULL){

            terminado = false;

        }else{

            insertH(historicFeed,cadena);
            *c = *c+1;

            if (strcmp(trozos[0], "authors") == 0) {

                fAuthors(trozos);

            } else if (strcmp(trozos[0], "getpid") == 0) {

                fGetPid();

            } else if (strcmp(trozos[0], "getppid") == 0) {

                fGetPpid();

            } else if (strcmp(trozos[0], "pwd") == 0) {

                fPwd();

            } else if (strcmp(trozos[0], "chdir") == 0) {

                fChdir(trozos);

            } else if (strcmp(trozos[0], "date")==0){

                fDate();

            } else if (strcmp(trozos[0], "time")==0){

                fTime();

            } else if (strcmp(trozos[0], "historic") == 0) {

                fHistoric(tam, trozos, historicFeed, c);

            } else if (strcmp(trozos[0], "create") == 0) {

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

                } else {

                    int check;

                    check = open(trozos[1], O_EXCL | O_WRONLY | O_APPEND | O_CREAT, 0775);

                    if (check == -1) {

                        printf("Unable to create file\n");
                        perror("Following error ocurred");

                    }
                }

            } else if (strcmp(trozos[0], "delete") == 0) {

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


            } else if (strcmp(trozos[0], "list") == 0){

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

            } else if (strcmp(trozos[0], "memory") == 0) {

                if ( trozos[1] == NULL ) {

                    showMemoryList(memoryFeed, "all");

                } else if (strcmp( trozos[1], "-allocate" ) == 0) {

                    if ( trozos[2] == NULL ) {

                        showMemoryList(memoryFeed,"all");

                    } else if ( strcmp( trozos[2], "-malloc" ) == 0 ) {

                        Cmd_AllocateMalloc( trozos[3], memoryFeed );

                    } else if ( strcmp ( trozos[2], "-mmap" ) == 0 ) {

                        Cmd_AllocateMmap(trozos[3], trozos[4], memoryFeed);

                    } else if ( strcmp (trozos[2], "-createshared") == 0) {

                        Cmd_AlocateCreateShared(trozos[3], trozos[4], memoryFeed);

                    } else if ( strcmp (trozos[2], "-shared") == 0 ) {

                        Cmd_AllocateShared( trozos[3], memoryFeed );

                    }

                } else if (strcmp(trozos[1], "-show") == 0) {

                    if ( trozos[2] == NULL ) { empty_show(); }
                    else if ( strcmp(trozos[2], "-malloc") == 0 ) { showMemoryList (memoryFeed, "malloc"); }
                    else if ( strcmp(trozos[2], "-mmap") == 0 ) { showMemoryList (memoryFeed, "mmap"); }
                    else if ( strcmp(trozos[2], "-shared") == 0 ) { showMemoryList (memoryFeed, "shared"); }
                    else if ( strcmp(trozos[2], "-all") == 0 ) { showMemoryList( memoryFeed, "all" ); }

                } else if (strcmp(trozos[1], "-dealloc") == 0) {

                    if ( trozos[2] == NULL ) { showMemoryList(memoryFeed, "all"); }
                    else if ( strcmp(trozos[2], "-malloc") == 0 ) { deallocMalloc ( memoryFeed, atoi(trozos[3]), "malloc" ); }

                } else if ( strcmp(trozos[1], "-show-vars") == 0 ) {

                    show_vars_addresses();

                } else if ( strcmp(trozos[1], "-show-funcs") == 0 ) {

                    show_function_addresses();

                } else if ( strcmp( trozos[1], "-dopmap" ) == 0 ) {

                    Cmd_dopmap();

                } else if ( strcmp( trozos[1], "-deletekey" ) == 0 ) {

                    Cmd_deletekey( trozos[2] );

		}

            } else if (strcmp ( trozos[0], "recurse" ) == 0){

                if ( trozos[1] != NULL ) {

                    doRecursiva( atoi(trozos[1]) );

                }

            } else if (strcmp ( trozos[0], "readfile" ) == 0) {

                LeerFichero ( trozos[1], trozos[2], atoi(trozos[3]) );

            }else if (strcmp ( trozos[0], "memfill" ) == 0 ) {

                Cmd_Memfill(trozos[1],atoi(trozos[2]),atoi(trozos[3]));

            } else {

                printf("Comando desconocido \n");

            }

        }
    }

    free(historicFeed);
    free(memoryFeed);
    free(c);

    return 0;

}
