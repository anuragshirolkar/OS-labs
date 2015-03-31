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
#include <queue>
#include "IOTableEntry.cpp"
using namespace std;

struct IOTable
{
	queue<IOTableEntry> table_queue;

	IOTable();
	void add_entry(int, int, int, bool);
	int get_next();
	int size();
};

IOTable::IOTable() {
	// constructor
}

void IOTable::add_entry(int pid, int page_no, int frame_no, bool type) {
	// add a new entry to the table_queue
	IOTableEntry iote(pid, page_no, frame_no, type);
	table_queue.push(iote);
	return;
}

int IOTable::get_next() {
	IOTableEntry iote = table_queue.front();
	table_queue.pop();
	if (iote.type) return iote.pid;
	return -1;
}

int IOTable::size() {
	return table_queue.size();
}
