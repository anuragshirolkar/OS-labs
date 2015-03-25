#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "event_manager.h"

using namespace std;

event_mgnr em;


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

	bool change_process_phase();
};

bool process::change_process_phase() {
	if (phases.empty()) return false;
	if (phases[0].itrs == 1) phases.erase(phases.begin());
	else {
		phases[0].itrs--;
	}
	return phases.empty();
}

class comp_process{
public:
 	int operator() ( const process& p1, const process &p2)
 	{
 		return p1.start_priority<p2.start_priority;
 	}
};


struct scheduling_level {
	int level_no;
	int priority;
	int time_slice;	//either some integer or N
	priority_queue<process, vector<process>, comp_process> ready_queue;
};

struct scheduler {
	int no_of_levels;
	vector<scheduling_level> levels_arr;
};

scheduler Scheduler;
vector<process> p_list;

void handling_PROCESS_SPEC_file(){
	string line, line2;
	int pid, prior;
	int adm;
	int iter;
	int cpu_t, io_t;
	ifstream infile("PROCESS_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="PROCESS"){
			process proc;
			getline(infile, line2);
			std::istringstream iss(line2);
		        if (!(iss >> pid >> prior >> adm)) { break; } // error
		    
			proc.pid = pid;
			proc.start_priority = prior;
			proc.admission = adm;

			getline(infile, line2);
			while(line2 != "END"){
				std::istringstream iss(line2);
				process_phase pp;
			        if (!(iss >> iter >> cpu_t >> io_t)) { break; } // error
			    
				pp.itrs = iter;
			    	pp.cpu_b = cpu_t;
			    	pp.io_b = io_t;
			    	(proc.phases).push_back(pp);
			    	getline(infile, line2);
			}
			p_list.push_back(proc);
			em.add_event(proc.admission,PA,proc.pid, proc.start_priority);	//event type "1" represents "process admission event"

		}
	}
}

int string_to_integer(string str)
{
	int r=1,s=0,l=str.length(),i;
	for(i=l-1;i>=0;i--)
	{
		s = s + ((str[i] - '0')*r);
		r *= 10;
	}
	return s;
}

void handling_SCHEDULER_SPEC_file(){
	string line, line2;
	int level_count;
	int prior;
	int s_lvl;
	int t_slice;
	string t_slice1;
	ifstream infile("SCHEDULER_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="SCHEDULER"){
			getline(infile, line2);
			std::istringstream iss(line2);
		    if (!(iss >> level_count)) { break; } // error
			
			Scheduler.no_of_levels = level_count;
			for(int i=0; i<level_count; i++){
				getline(infile, line2);
				std::istringstream iss(line2);
				if (!(iss >> s_lvl >> prior >> t_slice1)) { break; } // error
				scheduling_level scl;
				if(t_slice1 == "N")
					t_slice = 9999;
				else
					t_slice = string_to_integer(t_slice1);
				scl.level_no = s_lvl;
				scl.priority = prior;
				scl.time_slice = t_slice;
				
				Scheduler.levels_arr.push_back(scl);
			}
		}
	}
}

int main()
{
	
	handling_PROCESS_SPEC_file();
	handling_SCHEDULER_SPEC_file();
	//processing events
	event next;
	bool cpu_busy = false;
	while(!em.is_empty()) {
		next = em.next_event();
		//next.print();
		process *current = &p_list[next.pid-1];
		switch(next.type)
		{	
			//routine for handling process admission event
		case 1:
			// admission events
			cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: Process Admitted" << endl;
			if (!cpu_busy) {
				// process dispatched
				cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
				em.add_event(next.end_t + current->phases[0].cpu_b, CPUC, next.pid, current->start_priority);
				//cout << "adding event: " << next.end_t + current.phases[0].cpu_b << endl;
				cpu_busy = true;
			}
			else Scheduler.levels_arr[0].ready_queue.push(p_list[next.pid-1]);
			break;
		case 2 :
			// cpuburst completed
			em.add_event(next.end_t + current->phases[0].io_b, IOC, next.pid, current->start_priority);
			cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: CPU burst completed" << endl;
			cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: IO started" << endl;
			cpu_busy = false;
			if (!Scheduler.levels_arr[0].ready_queue.empty()) {
				//dispatch process
				process to_dispatch = Scheduler.levels_arr[0].ready_queue.top();
				cout << "PID :: " << to_dispatch.pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
				em.add_event(next.end_t + to_dispatch.phases[0].cpu_b, CPUC, to_dispatch.pid, to_dispatch.start_priority);
				Scheduler.levels_arr[0].ready_queue.pop();
				cpu_busy = true;
			}
			break;
		case 3 :
			// io completed
			bool p_completed = current->change_process_phase();
			cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: IO completed" << endl;
			if (!cpu_busy && !p_completed) {
				// process dispatched
				cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
				em.add_event(next.end_t + current->phases[0].cpu_b, CPUC, next.pid, current->start_priority);
				cpu_busy = true;
			}
			else if (!p_completed) Scheduler.levels_arr[0].ready_queue.push(*current);
			else if (p_completed) cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: Process completed" << endl;
			break;
		}
	}
	
	return 0;
}
