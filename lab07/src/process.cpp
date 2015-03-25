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

#include "pageTable.cpp"
using namespace std;

#define SPEC_FILE_BASE "s"

struct process {
	int id;
	int process_size;
	int frame_start, frame_end;
	string spec_file;
	page_table pt;
	int time;

	// constructor
	process(int, int, int, int);

	void start();
	int mmu(int);
	int page_fault_handler(int);
};

process::process(int i, int p, int fs, int fe) {
	id = i;
	process_size = p;
	frame_start = fs;
	frame_end = fe;
	pt = *(new page_table(frame_start, frame_end-frame_start+1));
	time = 0;
	spec_file = SPEC_FILE_BASE + to_string(id);
	return;
}

void process::start() {
	ifstream specfile;
	specfile.open(spec_file.c_str());
	string action;
	specfile >> action;
	while (action != "end") {
		time++;
		int page_no, word_no;
		specfile >> page_no >> word_no;
		int status = mmu(page_no);
		cout << id << ": attempted to " << action << " "<< page_no << " " << word_no << endl;
		if (status == -1) cout << "Reported memory protection violation" << endl;
		else if (status == -2) {
			cout << "Reported a page fault" << endl;
			status = page_fault_handler(page_no);
			cout << "Accessed page frame number " << status << endl;
		}
		else {
			cout << "Accessed page frame number " << status << endl;
 		}
		specfile >> action;
	}
	return;
}

int process::mmu(int page_no) {
	if (page_no >= process_size) return -1;
	if (! pt.page_exists(page_no)) return -2;
	pt.update_time(page_no, time);
	return pt.find_frame_no(page_no);
}

int process::page_fault_handler(int page_no) {
	int frame_no;
	if (pt.is_full()) {
		int removed_page = pt.replace_least_recent(page_no, time);
		frame_no = pt.find_frame_no(page_no);
		cout << "Removed page " << removed_page << " from frame " << frame_no << endl;
	}
	else frame_no = pt.add_page(page_no, time);
	cout << "Loaded page " << page_no << " into frame " << frame_no << endl;
	return frame_no;
}
