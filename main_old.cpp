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
using namespace std;


struct page_table_entry {
	int logical_address;
	int frame_no;
	bool valid_bit;
	bool dirty_bit;
	int ref_info;
	int protection_info;

	page_table_entry(int a) {
		ref_info = a;
	};
};

struct page_table_entry_compare {
    bool operator() (page_table_entry *lhs, page_table_entry *rhs) const{
		return lhs->ref_info < rhs->ref_info;
    }
};

struct page_table {
	map<int, page_table_entry*> access_entry;
	set<page_table_entry*, page_table_entry_compare> entries;

	void replace_least_recent(int page_no) {
		page_table_entry p = *((*entries.begin()));
		access_entry.erase(p.logical_address);
		entries.erase(entries.begin());
		cout << "Removed page "<< p.logical_address << " from frame "<< p.frame_no << endl;
		page_table_entry* new_entry;
		new_entry->logical_address = page_no;
		new_entry->frame_no = p.frame_no;
		access_entry[page_no] = new_entry;
		entries.insert(new_entry);
	}
};

struct process {
	int id;
	int process_size;
	int frame_start, frame_end;
	string spec_file;
	page_table pt;
	int time;

	process(int i, int p, int fs, int fe) {
		id = i;
		process_size = p;
		frame_start = fs;
		frame_end = fe;
		pt = *(new page_table());
		spec_file = "s"+to_string(id);
		time = 0;
	}

	int mmu(int page_no) {
		if (page_no >= process_size) return -1;
		map<int, page_table_entry*>::iterator it = pt.access_entry.find(page_no);
		if (it == pt.access_entry.end()) return -2;
		else {
			pt.access_entry[page_no]->ref_info = time;
			return it->second->frame_no;
		}
	}

	int page_fault_handler(int page_no) {
		if (pt.access_entry.size() == frame_end-frame_start+1) {
			pt.replace_least_recent(page_no);
			pt.access_entry[page_no]->ref_info = time;
		}
		else {
			int max_frame_no= frame_start;
			
			page_table_entry* new_entry;
			new_entry->logical_address = page_no;
			pt.access_entry[page_no] = new_entry;
			pt.entries.insert(new_entry);
		}
	}
	
	void start() {
		ifstream infile;
		infile.open(spec_file.c_str());
		string action;
		infile >> action;
		while (action != "end") {
			time++;
			int page_no, word_no;
			infile >> page_no >> word_no;
			int code = mmu(page_no);
			cout << id << ": attempted to " << action << " "<< page_no << " " << word_no << endl;
			if (code == -1) cout << "Reported a memory protection violation" << endl;
			else if (code == -2) {
				cout << "Reported a page fault" << endl;
				page_fault_handler(page_no);
			}
			infile >> action;
		}
	}
	
};

void start_process(process* p){
	p->start();
};

struct primary_thread {
	int no_process;
	map<int, process*> processes;

	void create_process(int pid, int size, int frame_start, int frame_end){
		process *p = new process(pid, size, frame_start, frame_end);
		processes[pid] = p;
		thread t(start_process,p);
		t.join();
	}

	void start() {
		ifstream infile;
		infile.open("init");
		string action;
		while (infile >> action) {
			int pid, size, frame_start, frame_end;
			char c;
			infile >> pid >> size >> frame_start >> c >> frame_end;
			create_process(pid, size, frame_start, frame_end);
		}
	}
};

int main()
{
	primary_thread p_thread;
	p_thread.start();
    return 0;
}
