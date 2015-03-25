#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <wait.h>
#include <sstream>
#include <string.h>
#include <fcntl.h>
using namespace std;

#define MAXLINE 1000
#define DEBUG 1

/* Function declarations and globals */
int parent_pid ;
vector<string> tokenize(string);
int execute_command(vector<string>) ;

// Helper function for function below
vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

/**
 * splits the string with delimiter and returns a vector of strings
 */
vector<vector<string> > split(vector<string> & command) {
	vector<vector<string> > commands;
	vector<string> current_command;
	for (int i = 1; i < command.size(); i++) {
		if (command[i] == ":::") {
			commands.push_back(current_command);
			current_command.clear();
		}
		else {
			current_command.push_back(command[i]);
		}
	}
	if (current_command.size()) commands.push_back(current_command);
	return commands;
}


/**
 * splits the string with delimiter and returns a vector of strings
 */
vector<vector<string> > split_pipes(vector<string> & command) {
	vector<vector<string> > commands;
	vector<string> current_command;
	for (int i = 0; i < command.size(); i++) {
		if (command[i] == "|") {
			commands.push_back(current_command);
			current_command.clear();
		}
		else {
			current_command.push_back(command[i]);
		}
	}
	if (current_command.size()) commands.push_back(current_command);
	return commands;
}

void deallocated(const char** pointers, int size){
	for (int i=0; i< size; i++){
		delete[] pointers[i];
	}
	delete[] pointers;
}

// Handles SIGINT and SIGQUIT
void handler(int dummy){
	cin.clear();

	if (getpid() != parent_pid){
		exit(0);
	}
}

int main()
{
	// assigning handler functions
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);

	parent_pid = getpid();
	//cout<<parent_pid<<endl;
	string input;
	vector<string> history;
	while(1) {
		cout << "$ " << flush;
		getline(cin, input);
		// char *in = fgets(input, MAXLINE, stdin); // Taking input one line at a time
		//Checking for EOF
		if (cin.eof()) {
			//if (in == NULL){
			if (DEBUG) printf("jash: EOF found. Program will exit.\n");
			else printf("\n");
			break ;
		}
		if (input == "history") {
			for (int i = 0; i < history.size(); i++) cout << history[i] << endl;
		}
		else if (input == "_" && history.size()) input = history.at(history.size() - 1) ;
		else {
			if (history.size() && history.at(history.size()-1) != input) history.push_back(input);
			else if (history.size() == 0) history.push_back(input);
		}
		// Calling the tokenizer function on the input line    
		vector<string> tokens = tokenize(input);	
		// Executing the command parsed by the tokenizer
		if (input != "history") execute_command(tokens) ; 
		
	}
	
	/* Kill all Children Processes and Quit Parent Process */
	return 0 ;
}


// Tokenizes a string into vector of strings (tokens)
vector<string> tokenize(string command) {
	vector<string> ans;
	bool double_quotes = false;
	string s = "";
	for (int i = 0; i < command.size(); i++) {
		if (command[i] == '\\') {
			if (i < command.size()-1) s += command[i+1];
			i++;
		}
		else if ((command[i] == ' ' || command[i] == '\n' || command[i] == '\t') && !(double_quotes)) {
			if (s.size()) ans.push_back(s);
			s = "";
		}
		else if (command[i] == '"') {
			double_quotes = !double_quotes;
		}
		else {
			s += command[i];
		}
	}
	if (s.size()) ans.push_back(s);
	return ans;
}

