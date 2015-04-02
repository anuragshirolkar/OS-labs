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

	// lab9
	string filename;
	int current_byte, first_block, file_dir_block;
	string uname;

	process(int, int, string);
	void start();
	void print_stats();
	void operation(int, bool);
	int page_fault_handler(int);
	void open_file(string, int);
	void seek(int);
	void seek_relative(int);
	void read_file(int);
	void write_file(int, string);
};

process::process(int pid, int psize, string u) {
	// constructor
	uname = u;
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
			//print_stats();
			break;
		}
		int page_no, word_no;
		if (action == "Access") {
			specfile >> page_no >> word_no;
			access_count++;
			operation(page_no, false);
		}
		else if (action == "Modify") {
			specfile >> page_no >> word_no;
			modify_count++;
			operation(page_no, true);
		}
		else if (action == "open") {
			string filename;
			specfile >> filename;
			shared_bundle.m.lock();
			int current_ulocn = shared_bundle.curdirs[uname];
			shared_bundle.m.unlock();
			open_file(filename, current_ulocn);
		}
		else if (action == "seek") {
			int offset;
			specfile >> offset;
			seek(offset);
		}
		else if (action == "seek_relative") {
			int offset;
			specfile >> offset;
			seek_relative(offset);			
		}
		else if (action == "read") {
			string filename;
			int nbytes;
			specfile >> filename >> nbytes;
			read_file(nbytes);
		}
		else if (action == "write") {
			string filename, value;
			int nbytes;
			specfile >> filename >> nbytes >> value;
			write_file(nbytes, value);
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

void process::open_file(string filename, int dirblock) {
	this->filename = filename;
	file_dir_block = dirblock;
	current_byte = 0;
	shared_bundle.m.lock();
	first_block = shared_bundle.dsk.open(dirblock, filename.c_str());
	shared_bundle.m.unlock();
	return;
}

void process::seek(int s){
	cout << "Process " << id << " performing seek on file " << filename << endl; 
	current_byte = s;
	shared_bundle.m.lock();
	shared_bundle.dsk.seek(first_block, current_byte);
	shared_bundle.m.unlock();
	return;
}

void process::seek_relative(int s) {
	cout << "Process " << id << " performing seek_relative on file " << filename << endl; 
	current_byte += s;
	shared_bundle.m.lock();
	shared_bundle.dsk.seek(first_block, current_byte);
	shared_bundle.m.unlock();
	return;
}

void process::read_file(int nbytes) {
	cout << "Process " << id << " performing read on file " << filename << endl; 
	char * value = new char[nbytes];
	shared_bundle.m.lock();
	int response = shared_bundle.dsk.read(first_block, current_byte, nbytes, value);
	if (response == 0) {
		cout << "Data Read by process " << id << " from file " << filename << ":" << endl;
		fwrite(value, 1, nbytes, stdout);
		cout << endl;
	}
	shared_bundle.m.unlock();
	return;
}

void process::write_file(int nbytes, string value) {
	cout << "Process " << id << " performing write on file " << filename << endl; 
	shared_bundle.m.lock();
	int response = shared_bundle.dsk.write(first_block, current_byte, nbytes, value.c_str());
	shared_bundle.m.unlock();
	return;
}

