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
#include "sharedMemoryBundle.cpp"
#include "primaryThread.cpp"
#include "freeFramesManager.cpp"
#include "pageIOManager.cpp"
using namespace std;

sharedMemoryBundle shared_bundle;
primaryThread primary_thread;
pageIOManager page_io_manager;
freeFramesManager free_frames_manager;

int main()
{
	thread primary_thread_thread(&primaryThread::start, &primary_thread);
	thread page_io_manager_thread(&pageIOManager::start, &page_io_manager);
	thread free_frames_manager_thread(&freeFramesManager::start, &free_frames_manager);
	primary_thread_thread.join();
	page_io_manager_thread.join();
	free_frames_manager_thread.join();
	cout << shared_bundle.timer << endl;
    return 0;
}
