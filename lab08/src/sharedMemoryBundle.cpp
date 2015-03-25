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
#include "IOTable.cpp"
#include "freeList.cpp"
using namespace std;

struct sharedMemoryBundle
{
    IOTable io_table;
	freeList free_list;
	int lower_threshold, upper_threshold;
	int memory_size;
	int timer;

	sharedMemoryBundle();
	void print();
};

sharedMemoryBundle::sharedMemoryBundle() {
	timer = 0;
}

void sharedMemoryBundle::print() {
	cout << "checking sharedMemoryBundle" << endl;
}
