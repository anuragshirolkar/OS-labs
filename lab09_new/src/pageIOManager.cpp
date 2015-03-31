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
    unique_lock<mutex> lk(shared_bundle.m);
	while (true) {
		while (shared_bundle.io_manager_signal == 0 && shared_bundle.io_table.size() == 0) {
			cout << "io waiting" << endl;
			shared_bundle.cv.wait(lk);
		}
		{
			if (shared_bundle.io_manager_signal == -1) {
				return;
			}
			else if(shared_bundle.io_table.size()) {
				int pid = shared_bundle.io_table.get_next();
				if (pid == -1) {
					shared_bundle.page_out_operation_finish_signal = true;
					shared_bundle.cv.notify_all();
				}
				else {
					shared_bundle.io_operation_finish_signals[pid] = true;
					shared_bundle.cv.notify_all();
				}
				//if (shared_bundle.io_table.size() == 0) shared_bundle.io_manager_signal = 0;
			}
		}
		
	}
	return;
}


// void pageIOManager::start() {
// 	// start pageIOManager
//     unique_lock<mutex> lk(shared_bundle.m);
// 	while (true) {
// 		while (shared_bundle.io_manager_signal == 0) {
// 			shared_bundle.cv.wait(lk);
// 		}
// 		if (shared_bundle.io_manager_signal == -1) {
// 			return;
// 		}
// 		else if(shared_bundle.io_manager_signal == 1) {
// 			int pid = shared_bundle.io_table.get_next();
// 			//usleep(200);
// 			if (pid == -1) {
// 				shared_bundle.page_out_operation_finish_signal = true;
// 				shared_bundle.cv.notify_all();
// 			}
// 			else {
// 				shared_bundle.io_operation_finish_signals[pid] = true;
// 				shared_bundle.cv.notify_all();
// 			}
// 			if (shared_bundle.io_table.size() == 0) shared_bundle.io_manager_signal = 0;
// 		}
// 	}
// 	return;
// }
