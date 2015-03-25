#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <pthread.h>
#include <time.h>
#include <sstream>
#include <unistd.h>


#include "process.cpp"
using namespace std;

#define INIT_FILE "init"
#define THREADING_ENABLED false

struct primary_thread {
    int no_processes;
 	map<int, process*> processes;

	// constructor
	primary_thread();

	void start();
	void create_process(int, int, int, int);
	void create_process_without_threading(int, int, int, int);
};

primary_thread::primary_thread() {
	no_processes = 0;
}

void primary_thread::start() {
	ifstream initfile;
	initfile.open(INIT_FILE);
	string action;
	while (initfile >> action) {
		int pid, size, frame_start, frame_end;
		char dump;
		initfile >> pid >> size >> frame_start >> dump >> frame_end;
		if (THREADING_ENABLED) create_process(pid, size, frame_start, frame_end);
		else create_process_without_threading(pid, size, frame_start, frame_end);
	}
	pthread_exit(NULL);
	return;
}

void *start_process(void * p){
	process *proc;
	proc = (struct process *) p;
	//usleep(1000*proc->id);
	proc->start();
	pthread_exit(NULL);}

void primary_thread::create_process(int pid, int size, int frame_start, int frame_end) {
	process *p = new process(pid, size, frame_start, frame_end);
	processes[pid] = p;
	no_processes++;
	pthread_t thread;
	int rc = pthread_create(&thread, NULL, start_process, (void *) p);
	return;
}

void primary_thread::create_process_without_threading(int pid, int size, int frame_start, int frame_end) {
	process *p = new process(pid, size, frame_start, frame_end);
	processes[pid] = p;
	no_processes++;
	p->start();
}

