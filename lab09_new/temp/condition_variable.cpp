#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <condition_variable>
using namespace std;
 
std::mutex m;
std::condition_variable cv;
std::string data;
bool ready = false;
bool processed = false;
 
// void worker_thread()
// {
//     // Wait until main() sends data
//     std::unique_lock<std::mutex> lk(m);
//     //cv.wait(lk, []{return ready;});
// 	while (!ready) {
// 		cout << "waiting" << endl;
// 		cv.wait(lk);
// 	}
 
//     // after the wait, we own the lock.
//     std::cout << "Worker thread is processing data\n";
//     data += " after processing";
 
//     // Send data back to main()
//     processed = true;
//     std::cout << "Worker thread signals data processing completed\n";
 
//     // Manual unlocking is done before notifying, to avoid waking up
//     // the waiting thread only to block again (see notify_one for details)
//     lk.unlock();
//     cv.notify_one();
// }
 
// int main()
// {
//     std::thread worker(worker_thread);
 
//     data = "Example data";
//     // send data to the worker thread
//     {
//         std::lock_guard<std::mutex> lk(m);
//         ready = true;
//         std::cout << "main() signals data ready for processing\n";
//     }
//     cv.notify_one();
 
//     // wait for the worker
//     {
//         std::unique_lock<std::mutex> lk(m);
//         cv.wait(lk, []{return processed;});
//     }
//     std::cout << "Back in main(), data = " << data << '\n';
 
//     worker.join();
// }

int ready1 = 3;

bool is_critical() {
	return ready1*ready1 == 16;
}

void worker_thread() {
	std::unique_lock<std::mutex> lk(m);
	while(!is_critical()) {
		cout << "waiting" << endl;
		cv.wait(lk);
	}
	cout << "wait completed" << endl;
}

void worker_thread1() {
	std::unique_lock<std::mutex> lk(m);
	while(ready1 != 5) {
		cout << "waiting1" << endl;
		cv.wait(lk);
	}
	cout << "wait1 completed" << endl;
}

int main()
{

	std::thread worker(worker_thread);
	std::thread worker1(worker_thread1);
	usleep(1000000);
	cout << "main wait completed" << endl;
	ready1 = 4;
	cv.notify_all();
	usleep(1000000);
	ready1 = 5;
	cv.notify_all();
    worker.join();
	worker1.join();
    return 0;
}
