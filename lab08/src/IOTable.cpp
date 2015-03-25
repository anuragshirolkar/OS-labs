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
	void add_entry();
};

IOTable::IOTable() {
	// constructor
}

void IOTable::add_entry() {
	// add a new entry to the table_queue
	return;
}
