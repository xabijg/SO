#include "file_system.h"
#include "list.h"
#include <sys/resource.h>

// zona de pruebas
typedef struct {

    pid_t pid;
    time_t time;
    char arg[100][100];
} proceso;
// typedef struct elemento {
//
//     proceso proceso;
//     struct elemento *next;
//
// } elemento;
//
// typedef elemento *lista;
//
// lista createProcessList();
proceso recordProcess(char *tr[], pid_t pid);
// void addProcess(proceso process, lista lista);
// void printProcessList(lista lista);

struct Node {
    proceso proceso;
    struct Node *next;
};

void insertStart (struct Node** head, proceso proceso_);
void displayList (struct Node* node);

void getPriority_Cmd (char *tr[]);
void setpriority_Cmd (char *tr[]);
void getuid_Cmd ();
void setuid_Cmd (char *tr[]);
void fork_Cmd ();
int execute_Cmd(char *tr[], int tam, bool show);
int foreground_Cmd (char *tr[], int tam, bool show);
int background_Cmd (char *tr[], int tam, bool show, struct Node** head);
void run_as_Cmd (char *tr[], int tam, struct Node** head);
void execute_as_Cmd (char *tr[], int tam);
int setuid_aux (char *user, bool login);
void unrecognized_Cmd(char *tr[], int tam, struct Node** head);
int tam_array (char *tr[]);

int main(int argc, char const *argv[]) {

    struct Node* head = NULL;

    char cadena[50];
    char *trozos[100];
    bool terminado = false;

    while (!terminado) {

        printf("$ ");
        fgets(cadena, 50, stdin);
        TrocearCadena( cadena, trozos );

        if ( trozos[0] == NULL ) {

            terminado = false;

        } else  {

            if ( strcmp(trozos[0], "getpriority") == 0 ) {

                getPriority_Cmd(trozos);

            } else if ( strcmp(trozos[0], "setpriority") == 0 ) {

                setpriority_Cmd(trozos);

            } else if ( strcmp(trozos[0], "getuid") == 0 ) {

                getuid_Cmd();

            } else if ( strcmp(trozos[0], "setuid") == 0 ) {

                setuid_Cmd(trozos);

            } else if (strcmp(trozos[0], "fork") == 0 ) {

                fork_Cmd();

            } else if (strcmp(trozos[0], "execute") == 0 ) {

                execute_Cmd(trozos, tam_array(trozos), true);

            } else if (strcmp(trozos[0], "foreground") == 0) {

                foreground_Cmd(trozos, tam_array(trozos), true, &head);

            } else if (strcmp(trozos[0], "background") == 0) {

                background_Cmd(trozos, tam_array(trozos), true, &head);

            } else if (strcmp(trozos[0], "run-as") == 0) {

                run_as_Cmd(trozos, tam_array(trozos), &head);

            } else if (strcmp(trozos[0], "listprocs") == 0) {

                displayList(head);

            } else if (strcmp(trozos[0], "execute-as") == 0) {

                execute_as_Cmd(trozos, tam_array(trozos), &head);

            } else if (strcmp(trozos[0], "record") == 0) {
                proceso tmp = recordProcess(trozos, getpid());
                insertStart(&head, tmp);

            } else if ( strcmp(trozos[0], "quit") == 0 || strcmp(trozos[0], "end") == 0 || strcmp(trozos[0], "exit") == 0 ) {

                terminado = true;
                exit(0);

            } else {

                unrecognized_Cmd(trozos, tam_array(trozos), &head);

            }

        }

    }

    return 0;
}

