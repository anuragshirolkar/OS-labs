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

struct freeListEntry
{
    int frame_no;

	freeListEntry(){};
	freeListEntry(int);
	void print();
};

freeListEntry::freeListEntry(int f) {
	frame_no = f;
}


bool operator<(const freeListEntry a, const freeListEntry b) {
	return a.frame_no < b.frame_no;
}

void freeListEntry::print() {
	
}
