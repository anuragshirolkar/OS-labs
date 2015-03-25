#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <iomanip>
#include <string>

#include "pageTableEntry.cpp"
using namespace std;

struct page_table {
	map<int, page_table_entry*> access_entry;
	set<page_table_entry*, page_table_entry_compare> entries;
	int frame_start;
	int table_size;

	// constructor
	page_table(){};
	page_table(int, int);

	bool page_exists(int);
	int find_frame_no(int);
	int size();
	bool is_full();
	int replace_least_recent(int, int);
	int add_page(int, int);
	void update_time(int, int);
};

page_table::page_table(int fs, int s) {
	frame_start = fs;
	table_size = s;
}

bool page_table::page_exists(int page_no) {
	map<int, page_table_entry*>::iterator it = access_entry.find(page_no);
	return it != access_entry.end();
}

int page_table::find_frame_no(int page_no) {
	return access_entry[page_no]->frame_no;
}

int page_table::size() {
	return access_entry.size();
}

bool page_table::is_full() {
	return table_size == size();
}

int page_table::replace_least_recent(int page_no, int time) {
	page_table_entry *pte = (*entries.begin());
	access_entry.erase(pte->logical_address);
	entries.erase(entries.begin());
	pte->ref_info = time;
	pte->logical_address = page_no;
	entries.insert(pte);
	access_entry[page_no] = pte;
	return pte->frame_no;
}

void page_table::update_time(int page_no, int time) {
	access_entry[page_no]->update_time(time);
	return;
}

int page_table::add_page(int page_no, int time) {
	int least_frame_available = frame_start + size();
	page_table_entry *pte = new page_table_entry(page_no, least_frame_available, time);
	access_entry  [page_no] = pte;
	entries.insert(pte);
	return pte->frame_no;
}
