#include <iostream>
#include <vector>
#include <queue>
#include "process.cpp"
using namespace std;


struct scheduling_level {
	int level_no;
	int priority;
	int time_slice;	//either some integer or N
};

struct scheduler {
	int no_of_levels;
	vector<scheduling_level> levels_arr;
	priority_queue<process*, vector<process*>, comp_process> ready_queue;
};
