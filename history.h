// COMP1521 18s2 mymysh ... command history
// Implements an interface to an abstract data object

#include <stdio.h>


// Functions on the Command History object

int initCommandHistory();
void addToCommandHistory(char *cmdLine, int seqNo);
void showCommandHistory(FILE *histFile);
int getCommandFromHistory(int cmdNo, char* line);
void saveCommandHistory();
void cleanCommandHistory();
