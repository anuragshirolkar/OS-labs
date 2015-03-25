#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <thread>
#include <time.h>
#include <sstream>
#include <unistd.h>
#include "process.cpp"
using namespace std;

#define INIT_FILE "init"

extern sharedMemoryBundle shared_bundle;

struct primaryThread {
    vector<process *> processes;
	vector<thread *> process_threads;

	primaryThread();
	void start();
};


primaryThread::primaryThread() {
	// constructor
}

void primaryThread::start() {
	// start the thread
	ifstream initfile;
	initfile.open(INIT_FILE);

	string action;
	while (initfile >> action) {
		if (action == "Memory_size") {
			initfile >> shared_bundle.memory_size;
		}
		else if (action == "Lower_threshold") {
			initfile >> shared_bundle.lower_threshold;
		}
		else if (action == "Upper_threshold") {
			initfile >> shared_bundle.upper_threshold;
		}
		else if (action == "Create") {
			int pid, psize;
			initfile >> pid >> psize;
			process *p = new process(pid, psize);
			processes.push_back(p);
			thread *t = new thread(&process::start, p);
			process_threads.push_back(t);
		}
	}
	for (int i = 0; i < process_threads.size(); i++) {
		process_threads[i]->join();
	}

	return;
}
