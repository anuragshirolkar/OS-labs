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
using namespace std;

struct pageTableEntry
{
   	int page_no;
	int frame_no;
	bool valid_bit;
	bool dirty_bit;
	int ref_info;
	int protection_info;

	pageTableEntry(int, int, bool, bool, int);
	void print();
    
};

pageTableEntry::pageTableEntry(int p, int f, bool v, bool d, int r) {
	page_no = p;
	frame_no = f;
	valid_bit = v;
	dirty_bit = d;
	ref_info = r;
}


void pageTableEntry::print() {
	cout << page_no << " " << frame_no << " " << valid_bit << " " << dirty_bit << " " << ref_info << endl;
}
