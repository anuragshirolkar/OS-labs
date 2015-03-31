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
#include <mutex>
#include <condition_variable>
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
	void operation(int, bool);
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
		{
			lock_guard<mutex> lk(shared_bundle.m);
			shared_bundle.timer++;
		}
		
		if (action == "End") {
			// no of access, modify, page faults should be printed
			print_stats();
			break;
		}
		int page_no, word_no;
		specfile >> page_no >> word_no;
		if (action == "Access") {
			access_count++;
			operation(page_no, false);
		}
		else if (action == "Modify") {
			modify_count++;
			operation(page_no, true);
		}
		usleep(20);
	}
	return;
}

void process::print_stats() {
	cout << "printing stats for process " << id << endl;
	cout << "Access count	  : " << access_count << endl;
	cout << "Modify count	  : " << modify_count << endl;
	cout << "Page fault count : " << page_fault_count << endl;
}

void process::operation(int page_no, bool type) {
	bool page_exists = page_table.page_exists(page_no);
	if (!page_exists) {
		// call page fault handler
		int frame_no = page_fault_handler(page_no);
		{
			cout << "to the parent " << id << endl;
			lock_guard<mutex> lk(shared_bundle.m);
			cout << "to the parent1 " << id << endl;
			page_table.validate(page_no, frame_no, shared_bundle.timer, type);
			shared_bundle.inverted_page_table.allocate(frame_no, id, page_no, shared_bundle.timer, type);
		}
		
	}
	else {
		int frame_no = page_table.find_frame_no(page_no);
		{
			lock_guard<mutex> lk(shared_bundle.m);
			page_table.update_ref_info(page_no, shared_bundle.timer);
			shared_bundle.inverted_page_table.update_ref_info(frame_no, shared_bundle.timer);
			if (type) {
				page_table.make_dirty(page_no);
				shared_bundle.inverted_page_table.make_dirty(frame_no);
			}
		}
		
	}
	return;
}


int process::page_fault_handler(int page_no) {
	page_fault_count++;
	int frame_no;
	{
		lock_guard<mutex> lk(shared_bundle.m);
		frame_no = shared_bundle.free_list.get_free_frame();
	}

	if (frame_no == -1) {
		cerr << "Free List is empty." << endl;
		return -1;
	}
	{
		lock_guard<mutex> lk(shared_bundle.m);
		shared_bundle.io_table.add_entry(id, page_no, frame_no, true);
		shared_bundle.io_operation_finish_signals[id] = false;
	}

	//shared_bundle.io_operation_finish_signals[id] = false;
	
	// {
	// 	lock_guard<mutex> lk(shared_bundle.m);
    //     shared_bundle.io_manager_signal = 1;
	// }
	
	shared_bundle.cv.notify_all();
	{
		unique_lock<mutex> lk(shared_bundle.m);
		cout << "process got the lock" << endl;
		while (!shared_bundle.io_operation_finish_signals[id]) {
			cout << "waiting " << id << endl;
			shared_bundle.cv.wait(lk);
		}
	}

	{
		lock_guard<mutex> lk(shared_bundle.m);
		if (shared_bundle.free_list.is_critical()) {
			shared_bundle.cv.notify_all();
		}
	}
	
	return frame_no;
}
