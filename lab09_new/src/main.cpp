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
#include <mutex>
#include <condition_variable>
#include "sharedMemoryBundle.cpp"
#include "primaryThread.cpp"
#include "freeFramesManager.cpp"
#include "pageIOManager.cpp"
using namespace std;

sharedMemoryBundle shared_bundle;
primaryThread primary_thread;
pageIOManager page_io_manager;
freeFramesManager free_frames_manager;

void stop_io_manager() {
	
	{
		lock_guard<mutex> lk(shared_bundle.m);
        shared_bundle.io_manager_signal = -1;
	}
	shared_bundle.cv.notify_all();
}

void stop_free_frames_manager() {
	
	{
		lock_guard<mutex> lk(shared_bundle.m);
		cout << "setting free" << endl;
        shared_bundle.free_frames_manager_signal = -1;
	}
	shared_bundle.cv.notify_all();
}

int main()
{
	shared_bundle.dsk.initialize();
	thread primary_thread_thread(&primaryThread::start, &primary_thread);
	//thread page_io_manager_thread(&pageIOManager::start, &page_io_manager);
	//thread free_frames_manager_thread(&freeFramesManager::start, &free_frames_manager);
	primary_thread_thread.join();
	//stop_io_manager();
	//page_io_manager_thread.join();
	//stop_free_frames_manager();
	//free_frames_manager_thread.join();
    return 0;
}
