#include "file_system.h"
//#include "list.h"
#include "list_d.h"
#include "list_d.c"

#include <sys/resource.h>
#include <signal.h>

#define SUWAIT 1 //Posibles errores en los define
#define SUNOWAIT 0
#define PRUNNING 0
#define PSTOPPED 1
#define PTERM 2
#define PSIGN 4
#define SHELL_EXIT_SUCCESS 2
#define SHELL_EXIT_FAILURE 3


struct listElement{

    struct tm time;
    char **command_line;
    int priority;
    pid_t pid;
    int *svalue;
    int status;
    int contarg;

};

char* chStats(int status){
    switch (status){
    case PRUNNING: return "Running";
    case PSTOPPED: return "Stopped";
    case PTERM : return "Terminated Normally";
    case PSIGN : return "Terminated By Signal";
    default : return "Undefined";
    }
}

int statsUpdate(struct listElement *item, int signal) {
 
    int status;
    int wpid = waitpid(item->pid,&status,signal ? 0 : WNOHANG | WUNTRACED | WCONTINUED);

    if (wpid == item->pid) {
        if (item->svalue == NULL)
            item->svalue = malloc(sizeof(int));

        if (WIFEXITED(status)){
            item->status = PTERM;    
            *item->svalue = WEXITSTATUS(status);
        }
        if (WIFSIGNALED(status)){
            item->status = PSIGN;    
            *item->svalue = WTERMSIG(status);
        }
        if (WIFSTOPPED(status)){
            item->status = PSTOPPED; 
            *item->svalue = WSTOPSIG(status);
        }
        if (WIFCONTINUED(status)){
            item->status = PRUNNING; 
            free(item->svalue);
        }
    }
    status = item->status;
    return status;
}

void getPriority_Cmd (char *pid_c);
void setpriority_Cmd (char *pid_c, char *newPrio);
void Cmd_getuid();
void Cmd_setuid (char *trozos[]);
int Cmd_fork ();
int Cmd_Execute (char *trozos[], int ntrozos);
int freeE(void *elem);
int Cmd_listarprocs (char *trozos[], int ntrozos,struct internal *i);
int Cmd_deleteProcess (char * trozos[], int ntrozos,struct internal *i);
int Cmd_proc (char *trozos[], int ntrozos,struct internal *i);
void Cmd_background (char *trozos[], int tam);
void Cmd_foreground (char *trozos[], int tam);
char *NombreSenal(int sen);
int Senal(char * sen);

