// COMP1521 18s2 mysh ... command history
// Implements an abstract data object

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

// This is defined in string.h
// BUT ONLY if you use -std=gnu99
//extern char *strdup(const char *s);

// Command History
// array of command lines
// each is associated with a sequence number

#define MAXHIST 20
#define MAXSTR  200

#define HISTFILE ".mymysh_history"

typedef struct _history_entry {
   int   seqNumber;
   char commandLine[MAXSTR];
} HistoryEntry;

typedef struct _history_list {
   int nEntries;
   HistoryEntry commands[MAXHIST];
} HistoryList;

HistoryList CommandHistory;

// initCommandHistory()
// - initialise the data structure
// - read from .history if it exists

void pushHistory(HistoryList* list); //declaration;


int initCommandHistory()
{
   // TODO
   char* home = strdup(getenv("HOME"));
   home = realloc(home, strlen(home) + strlen(HISTFILE) + 2);
   strcat(home, "/");
   strcat(home, HISTFILE);


   FILE * historyFile = (fopen(home, "r"));
   int i = 0;
   CommandHistory.nEntries = 0;
   while (i < MAXHIST){ //Initialising
      CommandHistory.commands[i].seqNumber = 0;
      CommandHistory.commands[i].commandLine[0]='\0';
      i++;
   }


   i = 0;
   if (historyFile == NULL){
      return 0; //unsuccessful

   }
   if (historyFile != NULL){
      while(fscanf(historyFile, "%d %[^\n]s", &CommandHistory.commands[i].seqNumber,
                                         CommandHistory.commands[i].commandLine) 
                                         != EOF){


         CommandHistory.nEntries++;
         i++;

      }
   }

   int cmdNo = CommandHistory.commands[i-1].seqNumber;

   return cmdNo; //successful 
}

// [ 0 1 2 3 4 5]//

// addToCommandHistory()
// - add a command line to the history list
// - overwrite oldest entry if buffer is full

void addToCommandHistory(char *cmdLine, int seqNo)
{
   // TODO

   if (CommandHistory.nEntries < MAXHIST){
      CommandHistory.commands[CommandHistory.nEntries].seqNumber = seqNo;
      strcpy(CommandHistory.commands[CommandHistory.nEntries].commandLine, cmdLine);
      CommandHistory.nEntries++;
   }
   else{

      pushHistory(&CommandHistory);
      strcpy(CommandHistory.commands[MAXHIST - 1].commandLine, cmdLine);
      CommandHistory.commands[MAXHIST-1].seqNumber = seqNo;
      
   }
}

// showCommandHistory()
// - display the list of 

void showCommandHistory(FILE *outf)
{
   // TODO
   
   int i = 0; 
   int seq;
   char* string = malloc(MAXSTR * sizeof(char));
   strcpy(string, CommandHistory.commands[i].commandLine);
   seq = CommandHistory.commands[i].seqNumber;
   while(i < MAXHIST && strcmp(string, "") != 0){

      fprintf(outf," %3d  %s\n",seq, string);
      i++;
      strcpy(string, CommandHistory.commands[i].commandLine);
      seq = CommandHistory.commands[i].seqNumber;
   }




}

// getCommandFromHistory()
// - get the command line for specified command
// - returns NULL if no command with this number

int getCommandFromHistory(int cmdNo, char* line)
{
   // TODO

   int i = 0;
   while (i < MAXHIST){

      if(cmdNo == CommandHistory.commands[i].seqNumber){
         strcpy(line, CommandHistory.commands[i].commandLine);
         return 0;
      }
      i++;
   }


   return 1;
   
}

// saveCommandHistory()
// - write history to $HOME/.mymysh_history

void saveCommandHistory()
{
   char* home = strdup(getenv("HOME"));
   home = realloc(home, strlen(home) + strlen(HISTFILE) + 2);
   strcat(home, "/");
   strcat(home, HISTFILE);


   FILE * saveto = (fopen(home, "w"));

   int i = 0;

   while (i < MAXHIST){

      fprintf(saveto, " %3d  %s\n", CommandHistory.commands[i].seqNumber,
                                    CommandHistory.commands[i].commandLine);

      i++;
   }
   fclose(saveto);

   // TODO

}

// cleanCommandHistory
// - release all data allocated to command history

void cleanCommandHistory()
{
   // TODO
   int i = 0;
   while(i < MAXHIST){

      i++;
   }
   
}

void pushHistory(HistoryList* list){
   int i = 0;

   while(i < MAXHIST - 1){
      strcpy(list->commands[i].commandLine, list->commands[i+1].commandLine);
      list->commands[i].seqNumber = list->commands[i+1].seqNumber;
      i++;
   }

}
