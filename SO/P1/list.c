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
// void listDirRecLong(const char *name, bool hid);

int main(int arg, char *argv[]) {

    if ( argv[1] == NULL ) {

      listDirShort(".", false);

    } else if (strcmp (argv[1], "-dir") == 0) {

      int i = 2;

      while (argv[i] != NULL) {

        listDirShort(argv[i], false);
        i++;

      }

    } else {

      int i = 1;
      char flags[6] = {};
      char argDir[20] = {};
      int posDir = -1;
      strcpy(flags, "---- ");

      while (argv[i] != NULL) {

          if (strcmp(argv[i], "-dir")==0) {flags[0] = 'd'; posDir = i+1; strcpy(argDir ,argv[posDir]);}
          else if (strcmp(argv[i], "-hid")==0) flags[1] = 'h';
          else if (strcmp(argv[i], "-long")==0) flags[2] = 'l';
          else if (strcmp(argv[i], "-rec")==0) flags[3] = 'r';
          else{};
          i++;

      }

      listDirWithVariables( flags, argDir );


    }

}

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

      if (rec==true){ listDirRecShort(path, hid); } // deberia ser listDirRecLong pero da error.
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

  closedir(dir);

}

// void listDirRecLong(const char * basePath, bool hid) {
//
//   DIR *folder;
//   struct dirent *entry;
//   struct stat filestat;
//   time_t tiempo;
//   char permisos[12];
//   char name [6000];
//   char path[50];
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
//     char buff[20];
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
// }

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

    if ( (check = open(file, O_RDONLY)) == -1 ) {

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
