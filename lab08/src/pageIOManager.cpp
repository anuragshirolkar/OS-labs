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

struct pageIOManager
{
	pageIOManager();
	void start();
	
};

pageIOManager::pageIOManager() {
	// constructor
}

void pageIOManager::start() {
	// start pageIOManager
	return;
}
