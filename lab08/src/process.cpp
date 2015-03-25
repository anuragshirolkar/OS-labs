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
#include "pageTable.cpp"
using namespace std;

extern sharedMemoryBundle shared_bundle;

#define SPEC_FILE_BASE "s"


struct process
{
	int id;
	int process_size;
	string spec_file;
	pageTable page_table;
	int access_count, modify_count, page_fault_count;

	process(int, int);
	void start();
	void print_stats();
	void access(int);
	void modify(int);
	int page_fault_handler(int);
};

process::process(int pid, int psize) {
	// constructor
	id = pid;
	process_size = psize;
	spec_file = SPEC_FILE_BASE + to_string(id);
	access_count = 0;
	modify_count = 0;
	page_fault_count = 0;
}

void process::start() { 
	// start the process thread
	ifstream specfile;
	specfile.open(spec_file.c_str());

	string action;
	while(specfile >> action) {
		shared_bundle.timer++;
		if (action == "End") {
			// no of access, modify, page faults should be printed
			print_stats();
			break;
		}
		int page_no, word_no;
		specfile >> page_no >> word_no;
		if (action == "Access") {
			access_count++;
			access(page_no);
		}
		else if (action == "Modify") {
			modify_count++;
			modify(page_no);
		}
	}
	return;
}

void process::print_stats() {
	cout << "Access count	  : " << access_count << endl;
	cout << "Modify count	  : " << modify_count << endl;
	cout << "Page fault count : " << page_fault_count << endl;
}

void process::access(int page_no) {
	bool page_exists = page_table.page_exists(page_no);
	if (!page_exists) {
		// call page fault handler
		page_fault_handler(page_no);
	}
	return;
}

void process::modify(int page_no) {
	return;
}

int process::page_fault_handler(int page_no) {
	int frame_no = shared_bundle.free_list.get_free_frame();
	
	return 0;
}