char * NombreUsuario (uid_t uid) {
    struct passwd *p;
    if ((p=getpwuid(uid))==NULL)
        return (" ??????");
    return p->pw_name;
}
uid_t UidUsuario (char * nombre) {
    struct passwd *p;
    if ((p=getpwnam (nombre))==NULL)
        return (uid_t) -1;
    return p->pw_uid;
}
void getuid_Cmd () {
    uid_t real=getuid(), efec=geteuid();
    printf ("Credencial real: %d, (%s)\n", real, NombreUsuario (real));
    printf ("Credencial efectiva: %d, (%s)\n", efec, NombreUsuario (efec));
}
void setuid_Cmd (char *tr[]) {

    if (tr[1]== NULL) {
        getuid_Cmd();
        return;
    } else if ( strcmp(tr[1], "-l") == 0 ) {
        if (tr[2]==NULL) {
            getuid_Cmd();
            return;
        } else
        setuid_aux(tr[2], true);
    } else {
        setuid_aux(tr[1], false);
    }
}
int setuid_aux (char *user, bool login) {

    uid_t uid;
    int u;
    if (login){
        if ((uid=UidUsuario(user))==(uid_t) -1){
            printf ("Usuario no existente %s\n", user);
            return -1;
        }
    }
    else if ((uid=(uid_t) ((u=atoi (user))<0)? -1: u) ==(uid_t) -1){
        printf ("Valor no valido de la credencial %s\n",user);
        return -1;
    }
    if (setuid (uid)==-1) {
        printf ("Imposible cambiar credencial: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
void getPriority_Cmd (char *tr[]) {

    id_t pid;
    int which;
    int ret;

    if (tr[1] == NULL) {
        pid = getpid();
    } else {
        pid = (id_t) atoi(tr[1]);
    }

    which = PRIO_PROCESS;
    ret = getpriority(which, pid);

    if ( ret == -1 ) {
        perror("Following error ocurred ");
    } else {
        printf("Priority of process %d is %d\n", pid, ret);
    }
}
void setpriority_Cmd (char *tr[]) {

    id_t pid;
    int priority;
    int which = PRIO_PROCESS;

    if ( tr[1] == NULL ) {
        getPriority_Cmd(NULL);
        return;
    } else if ( tr[2] == NULL ) {
        pid = getpid();
        priority = atoi (tr[1]);
    } else {
        pid = (id_t) atoi(tr[1]);
        priority = atoi (tr[2]);
    }

    if ( priority < -20 || priority > 19 ) {
        printf("Priority value must be between -20 and 19\n");
        return;
    }

    if ( setpriority(which, pid, priority) == -1 ) {
        perror("Following error ocurred ");
    } else {
        printf("Priority of process %d changed to %d\n", pid, priority);
    }
}
void fork_Cmd () {

    pid_t pidhijo;
    if ((pidhijo = fork())==(-1)){
        perror("Error: fork failed");
        return;
    }
    if (pidhijo!=0){
        waitpid(pidhijo,NULL,0); //padre
        return;
    } else {
        printf("Executing proccess %d\n", getpid());
        return; //hijo
    }
}
int execute_Cmd (char *tr[], int tam, bool show, bool record, struct Node** head) {
    int which = PRIO_PROCESS;
    if (record) {
        insertStart(head, recordProcess(tr, getpid()));
    }

    if ( tam == 1 ) {
        if (show) { printf("Program not specified\n"); }
        return -1;
    }
    if ( tr[tam-1][0] == '@' ) {
        tr[tam-1]++;
        setpriority(which, getpid(), atoi(tr[tam-1]));
        tr[tam-1] = NULL;
        return execvp(tr[1],(char * const *) (&tr[1]));
        if (show) { perror("Following error ocurred "); }
        return -1;
    }
    execvp(tr[1],(char * const *) (&tr[1]));
    if (show) { perror("Following error ocurred "); }
    return -1;
}
int foreground_Cmd (char *tr[], int tam, bool show) {

    pid_t pid;
    if ((pid=fork())==0) {
        return execute_Cmd(tr, tam, show);
        exit (255);
    }
    waitpid(pid,NULL,0);
    return 0;
}
int background_Cmd (char *tr[], int tam, bool show, struct Node** head) {

    pid_t pid;
    if ((pid=fork())==0) {
        return execute_Cmd(tr,tam, show);
        exit(255);
    }
    return 0;
}
void run_as_Cmd (char *tr[], int tam) {
    pid_t pid;
    if (tr[tam-1][0] == '&') {
        if ((pid=fork())==0) {
            setuid_aux(tr[1], true);
            execute_Cmd(tr+1,tam-1, true);
            exit(255);
        }
        return;
    } else {
        if ((pid=fork())==0) {
            setuid_aux(tr[1], true);
            execute_Cmd(tr+1, tam-1, true);
            exit (255);
        }
        waitpid(pid,NULL,0);
        return;
    }
}
void execute_as_Cmd (char *tr[], int tam) {

    setuid_aux(tr[1], true);
    execute_Cmd(tr+1, tam-1, true);

}
void unrecognized_Cmd (char *tr[], int tam, struct Node** head) {

    if (tr[tam-1][0] == '&') {
        tr[tam-1] = NULL;
        if (background_Cmd(tr-1, tam, false, head) == -1) {
            printf("Comando no reconocido\n");
            exit(0);
        }
    } else {
        if (foreground_Cmd(tr-1, tam+1, false, head) == -1) {
            printf("Comando no reconocido\n");
            exit(0);
        }
    }
}
int tam_array (char *tr[]) {

    int size;
    int i = 0;
    while (tr[i] != NULL) {

        size++;
        i++;

    }
    return size;
}

// zona test
void insertStart (struct Node** head, proceso proceso_) {

    struct Node* newNode = (struct Node*) malloc(sizeof(struct Node));
    newNode -> proceso = proceso_;
    newNode -> next = *head;
    *head = newNode;

}
void displayList (struct Node* head) {
    while (head != NULL) {
        int i = 0;
        printf("%d %ld ", head->proceso.pid, head->proceso.time);
        printf("%s ", head->proceso.arg[i]);
        while (head->proceso.arg[i] != NULL) {
            //printf("%s ", head->proceso.arg[i]);
            i++;
        }
        printf("%d\n", i);
        head = head->next;
    }
    printf("\n");
}

// void addProcess(proceso process, lista lista_) {
//
//     lista current = lista_;
//     elemento *newElement = (elemento *) malloc(sizeof(elemento));
//     if (newElement == NULL) {
//         printf("Add process failed\n");
//         exit(-1);
//     }
//     newElement->proceso = process;
//     newElement->next = NULL;
//     while (current->next) {
//         current = current->next;
//     }
//     current->next=newElement;
//     return;
// }
//
// void addProcess(proceso process, lista lista_) {
//
//     lista tmp = (lista) malloc(sizeof(lista));
//     tmp -> proceso = process;
//     link -> next = lista_;
//
//
// }
//
proceso recordProcess(char *tr[], pid_t pid) {
    proceso tmp;
    int i = 0;
    tmp.pid = pid;
    tmp.time = time(NULL);
    while (tr[i] != NULL) {
        strcpy(tmp.arg[i], tr[i]);
        i++;
    }
    return tmp;
}
// void printProcessList(lista lista) {
//     while (lista != NULL) {
//         printf("%d\n", lista->proceso.pid);
//         lista = lista->next;
//     }
// }
