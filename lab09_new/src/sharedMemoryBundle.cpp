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
#include "IOTable.cpp"
#include "freeList.cpp"
#include "invertedPageTable.cpp"
#include "disk.cpp"
using namespace std;

struct sharedMemoryBundle
{
    IOTable io_table;
	freeList free_list;
	int lower_threshold, upper_threshold;
	int memory_size;
	int timer;
	invertedPageTable inverted_page_table;

	// mutex and condition variable
	mutex m;
	condition_variable cv;

	// signal variables
	int io_manager_signal;
	map<int, bool> io_operation_finish_signals;
	bool page_out_operation_finish_signal;
	int free_frames_manager_signal;

	// lab 09
	disk dsk;
	map<string, int> curdirs;
	
	sharedMemoryBundle();
	void initialize_free_list();
	void update_free_list();
	void setup_new_process(int);
	void print();
};

sharedMemoryBundle::sharedMemoryBundle() {
	timer = 0;
	free_frames_manager_signal = 0;
}

void sharedMemoryBundle::initialize_free_list() {
	free_list.initialize(memory_size);
	return;
}

void sharedMemoryBundle::update_free_list() {
	free_list.update(memory_size, lower_threshold, upper_threshold);
}

void sharedMemoryBundle::setup_new_process(int pid) {
	io_operation_finish_signals[pid] = true;
}

void sharedMemoryBundle::print() {
	cout << "checking sharedMemoryBundle" << endl;
}
