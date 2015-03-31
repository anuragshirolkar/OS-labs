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

struct IOTableEntry
{
	int pid;
	int page_no;
	int frame_no;
	bool type;  // page_in : true , page_out : false

	IOTableEntry(int, int, int, bool);
};

IOTableEntry::IOTableEntry(int p, int pn, int f, bool t) {
	pid = p;
	page_no = pn;
	frame_no = f;
	type = t;
}
