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
using namespace std;

struct invertedPageTableEntry {
	int frame_no;
	int pid;
	int page_no;
	int ref_info;
	bool dirty_bit;

	invertedPageTableEntry(int, int, int, int, bool);
};

invertedPageTableEntry::invertedPageTableEntry(int f, int id, int p, int r, bool d) {
	frame_no = f;
	pid = id;
	page_no = p;
	ref_info = r;
	dirty_bit = d;
}


struct invertedPageTableEntryCompare {
	bool operator() (invertedPageTableEntry *lhs, invertedPageTableEntry *rhs) const{
		if (lhs->ref_info == rhs->ref_info) return lhs->frame_no < rhs->frame_no;
		return lhs->ref_info < rhs->ref_info;
    }
};


struct invertedPageTable
{
	map<int, invertedPageTableEntry *> inverted_entries_access;
	set<invertedPageTableEntry *, invertedPageTableEntryCompare> inverted_entries;

	void make_dirty(int);
	void update_ref_info(int, int);
	int free_least_recent(int &, int &);
	void allocate(int, int, int, int, bool);
};


void invertedPageTable::make_dirty(int f) {
	inverted_entries_access[f]->dirty_bit = true;
}

void invertedPageTable::update_ref_info(int frame_no, int time) {
	invertedPageTableEntry * ipte = inverted_entries_access[frame_no];
	inverted_entries.erase(ipte);
	ipte->ref_info = time;
	inverted_entries.insert(ipte);
	return;
}

int invertedPageTable::free_least_recent(int &pid, int &page_no) {
	invertedPageTableEntry * ipte = (*inverted_entries.begin());
	pid = ipte->pid;
	page_no = ipte->page_no;
	int frame_no = ipte->frame_no;
	bool dirty = inverted_entries_access[frame_no]->dirty_bit;
	inverted_entries.erase(inverted_entries.begin());
	inverted_entries_access.erase(frame_no);
	if (dirty) frame_no = 0-frame_no;
	return frame_no;
};

void invertedPageTable::allocate(int frame_no, int pid, int page_no, int time, bool dirty) {
	if (inverted_entries_access.find(frame_no) != inverted_entries_access.end()) {
		invertedPageTableEntry *ipte = inverted_entries_access[frame_no];
		inverted_entries.erase(ipte);
		inverted_entries_access.erase(frame_no);
	}
	invertedPageTableEntry * ipte = new invertedPageTableEntry(frame_no, pid, page_no, time, dirty);
	inverted_entries_access[frame_no] = ipte;
	inverted_entries.insert(ipte);
	return;
}
