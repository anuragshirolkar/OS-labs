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
extern primaryThread primary_thread;

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
    unique_lock<mutex> lk(shared_bundle.m);
	while (true) {
		while (shared_bundle.free_frames_manager_signal==0 && !shared_bundle.free_list.is_critical()) {
			cout << "free manager is waiting" << endl;
			shared_bundle.cv.wait(lk);
		}
		cout << "free lock requested " << shared_bundle.free_frames_manager_signal << endl;
		//lock_guard<mutex> lk(shared_bundle.m);
		cout << "free lock acquired" << endl;
		if (shared_bundle.free_list.is_critical()) {
			cout << "Free Frames Manager is activated" << endl;
			int pid, page_no;
			int frame_no = shared_bundle.inverted_page_table.free_least_recent(pid, page_no);
			if (frame_no < 0) {
				frame_no = 0-frame_no;
				shared_bundle.io_table.add_entry(-1, -1, frame_no, false);
				//shared_bundle.io_manager_signal = 1;
			}
			shared_bundle.free_list.add_entry(frame_no);
			shared_bundle.cv.notify_all();
			primary_thread.processes[pid]->page_table.invalidate(page_no);
			shared_bundle.free_frames_manager_signal =0;
		}
		if (shared_bundle.free_frames_manager_signal == -1) return;
		if (shared_bundle.free_frames_manager_signal > 0) {
			cout << "Free Frames Manager has added page " << shared_bundle.free_frames_manager_signal - 10 << " to free list" << endl;
			shared_bundle.free_list.add_entry(shared_bundle.free_frames_manager_signal - 10);
			shared_bundle.free_frames_manager_signal =0;
		}
	}
	return;
}
