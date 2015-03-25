#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <iomanip>
#include <string>

using namespace std;

struct page_table_entry {
   	int logical_address;
	int frame_no;
	bool valid_bit;
	bool dirty_bit;
	int ref_info;
	int protection_info;

	// constructor
	page_table_entry(int, int, int);
	void update_time(int);
};

page_table_entry::page_table_entry(int l, int f, int r) {
	logical_address = l;
	frame_no = f;
	ref_info = r;
}

struct page_table_entry_compare {
	bool operator() (page_table_entry *lhs, page_table_entry *rhs) const{
		return lhs->ref_info < rhs->ref_info;
    }
};

void page_table_entry::update_time(int time) {
	ref_info = time;
}
