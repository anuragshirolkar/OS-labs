#include <iostream>
#include <vector>
using namespace std;


struct process_phase {
	int itrs;	//number of iterations
	int cpu_b;	//cpu burst time
	int io_b;	//IO burst time
};

struct process {
	int pid;
	int start_priority;
	int admission;
	vector<process_phase> phases;

	int start_ready_state;
};


class comp_process{
public:
 	int operator() ( const process* p1, const process* p2)
 	{
 		if (p1->start_priority == p2->start_priority) return p1->start_ready_state > p2->start_ready_state;
		return p1->start_priority < p2->start_priority;
 	}
};
