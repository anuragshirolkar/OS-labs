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
	
	freeList(){};
	freeList(int, int, int);
	void add_entry(int);
	int get_free_frame();
};

freeList::freeList(int memory_size, int l, int u) {
	// constructor
	lower_threshold = l;
	upper_threshold = u;
	for (int i = 0; i < memory_size; i++) {
		freeListEntry fle(i);
		list.insert(fle);
	}
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