// executes the commands for jash script
int execute_command(vector<string> tokens) {
	int error;
	FILE* fp;
	char * line = NULL;
  	size_t len = 0;
   	ssize_t read;
   	char* home = getenv("HOME");
   	string homes(home);


	
	
	
	/**
	 * Replaces ~ with home directory location
	 */
   	for (int i = 0; i< tokens.size(); i++){
   		if (tokens[i] == "~"){
   			tokens[i] = homes;
   		}
   		else if (tokens[i][0] == '~' && tokens[i][1] == '/'){
   			tokens[i] = homes + tokens[i].substr(1);
   		}
   	}
	

	/** 
	 * Takes the tokens from the parser and based on the type of command 
	 * and proceeds to perform the necessary actions. 
	 * Returns 0 on success, -1 on failure. 
	 */
	if (tokens.size() == 0) {
	 	return -1 ; 				// Null Command
	 }
	if (tokens[0] == "") {
		return 0 ;					// Empty Command
	} else if (tokens[0] == "exit") {
		exit(0);
		/* Quit the running process */
		return 0 ;
	}

	/**
	 * If the command contains pipelies
	 */

	if (tokens[0] != "parallel" && tokens[0] != "sequential") {
		vector<vector<string> > pipe_commands = split_pipes(tokens);
		// if (pipe_commands.size() > 1) {
		// 	int pipes[2];
		// 	pipe(pipes);
		// 	int save_in = dup(0);
		// 	int save_out = dup(1);

		// 	int pid1 = fork();
		// 	if (pid1 == 0) {
		// 		dup2(pipes[1], 1);
		// 		close(pipes[0]);
		// 		execute_command(pipe_commands[0]);
		// 		exit(0);
		// 	}
		// 	else{
		// 		int pid2 = fork();
		// 		if (pid2 == 0) {
		// 			close(pipes[1]);
		// 			dup2(pipes[0], 0);
		// 			execute_command(pipe_commands[1]);
		// 			exit(0);
		// 		}
		// 		else {
		// 			close(pipes[0]);
		// 			close(pipes[1]);
		// 			waitpid(pid2, NULL, 0);
		// 			waitpid(pid1, NULL, 0);
		// 		}
		// 	}
		// 	return 0;
		// }

		/**
		 * Works with any number of pipelines :)
		 */
		if (pipe_commands.size() > 1) {
			int pipes[2*pipe_commands.size()-2];
			int save_in = dup(0);
			int save_out = dup(1);
			vector<int> pids;
			for (int i = 0; i < pipe_commands.size()-1; i++) pipe(pipes+2*i);
			for (int i = 0; i < pipe_commands.size(); i++) {
				int pid = fork();
				if (pid == 0) {
					if (i) dup2(pipes[2*(i-1)], 0);
					else dup2(save_in, 0);
					if (i < pipe_commands.size()-1) dup2(pipes[2*i+1], 1);
					else dup2(save_out, 1);
					for (int j = 0; j < 2*pipe_commands.size()-2; j++) close(pipes[j]);
					execute_command(pipe_commands[i]);
					exit(0);
				}
			}
			for (int i = 0; i < 2*pipe_commands.size()-2; i++) close(pipes[i]);
			int status;
			for (int i = 0; i < pipe_commands.size(); i++) wait(&status);
			return 0;
		}
	}
	
	/**
	 * For input file redirection
	 * infile = name of the file to take input from
	 * outfile = name of the file to redirect output to
	 * append_output = whether to append the output to the output file
	 */
	string infile = "", outfile = "";
	bool append_output = false;
	if (tokens[0] != "parallel" && tokens[0] != "sequential") {
		for (int i = 0; i < tokens.size(); i++) {
			if (tokens[i] == "<") {
				infile = tokens[i+1];
				i++;
			}
			else if(tokens[i] == ">") {
				outfile = tokens[i+1];
				append_output = false;
			}
			else if(tokens[i] == ">>") {
				outfile = tokens[i+1];
				append_output = true;
			}
		}

		vector<string> tokens_temp = tokens;
		tokens.clear();
		for (int i = 0;i < tokens_temp.size(); i++) {
			if (tokens_temp[i] == "<" || tokens_temp[i] == ">" || tokens_temp[i] == ">>") {
				break;
			}
			tokens.push_back(tokens_temp[i]);
		}
	}
	
	if (tokens[0] == "cd") {
		
		if (tokens.size() < 2){
			tokens.push_back(homes);
		}
		/* Change Directory, or print error on failure */
		error = chdir(tokens[1].c_str());
		//printf ("%d\n", error);
		if (error != 0){
			perror("cd");
			return -1;
		}
		return 0 ;
	} else if (tokens[0] == "parallel") {
		/* Analyze the command to get the jobs */
		/* Run jobs in parallel, or print error on failure */
		vector<vector<string> > v = split(tokens);
		for (int i = 0; i < v.size(); i++) {
			int pid = fork();
			if (pid == 0) {
				exit(execute_command(v[i]));
			}
			else if (pid == -1) return -1;
		}
		for (int i = 0; i < v.size(); i++) waitpid(0, NULL, 0);
		return 0 ;
	} else if (tokens[0] == "sequential") {
		/* Analyze the command to get the jobs */
		/* Run jobs sequentially, print error on failure */
		/* Stop on failure or if all jobs are completed */
		vector<vector<string> > v = split(tokens);
		for (int i = 0; i < v.size(); i++) {
			int code = execute_command(v[i]);
			if (code == -1) return -1;
		}
		return 0;					// Return value accordingly
	} else if (tokens[0] == "sequential_or") {
		/* Analyze the command to get the jobs */
		/* Run jobs sequentially, print error on failure */
		/* Stop on success or if all jobs are completed */
		vector<vector<string> > v = split(tokens);
		for (int i = 0; i < v.size(); i++) {
			int code = execute_command(v[i]);
			if (code == 0) return 0;
		}
		return -1;					// Return value accordingly
	} else {
		/* Either file is to be executed or batch file to be run */
		/* Child process creation (needed for both cases) */
		int save_in = dup(0), save_out = dup(1);
		int pid = fork() ;
		if (pid == -1) return -1;
		if (pid == 0) {
			if (tokens[0] == "run") {
				/* Locate file and run commands */
				/* May require recursive call to execute_command */
				/* Exit child with or without error */
				fp = fopen(tokens[1].c_str(), "r");
				if (fp != NULL){
					while ((read = getline(&line, &len, fp)) != -1) {
			           tokens = tokenize(line);
			           execute_command(tokens);
			       }
				}
				else{
					perror("Error");
					fclose(fp);
					exit(-1);
				}
				fclose(fp);
				exit (0) ;
			}
			else {

				if (infile != "") {
					int in = open(infile.c_str(), O_RDONLY);
					if (in == -1) {
						cerr << "Error while reading from input file " << infile << endl;
						exit(-1);
					}
					dup2(in, 0);
					
				}
				if (outfile != "") {
					int out;
					if (append_output) out = open(outfile.c_str(), O_WRONLY | O_APPEND | O_CREAT , 0777);
					else out = open(outfile.c_str(), O_WRONLY | O_TRUNC | O_CREAT , 0777);
					if (out == -1) {
						cerr << "Error while redirecting to output file " << outfile << endl;
						exit(-1);
					}
					dup2(out, 1);
				}
				// Uses execvp function to find the file from PATH variables execute it with given arguments
				const char **argv = new const char* [tokens.size()+2];   // extra room for program name and sentinel
			    for (int j = 0;  j < tokens.size()+1;  ++j)     // copy args
			            argv [j] = tokens[j] .c_str();
			    argv [tokens.size()] = NULL;

				error = execvp(tokens[0].c_str(), (char**) argv);

				if (error == -1){
					cerr<<"Command "<<tokens[0]<<" not found"<<endl;
					deallocated(argv, tokens.size() + 2);
					exit(-1);
				}
				deallocated(argv, tokens.size() + 1);
				exit(0);
			}
		}
		else {

			/* Parent Process */
			/* Wait for child process to complete */
			int status;
			waitpid(pid, &status, 0);
			dup2(save_in, 0);
			dup2(save_out, 1);
			if (status == 0){
				return 0;
			}
			return -1;
		}
	}
	return 1 ;
}

