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

void showList();
int remove_directory(const char *path);

int main (int arg, char *argv[]) {

	if (argv[1] == NULL){

		showList();

	} else if ( strcmp(argv[1], "-rec") == 0 ){

	    int check;

	   check = remove_directory(argv[2]);

	   if (check) {

	       check = remove(argv[2]);

	       if (check) {

	           printf("Unable to delete %s \n", argv[2]);
	           perror("Following error occurred");

	       }

	   }

	} else {

		int check;

		check = remove(argv[1]);

		if (check){

		printf("Unable to delete %s \n", argv[1]);
		perror("Following error occurred");
			}
	}

	return 0;
}

void showList(){

    DIR *folder;
    struct dirent *entry;
    struct stat filestat;

    printf("\n");
    folder = opendir(".");
    if(folder == NULL)
    {
        perror("Unable to read directory");
    }

    /* Read directory entries */
    while( (entry=readdir(folder)) ) {

        stat(entry->d_name,&filestat);
        if( S_ISDIR(filestat.st_mode) )
            printf("%4s: %s \n","Dir",entry->d_name);
        else
            printf("%4s: %s \n","File",entry->d_name);
    }
    printf("\n");
    closedir(folder);}
int remove_directory(const char *path) {
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d) {
      struct dirent *p;

      r = 0;
      while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;

          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;

          len = path_len + strlen(p->d_name) + 2;
          buf = malloc(len);

          if (buf) {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = remove_directory(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }

   if (!r)
      r = rmdir(path);

   return r;}
