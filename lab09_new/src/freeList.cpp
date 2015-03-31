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
#include "freeListEntry.cpp"
using namespace std;


struct freeList
{
	set<freeListEntry> list;
	int lower_threshold, upper_threshold;
	int memory_size;
	
	freeList();
	void initialize(int);
	void update(int, int, int);
	void add_entry(int);
	int get_free_frame();
	bool is_critical();
};

freeList::freeList() {
	memory_size = -1;
}

void freeList::initialize(int memory_size) {
	if (this->memory_size != -1) {
		this->memory_size = memory_size;
		return;
	}
	this->memory_size = memory_size;
	for (int i = 0; i < memory_size; i++) {
		freeListEntry fle(i);
		list.insert(fle);
	}
}

void freeList::update(int ms, int l, int u) {
	memory_size = ms;
	lower_threshold = l;
	upper_threshold = u;
	return;
}

void freeList::add_entry(int frame_no) {
	freeListEntry fle(frame_no);
	list.insert(fle);
	// add a new entry
	return;
}

int freeList::get_free_frame() {
	if(list.empty()) return -1;
	int frame_no = list.begin()->frame_no;
	list.erase(list.begin());
	return frame_no;
}

bool freeList::is_critical() {
	cout << list.size() << " " << lower_threshold << endl;
	return list.size() < lower_threshold;
}
