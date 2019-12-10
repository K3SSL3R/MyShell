// mysh.c ... a personal shell

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <glob.h>
#include <assert.h>
#include <fcntl.h>
#include "history.h"
#include <errno.h>
// This is defined in string.h
// BUT ONLY if you use -std=gnu99
//extern char *strdup(char *);

// Function forward references

void trim(char *);
int strContains(char *, char *);
char **tokenise(char *, char *);
char **fileNameExpand(char **);
void freeTokens(char **);
char *findExecutable(char *, char **);
int isExecutable(char *);
void prompt(void);
void pwd();

//My functions
void execute(char **args, char** path, char** envp);
int validCommCheck(char** args);

// Global Constants

#define MAXLINE 200

// Global Data

/* none ... unless you want some */


// Main program
// Set up enviroment and then run main loop
// - read command, execute command, repeat

int main(int argc, char *argv[], char *envp[])
{
   pid_t pid;   // pid of child process
   int stat;    // return status of child
   char **path; // array of directory names
   int cmdNo;   // command number
   int i;       // generic index

   // set up command PATH from environment variable
   for (i = 0; envp[i] != NULL; i++) {
      if (strncmp(envp[i], "PATH=", 5) == 0) break;
   }
   if (envp[i] == NULL)
      path = tokenise("/bin:/usr/bin",":");
   else
      // &envp[i][5] skips over "PATH=" prefix
      path = tokenise(&envp[i][5],":");
#ifdef DBUG
   for (i = 0; path[i] != NULL;i++)
      printf("path[%d] = %s\n",i,path[i]);
#endif

   // initialise command history
   // - use content of ~/.mymysh_history file if it exists

   cmdNo = initCommandHistory();
   cmdNo++;

   // main loop: print prompt, read line, execute command

   char line[MAXLINE];
   prompt();
   while (fgets(line, MAXLINE, stdin) != NULL) {
      trim(line); // remove leading/trailing space


      // TODO
      // Code to implement mainloop goes here
      // Uses
      // - addToCommandHistory()
      // - showCommandHistory()
      // - and many other functions
      // TODO


      // before command, need to print pathname of the command executable: use 
      // the full pathname if the command is found by using a search of the PATH;
      // if the command name is relative, use the name as given.
      
      //after command, print command return status
      if(  line[0]=='\0' 
         ||line[0]=='\n'){
         prompt();
         continue;
      }
      char** args = tokenise(line, " "); 
      fileNameExpand(args);
      if(  line[0]=='\0' 
         ||line[0]=='\n'){
         prompt();
         continue;
      }
      else if(line[0] == '!'){
         char number[MAXLINE];
         int noCheck = 0;
         int i = 1;
         while(   line[i] != '\0'
               && line[i] != ' '
               &&( 48 <= line[i] && line[i] <= 57 )){
            number[i-1] = line[i];
            i++;
         }
         noCheck = atoi(number);
         char *newLine = malloc(MAXLINE);
         int validCheck = getCommandFromHistory(noCheck, newLine);
         if(validCheck == 1){
            printf("INVALID COMMAND.\n");
            prompt();
            continue;
         }
         args = tokenise(newLine, " ");

      }

      else if(strcmp(args[0], "exit") == 0){
         break;

      }
      else if(strcmp(args[0], "pwd") == 0){
         pwd(); //prints working directory
         prompt();
         addToCommandHistory(line,cmdNo);
         cmdNo++;
         continue;//next iteration
      }

      else if(strcmp(args[0],"cd") == 0){
         chdir(args[1]);
         pwd();
         prompt();
         addToCommandHistory(line,cmdNo);
         cmdNo++;
         continue;
      }

      else if(strcmp(args[0],"h") == 0 
            ||strcmp(args[0],"history") == 0){
         showCommandHistory(stdout);
         prompt();
         addToCommandHistory(line,cmdNo);
         cmdNo++;
         continue;
      }

      int argsCounter = 0;
      while(args[argsCounter] != NULL){
         argsCounter++;
      }

      int redirectionFileDesc = -1;
      int redirType = -1;
      int checkCounter = 0; //CHECK FOR OTHER < AND > IN ARGS
      int extraPresent = 0; //condition toggled if > found outside 2nd last arg
      while(checkCounter < argsCounter){
         if((checkCounter != 0 && checkCounter == argsCounter - 2)){
            checkCounter++;
            continue;
         }

         if(strcmp(args[checkCounter], ">") == 0
         || strcmp(args[checkCounter], "<") == 0 ){
            extraPresent = 1;
         }

         checkCounter++;
      }

      if (extraPresent == 1){
         printf("Error: Extra '>' or '<' character present\n");
         prompt();
         continue;
      }
      if (  argsCounter > 2 &&
            (args[argsCounter - 2][0] == '<' 
         || args[argsCounter - 2][0] == '>')){

         if(args[argsCounter - 2][0] == '<' ){
               redirectionFileDesc = open(args[argsCounter-1], O_RDONLY);
               redirType = 0;
            if (redirectionFileDesc == -1){ //ERRORS

               if(errno == ENOENT){
                  printf("Input Redirection Error: File does not exist.\n");
               }

               else if(errno == EACCES){
                  printf("Input Redirection Error: File permission not granted.\n");
               }

               else if(errno == ENOTDIR){
                  printf("Input Redirection Error: No file descriptor associated with directory.\n");
               }
               else if(errno == EISDIR){
                  printf("Input Redirection Error: Attempting to write to directory.\n ");
               }
               prompt();
               continue; 
            }
         }
        
         else if(args[argsCounter - 2][0] == '>'){
            int redirectionFileDesc = open(args[argsCounter-1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            
            redirType = 1;
            if (redirectionFileDesc == -1){ //ERRORS
               if (errno == ENOENT){
                  printf("Output Redirection Error: File does not exist.\n");
               }
               else if( errno == EACCES){
                  printf("Output Redirection Error: File permission not granted.\n");

               }
               else if( errno == ENOTDIR){
                  printf("Output Redirection Error: No file descriptor associated with directory\n");
               }
               else if( errno == EISDIR){
                  printf("Output Redirection Error: Attempting to write to directory.\n");
               }
            prompt();
            continue;
         
            }

         }

      }



         addToCommandHistory(line, cmdNo);
         cmdNo++;
         pid = fork();


         if(pid == 0) {
         if (redirectionFileDesc != -1){
            if (redirType == 0){
               dup2(redirectionFileDesc, STDIN_FILENO);
            }
            else{
               dup2(redirectionFileDesc, STDOUT_FILENO);
            }
            close(redirectionFileDesc);

         }
            execute(args, path, envp);
         } else {
            close(redirectionFileDesc);
            wait(&stat);
            freeTokens(args);
            printf("--------------------\nreturn %d\n", 
                                            WEXITSTATUS(stat));


         }
      
      prompt();
   }
   saveCommandHistory();
   cleanCommandHistory();
   printf("\n");
   return(EXIT_SUCCESS);
}

// fileNameExpand: expand any wildcards in command-line args
// - returns a possibly larger set of tokens
char **fileNameExpand(char **tokens)
{
   // TODO
   int tokenCounter = 0;
   while(tokens[tokenCounter] != NULL){
      tokenCounter++;
   }
   tokenCounter++;

   glob_t Globbo;
   for (int i = 0; i < tokenCounter ;i++){
      if (strContains(tokens[i], "~*[?") == 0){
         continue;

         glob(tokens[i], GLOB_NOCHECK | GLOB_TILDE, NULL, &Globbo);

         if(Globbo.gl_pathc == 0){
            continue;
         }

         if(Globbo.gl_pathc > 1){
            tokenCounter = tokenCounter + Globbo.gl_pathc - 1;
            tokens = realloc(tokens, tokenCounter * sizeof(char*));
            for (int j = tokenCounter - Globbo.gl_pathc; j > i; j--){
               tokens[j + Globbo.gl_pathc - 1] = tokens[j];
            }
         }
         free(tokens[i]);
         for(int j = 0; j < Globbo.gl_pathc; j++){
            tokens[i + j] = strdup(Globbo.gl_pathv[j]);
         }
         globfree(&Globbo);
      }
   } 
   return tokens;
}

// findExecutable: look for executable in PATH
char *findExecutable(char *cmd, char **path)
{
      char executable[MAXLINE];
      executable[0] = '\0';
      if (cmd[0] == '/' || cmd[0] == '.') {
         strcpy(executable, cmd);
         if (!isExecutable(executable))
            executable[0] = '\0';
      }
      else {
         int i;
         for (i = 0; path[i] != NULL; i++) {
            sprintf(executable, "%s/%s", path[i], cmd);
            if (isExecutable(executable)) break;
         }
         if (path[i] == NULL) executable[0] = '\0';
      }
      if (executable[0] == '\0')
         return NULL;
      else
         return strdup(executable);
}

// isExecutable: check whether this process can execute a file
int isExecutable(char *cmd)
{
   struct stat s;
   // must be accessible
   if (stat(cmd, &s) < 0)
      return 0;
   // must be a regular file
   //if (!(s.st_mode & S_IFREG))
   if (!S_ISREG(s.st_mode))
      return 0;
   // if it's owner executable by us, ok
   if (s.st_uid == getuid() && s.st_mode & S_IXUSR)
      return 1;
   // if it's group executable by us, ok
   if (s.st_gid == getgid() && s.st_mode & S_IXGRP)
      return 1;
   // if it's other executable by us, ok
   if (s.st_mode & S_IXOTH)
      return 1;
   return 0;
}

// tokenise: split a string around a set of separators
// create an array of separate strings
// final array element contains NULL
char **tokenise(char *str, char *sep)
{
   // temp copy of string, because strtok() mangles it
   char *tmp;
   // count tokens
   tmp = strdup(str);
   int n = 0;
   strtok(tmp, sep); n++;
   while (strtok(NULL, sep) != NULL) n++;
   free(tmp);
   // allocate array for argv strings
   char **strings = malloc((n+1)*sizeof(char *));
   assert(strings != NULL);
   // now tokenise and fill array
   tmp = strdup(str);
   char *next; int i = 0;
   next = strtok(tmp, sep);
   strings[i++] = strdup(next);
   while ((next = strtok(NULL,sep)) != NULL)
      strings[i++] = strdup(next);
   strings[i] = NULL;
   free(tmp);
   return strings;
}

// freeTokens: free memory associated with array of tokens
void freeTokens(char **toks)
{
   for (int i = 0; toks[i] != NULL; i++)
      free(toks[i]);
   free(toks);
}

// trim: remove leading/trailing spaces from a string
void trim(char *str)
{
   int first, last;
   first = 0;
   while (isspace(str[first])) first++;
   last  = strlen(str)-1;
   while (isspace(str[last])) last--;
   int i, j = 0;
   for (i = first; i <= last; i++) str[j++] = str[i];
   str[j] = '\0';
}

// strContains: does the first string contain any char from 2nd string?
int strContains(char *str, char *chars)
{
   for (char *s = str; *s != '\0'; s++) {
      for (char *c = chars; *c != '\0'; c++) {
         if (*s == *c) return 1;
      }
   }
   return 0;
}

// prompt: print a shell prompt
// done as a function to allow switching to $PS1
void prompt(void)
{
   printf("mymysh$ ");
}
void execute(char **args, char **path, char **envp) { //for executing (obviously)
   char* program = findExecutable(args[0], path);

   if(program == NULL){
      printf("%s :COMMAND NOT FOUND.",program);
      exit(1);
   }
   else{
      printf("Running %s ...\n--------------------\n", program);
      if(execve(program, args, envp) == -1){
         perror("Execution failed.");
      }
      free(program);
   }

}
void pwd(){ //prints current working directory
   char dir[MAXLINE] = {'\0'};
   getcwd(dir, MAXLINE);
   printf("%s\n",dir);
}

int validCommCheck(char** args){
return 1;
}
