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
    map<int, process *> processes;
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
		//usleep(100);
		if (action == "Memory_size") {
			initfile >> shared_bundle.memory_size;
			shared_bundle.initialize_free_list();
		}
		else if (action == "Lower_threshold") {
			initfile >> shared_bundle.lower_threshold;
			shared_bundle.update_free_list();
		}
		else if (action == "Upper_threshold") {
			initfile >> shared_bundle.upper_threshold;
			shared_bundle.update_free_list();
		}
		else if (action == "Create") {
			int pid, psize;
			string uname;
			initfile >> uname >> pid >> psize;
			process *p = new process(pid, psize, uname);
			processes[pid] = p;
			shared_bundle.setup_new_process(pid);
			thread *t = new thread(&process::start, p);
			process_threads.push_back(t);
			usleep(100000);
		}
		else if(action == "Page_table") {
			cout << "printing page tables" << endl;
			for (map<int, process *>::iterator it = processes.begin(); it != processes.end(); it++) {
				cout << "page table of process " << it->first << endl;
				it->second->page_table.print();
			}
		}
		else if(action == "uname") {
			string uname;
			initfile >> uname;
			shared_bundle.m.lock();
			int rootblock = shared_bundle.dsk.get_root();
			shared_bundle.dsk.mkdir(rootblock, uname.c_str());
			shared_bundle.curdirs[uname] = shared_bundle.dsk.cd(rootblock, uname.c_str());
			shared_bundle.m.unlock();
		}
		else if (action == "mkdir") {
			string uname, dirname;
			initfile >> uname >> dirname;
			shared_bundle.m.lock();
			int curdir = shared_bundle.curdirs[uname];
			shared_bundle.dsk.mkdir(curdir, dirname.c_str());
			shared_bundle.m.unlock();
		}
		else if (action == "cd") {
			string uname, dirname;
			initfile >> uname >> dirname;
			shared_bundle.m.lock();
			int curdir = shared_bundle.curdirs[uname];
			int newdir = shared_bundle.dsk.cd(curdir, dirname.c_str());
			if (newdir < 0) {
				
			}
			else {
				shared_bundle.curdirs[uname] = newdir;
			}
			shared_bundle.m.unlock();
		}
		else if (action == "ls") {
			string uname;
			initfile >> uname;
			shared_bundle.m.lock();
			int curdir = shared_bundle.curdirs[uname];
			shared_bundle.dsk.ls(curdir);
			shared_bundle.m.unlock();
		}
	}
	for (int i = 0; i < process_threads.size(); i++) {
		process_threads[i]->join();
	}
	return;
}
