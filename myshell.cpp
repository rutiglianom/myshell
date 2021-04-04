// Matthew Rutigliano
// 23 January 2021
// CPSC 3500
// myshell.cpp

#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>


using namespace std;

int main() {
	
	cout << "myshell$";
	string line;
	getline(cin, line);
	line.append(" ");	// Add trailing space for counting terms
	
	// Tokenizing
	char buf[10][500]; 
	int posx = 0;
	int posy = 0;
	int terms[10] = { 0 };
	for(int i=0; (size_t)i<line.size(); i++){
		// If pile, new row
		if (line[i] == '|'){
			posx = 0;
			posy++;
			while(line[i+1] == ' ') 	// Skip trailing spaces
				i++;
		} else {
			// If space, up term count
			if (line[i] == ' '){
				terms[posy]++;
				buf[posy][posx] = (char)NULL; 	// Delimiting character
				while(line[i+1] == ' ') 	// Skip trailing spaces
					i++;
			}
			else
				buf[posy][posx] = line[i];
			posx++;
		}
	}
	// Count final term
	posy++;
	
	
	// Creating Argument list
	char* args[posy][50];
	for(int i=0; i<posy; i++){
		// Reset counter and list
		posx = 0;
		 
		for(int j=0; j<terms[i]; j++){
			args[i][j] = (char*)&buf[i][posx];
			posx += strlen(args[i][j]) + 1;
		}
		args[i][terms[i]] = (char*)NULL;	// End Argument list
	}
	
	// Pipes
	if (posy > 1){
		// Create N-1 Pipes
		int fd[posy-1][2];
		for(int i=0; i<posy-1; i++){
			// Exits if unsuccessful
			if (pipe(fd[i]))
				exit(1);
		}
		
		pid_t pid;
		for(int i=0; i<posy; i++){
			pid = fork();
			// Child
			if (!pid) {
				// Middle Child
				if (i > 0 && i != (posy-1)){
					dup2(fd[i-1][0], 0);
					dup2(fd[i][1], 1);
				}
				// Last Born
				else if (i == (posy-1)){
					dup2(fd[i-1][0], 0);
				}
				// First Born
				else {
					dup2(fd[i][1], 1);
				}
				// Close everything
				for (int x=0; x<posy-1; x++){
					close(fd[x][1]);
					close(fd[x][0]);
				}
				// Run Command
				int STATUS;
				STATUS = execvp(args[i][0], args[i]);
				exit(STATUS);
			}
			// Parent
			else if (pid > 0) {
				// Do nothing
			}
			// Error with Fork
			else {
				cout << "Error with fork\n";
				exit(1);
			}
		}
		
		// Close everything
		for (int x=0; x<posy-1; x++){
			close(fd[x][1]);
			close(fd[x][0]);
		}
		
		// Print child statuses
		int status;
		for(int i=0; i<posy; i++){
			wait(&status);
			printf("Process %i exits with %i\n", (int)pid, status);
		}
	}
	
	// No pipes
	else {
		pid_t pid = fork();
		// Child
		if (!pid) {
			execvp(args[0][0], args[0]);
		}
		// Parent
		else if (pid > 0) {
			int status;
			wait(&status);
			printf("process %i exits with %i\n", (int)pid, status);
		}
		// Error with fork
		else {
			cout << "Error with fork\n";
			exit(1);
		}
	}
	exit(0);
}