int main(int argc, char const *argv[]) {

    char cadena[50];
    char *trozos[100];
    int ntrozos=0;
    struct internal *i;
    bool terminado = false;

    while (!terminado) {

        printf("$ ");
        fgets(cadena, 50, stdin);
        TrocearCadena( cadena, trozos );

        if ( trozos[0] == NULL ) {

            terminado = false;

        } else  {

            if ( strcmp(trozos[0], "getpriority") == 0 ) {

                getPriority_Cmd(trozos[1]);

            } else if ( strcmp(trozos[0], "setpriority") == 0 ) {

                setpriority_Cmd(trozos[1], trozos[2]);

            } else if ( strcmp(trozos[0], "getuid") == 0 ) {

                Cmd_getuid();

            } else if ( strcmp(trozos[0], "setuid") == 0 ) {

                Cmd_setuid(trozos);

            }else if ( strcmp(trozos[0], "fork") == 0 ) {

                Cmd_fork ();

            }else if ( strcmp(trozos[0], "listprocs") == 0 ) {

                Cmd_listarprocs (trozos,ntrozos,i);

            }else if ( strcmp(trozos[0], "foreground") == 0 ) {

                Cmd_foreground (trozos,ntrozos);

            }else if ( strcmp(trozos[0], "background") == 0 ) {

                Cmd_background (trozos,ntrozos);

            }else if ( strcmp(trozos[0], "deleteprocs") == 0 ) {

                Cmd_deleteProcess(trozos,ntrozos,i);

            }else if ( strcmp(trozos[0], "deleteprocs") == 0 ) {

                Cmd_proc(trozos,ntrozos,i);

            }else if ( strcmp(trozos[0], "quit") == 0 || strcmp(trozos[0], "end") == 0 || strcmp(trozos[0], "exit") == 0 ) {

                terminado = true;

            }

        }

    }

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void getPriority_Cmd (char *pid_c) { //SIN TOCAR MARTIN

    id_t pid;
    if (pid_c == NULL) {
        pid = getpid();
    } else {
        pid = (id_t) atoi(pid_c);
    }

    int which = PRIO_PROCESS;
    int ret = getpriority(which, pid);

    if ( ret == -1 ) {
        perror("Following error ocurred: ");

    } else {
        printf("Priority of process %d is %d\n", pid, ret);

    }
}


void setpriority_Cmd (char *pid_c, char *newPrio) { // SIN TOCAR MARTIN

    id_t pid;
    int priority;
    int which = PRIO_PROCESS;

    if ( pid_c == NULL ) {
        getPriority_Cmd(NULL);
        return;
    } else if ( newPrio == NULL ) {
        pid = getpid();
        priority = atoi (pid_c);
    } else {
        pid = (id_t) atoi(pid_c);
        priority = atoi (newPrio);
    }

    if ( priority < -20 || priority > 19 ) {
        printf("Priority value must be between -20 and 19\n");
        return;
    }

    if ( setpriority(which, pid, priority) == -1 ) {
        perror("Following error ocurred: ");
    } else {
        printf("Priority of process %d changed to %d\n", pid, priority);
    }
}

void changePriority (const char *tr){ // PARA OBTENER LOS PARAMETROS QUE QUIERO EN EXECUTE
   
    if (tr[0]!='@') return; 
    int n = atoi(&tr[1]);

    if (setpriority(PRIO_PROCESS,getpid(),n) == -1) 
        perror("Error setpriority in changePriority");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

char * NombreUsuario (uid_t uid)
{
    struct passwd *p;
    if ((p=getpwuid(uid))==NULL)
        return (" ??????");
    return p->pw_name;
}

uid_t UidUsuario (char * nombre){
    struct passwd *p;
    if ((p=getpwnam (nombre))==NULL)
        return  (uid_t) -1;
    return p->pw_uid;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Cmd_getuid () // NUMERO Y CRENDECIAL DEL PROCESO EN EJECUCION //Le quite el parametro char *tr[]
{
    uid_t real=getuid();
    uid_t efect=getuid();

    printf ("Credencial real : %d, (%s)\n", real, NombreUsuario (real));
    printf ("Credencial efectiva : %d, (%s)\n", efect, NombreUsuario (efect));
}


void Cmd_setuid (char *trozos[]) 
{
    uid_t uid;
    int n;
    if (trozos[0]==NULL || (!strcmp(trozos[0],"-l") && trozos[1]==NULL)){
        Cmd_getuid(trozos);
        return;
    }
    if (!strcmp(trozos[0],"-l")){
        if ((uid=UidUsuario(trozos[1]))==(uid_t) -1){
            printf ("Usuario no existente %s\n", trozos[1]);
            return;
        }
    }
    else if ((uid=(uid_t) ((n=atoi (trozos[0]))<0)? -1: n) ==(uid_t) -1){
        printf ("Valor no valido de la credencial %s\n",trozos[0]);
        return;
    }
    if (setuid (uid)==-1)
        printf ("Imposible cambiar credencial: %s\n", strerror(errno));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
int Cmd_fork (){ // FALTA meter parametro lista con process.Creo que ya no es necesario

    pid_t childpid;

    if ((childpid = fork())==-1){
        perror("Error: fork failed");
        return -1;
    }
    if (childpid!=0){
        waitpid(childpid,NULL,0); 
        return 0;
    }
    else{
        printf("Ejecutando proceso %d\n",getpid());
        return 0; 
    }
    
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////FUNCIONES AUXILIARES/////////////////////////////
int showE(struct listElement *item) {

    statsUpdate(item,0);
    char svalue[30];
    if (item->status & (PSIGN | PSTOPPED))
        sprintf(svalue,"| Signal: %s", NombreSenal(*item->svalue));
    else
        if (item->status & PTERM)
            sprintf(svalue,"| Value: %i",*item->svalue);
        else
            sprintf(svalue,"\b");

    char date[20];
    strftime(date,20,"%a %b %d %T %Y",&item->time); 
    printf(" Pid: %5i | Status: %s | Started: %s %s | Command: ",item->pid,chStats(item->status),date,svalue);

    for (int j = 0; j < item->contarg; j++) printf("%s ",item->command_line[j]);
    printf("\n");
    return 0;
}

int freeE(void *elem) {
    struct listElement *x = elem;
    for (int i = 0; i < x->contarg; i++) 
        free(x->command_line[i]);

    free(x->command_line);
    if (x->svalue != NULL)
        free(x->svalue);

    free(elem);

    return 0;
}

int search(list *l,struct listElement **p,pid_t pid) {
    *p = NULL;
    for (pos i = first(*l); !last(i); i = nextE(i)) {
        struct listElement *item = getElement(i);

        if (item->pid == pid) {
            *p = item;
            return 0;
        }
    }
  return 1; 
}
///////////////////////////////////////////////////////////////////////////////////////////////
void Cmd_execute (char *trozos[], int tam) {
    int which = PRIO_PROCESS;

    if ( tam == 1 ) {
        printf("Program not specified\n");
        return;
    }
    if ( trozos[tam-1][0] == '@' ) {
        trozos[tam-1]++;
        setpriority(which, getpid(), atoi(trozos[tam-1]));
        trozos[tam-1] = NULL;
        execvp(trozos[1],(char * const *) (&trozos[1]));
        perror("Following error ocurred: ");
        return;
    }
    execvp(trozos[1],(char * const *) (&trozos[1]));
    perror("Following error ocurred: ");
    return;
}
int Cmd_listarprocs (char *trozos[], int ntrozos,struct internal *i){ 
    for(pos j=first(i->process);!last(j);j=nextE(j))
        showE(getElement(j));
    return 0;
}

int Cmd_proc (char *trozos[], int ntrozos,struct internal *i){
    switch (ntrozos){
    case 1: 
        return Cmd_listarprocs(trozos,ntrozos,i);
    case 2:{ 
        if(!strcmp(trozos[1],"-fg")){ //no PID
            fprintf(stderr,"Error: no PID specified\n");
        return -1;
        }
        else{ //"proc pid"
            pid_t pid = atoi(trozos[1]);

            if ((pid<0)||(pid == getpid())) 
                return Cmd_listarprocs(trozos,ntrozos,i);

        struct listElement *item;
        if (search(&i->process,&item,pid) == 1){
            return Cmd_listarprocs(trozos,ntrozos,i); 
        }
        return showE(item);
        }
    }
    case 3:{ 
        if(!strcmp(trozos[1],"-fg")){
            struct listElement *item;
            pid_t pid = atoi(trozos[2]);
            list l= i->process;;// y aqui
            search(&i->process,&item,pid);
            statsUpdate(item,SUWAIT);
            showE(item);
            removeE(l,i->process);//error aqui
            return 0;
        }
        fprintf(stderr, "Error: Invalid flag specified\n");
        return -1;
    }
    default:
        fprintf(stderr, "Error: Not recognised");
        return -1;
    }
}

void Cmd_foreground (char *trozos[], int tam) {

    pid_t pid;
    if ((pid=fork())==0) {
        Cmd_execute(trozos, tam);
        exit (255);
    }
    waitpid(pid,NULL,0);
}

void Cmd_background (char *trozos[], int tam) {

    pid_t pid;
    if ((pid=fork())==0) {
        Cmd_execute(trozos,tam);
        exit(255);
    }
}

int Cmd_deleteProcess (char * trozos[], int ntrozos,struct internal *i){

    if ((trozos[1]==NULL)||((!strcmp(trozos[1],"-sig")&&(!strcmp(trozos[1],"-term"))))){
        fprintf(stderr, "Error: No option specified (-term | -sig)\n");
        return -1;
    }  

    int valor = PRUNNING;
    if (!strcmp(trozos[1],"-term")) 
        valor = PTERM;
    else if (!strcmp(trozos[1],"-sig")) 
            valor = PSIGN;
        
    struct listElement *item;
    list l = i->process;
    pos x = first(l);

    while (!isEmptyList(l) && !last(x)) {
        item = getElement(x);
        statsUpdate(item,SUNOWAIT);
        if (item->status == valor) {
            removeE(l,i->process);//error aqui
            x = first(l);
        }
        else
            x = nextE(x);
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////
/******************************SENALES ******************************************/
struct SEN{
    char *nombre;int senal;
};
static struct SEN sigstrnum[]={//Ya no da warning al subidividir en cajas.No probado sin funciona igual,aparentemente correcto
    {"HUP", SIGHUP},
    {"INT", SIGINT},
    {"QUIT", SIGQUIT},
    {"ILL", SIGILL},
    {"TRAP", SIGTRAP},
    {"ABRT", SIGABRT},
    {"IOT", SIGIOT},
    {"BUS", SIGBUS},
    {"FPE", SIGFPE},
    {"KILL", SIGKILL},
    {"USR1", SIGUSR1},
    {"SEGV", SIGSEGV},
    {"USR2", SIGUSR2},
    {"PIPE", SIGPIPE},
    {"ALRM", SIGALRM},
    {"TERM", SIGTERM},
    {"CHLD", SIGCHLD},
    {"CONT", SIGCONT},
    {"STOP", SIGSTOP},
    {"TSTP", SIGTSTP},
    {"TTIN", SIGTTIN},
    {"TTOU", SIGTTOU},
    {"URG", SIGURG},
    {"XCPU", SIGXCPU},
    {"XFSZ", SIGXFSZ},
    {"VTALRM", SIGVTALRM},
    {"PROF", SIGPROF},
    {"WINCH", SIGWINCH},
    {"IO", SIGIO},
    {"SYS", SIGSYS},
    /*senales que no hay en todas partes*/
    #ifdef SIGPOLL
        {"POLL", SIGPOLL},
    #endif
    #ifdef SIGPWR
        {"PWR", SIGPWR},
    #endif
    #ifdef SIGEMT
        {"EMT", SIGEMT},
    #endif
    #ifdef SIGINFO
        {"INFO", SIGINFO},
    #endif
    #ifdef SIGSTKFLT
        {"STKFLT", SIGSTKFLT},
    #endif
    #ifdef SIGCLD
        {"CLD", SIGCLD},
    #endif
    #ifdef SIGLOST
        {"LOST", SIGLOST},
    #endif
    #ifdef SIGCANCEL
        {"CANCEL", SIGCANCEL},
    #endif
    #ifdef SIGTHAW
        {"THAW", SIGTHAW},
    #endif
    #ifdef SIGFREEZE
        {"FREEZE", SIGFREEZE},
    #endif
    #ifdef SIGLWP
        {"LWP", SIGLWP},
    #endif
    #ifdef SIGWAITING
        {"WAITING", SIGWAITING},
    #endif
        {NULL,-1},
};    /*fin array sigstrnum */

int Senal(char * sen)  /*devuel el numero de senial a partir del nombre*/{
    int i;
    for (i=0; sigstrnum[i].nombre!=NULL; i++)
        if (!strcmp(sen, sigstrnum[i].nombre))
            return sigstrnum[i].senal; 
    return -1;
}
char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/{
                       /* para sitios donde no hay sig2str*/
int i;
for (i=0; sigstrnum[i].nombre!=NULL; i++)
    if (sen==sigstrnum[i].senal)
        return sigstrnum[i].nombre;
return ("SIGUNKNOWN");
}