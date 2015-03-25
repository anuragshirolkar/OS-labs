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

extern sharedMemoryBundle shared_bundle;

struct freeFramesManager
{

	freeFramesManager();
	void start();
};


freeFramesManager::freeFramesManager() {
	// constructor
}

void freeFramesManager::start() {
	// start freeFramesManager
	return;
}
