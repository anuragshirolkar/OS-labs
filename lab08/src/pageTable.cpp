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
#include "pageTableEntry.cpp"
using namespace std;

struct pageTable
{
	map<int, pageTableEntry *> access_entry;

    
	// constructor
	pageTable();

	bool page_exists(int);
	int find_frame_no(int);
	void validate(int, int, int, bool);
	int make_dirty(int);
	int invalidate(int);
	int update_ref_info(int, int);
};

pageTable::pageTable() {
	// constructor
}

bool pageTable::page_exists(int p) {
	map<int, pageTableEntry *>::iterator it = access_entry.find(p);
	if (it == access_entry.end()) return false;
	else return it->second->valid_bit;
} 

int pageTable::find_frame_no(int p) {
	if (!page_exists(p)) return -1;
	return access_entry.find(p)->second->frame_no;
}

void pageTable::validate(int p, int f, int timer, bool dirty) {
	if (page_exists(p)) {
		pageTableEntry *pte = access_entry[p];
		pte->frame_no = f;
		pte->ref_info = timer;
		pte->dirty_bit = dirty;
		pte->valid_bit = true;
	}
	else {
		pageTableEntry *pte = new pageTableEntry(p, f, true, dirty, timer);
		access_entry[p] = pte;
	}
	return;
}

int pageTable::make_dirty(int p) {
	if (!page_exists(p)) return -1;
	access_entry[p]->dirty_bit = true;
	return 0;
}

int pageTable::invalidate(int p) {
	if (!page_exists(p)) return -1;
	access_entry[p]->valid_bit = false;
	return 0;
}

int pageTable::update_ref_info(int p, int t) {
	if (!page_exists(p)) return -1;
	access_entry[p]->ref_info = t;
	return 0;
}
