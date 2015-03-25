#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "event_manager.h"
#include <queue>
#include <map>
#include <algorithm>

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

	void change_phase(int);
	bool reduce_phase();
};

void process::change_phase(int completed) {
	if (phases[0].itrs == 1) phases[0].cpu_b-=completed;
	process_phase p = phases[0];
	phases[0].itrs--;
	p.itrs = 1;
	p.cpu_b -= completed;
	phases.insert(phases.begin(), p);
}

bool process::reduce_phase() {
	if (phases.size() == 1 && phases[0].itrs == 1) {
		phases.clear();
		return true;
	}
	if (phases[0].itrs == 1) {
		phases.erase(phases.begin());
		return false;
	}
	phases[0].itrs--;
	return false;
}

struct scheduling_level {
	int level_no;
	int priority;
	int time_slice;	//either some integer or N
	queue<int> ready_queue;

	bool is_empty();
};

bool operator<(const scheduling_level l1, const scheduling_level l2) {
	return l1.priority > l2.priority;
}

bool scheduling_level::is_empty() {
	return ready_queue.empty();
}

struct scheduler {
	int no_of_levels;
	vector<scheduling_level> levels_arr;
	map<int, int> priority_to_index;
	int current_pid, current_start_time;
	bool cpu_busy;

	scheduler();
	void add_pid_to_ready_queue(process*);
	void promote(process*);
	void demote(process*);
	void print();
	bool should_preempt(process*);
	int find_time_slice(process *);
	int next_process();
};

scheduler::scheduler() {
	cpu_busy = false;
}

void scheduler::add_pid_to_ready_queue(process *p) {
	levels_arr[priority_to_index[p->start_priority]].ready_queue.push(p->pid);
}

void scheduler::promote(process *p) {
	int index = priority_to_index[p->start_priority];
	if (index == 0) return;
	p->start_priority = levels_arr[index-1].priority;
	return;
}

void scheduler::demote(process *p) {
	int index = priority_to_index[p->start_priority];
	if (index == levels_arr.size()-1) return;
	p->start_priority = levels_arr[index+1].priority;
	return;
}

void scheduler::print() {
	cout << "no levels: " << no_of_levels << endl;
	for (int i = 0; i < levels_arr.size(); i++) {
		cout << "printing level: " << levels_arr[i].level_no << " " << levels_arr[i].priority << " " << levels_arr[i].time_slice << endl;
	}
	for (map<int, int> :: iterator it = priority_to_index.begin(); it != priority_to_index.end(); it++)
		cout << it->first << " "<< it->second << endl;
}

bool scheduler::should_preempt(process *p) {
	return (p->phases[0].cpu_b > levels_arr[priority_to_index[p->start_priority]].time_slice);
}

int scheduler::find_time_slice(process *p) {
	return levels_arr[priority_to_index[p->start_priority]].time_slice;
}

int scheduler::next_process() {
	int ans = -1;
	for (int i = 0; i < levels_arr.size(); i++) {
		if (!levels_arr[i].is_empty()) {
			ans = levels_arr[i].ready_queue.front();
			levels_arr[i].ready_queue.pop();
			return ans;
		}
	}
	return ans;
}

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
			em.add_event(proc.admission,PA,proc.pid);	//event type "1" represents "process admission event"
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
	sort(Scheduler.levels_arr.begin(), Scheduler.levels_arr.end());
	for (int i = 0; i < Scheduler.levels_arr.size(); i++) {
		Scheduler.priority_to_index[Scheduler.levels_arr[i].priority] = i;
	}
}

int main()
{
	
	handling_PROCESS_SPEC_file();
	handling_SCHEDULER_SPEC_file();
	//processing events
	event next;
	while (!em.is_empty()) {
		next = em.next_event();
		process * current = &p_list[next.pid-1];
		switch(next.type)
		{	
			//routine for handling process admission event
		case 1:
			{
				cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: Process Admitted" << endl;
				//Define routines for other required events here.
				if (!Scheduler.cpu_busy) {
					Scheduler.current_pid = next.pid;
					Scheduler.current_start_time = next.end_t;
					if (Scheduler.should_preempt(current)){
						em.add_event(next.end_t + Scheduler.find_time_slice(current), TC, next.pid);
					}
					else {
						em.add_event(next.end_t + current->phases[0].cpu_b, CPUC, next.pid);
					}
					cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
				}
				else if (current->start_priority > p_list[Scheduler.current_pid-1].start_priority) {
					p_list[Scheduler.current_pid-1].change_phase(next.end_t - Scheduler.current_start_time);
					Scheduler.add_pid_to_ready_queue(&p_list[Scheduler.current_pid-1]);
					Scheduler.current_pid = next.pid;
					Scheduler.current_start_time = next.end_t;
					if (Scheduler.should_preempt(current)){
						em.add_event(next.end_t + Scheduler.find_time_slice(current), TC, next.pid);
					}
					else {
						em.add_event(next.end_t + current->phases[0].cpu_b, CPUC, next.pid);
					}
					cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
				}
				else {
					Scheduler.add_pid_to_ready_queue(current);
				}
				break;
			}
			
		case 2 :
			{
				cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: CPU burst completed" << endl;
				cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: IO started" << endl;
				// if next event is completion of cpu burst
				if (Scheduler.current_pid != next.pid) break;
				Scheduler.promote(current);
				em.add_event(next.end_t + current->phases[0].io_b, IOC, next.pid);
				int next_pid = Scheduler.next_process();
				if (next_pid == -1) {
					Scheduler.cpu_busy = false;
				}
				else {
					current = &p_list[next_pid-1];
					Scheduler.current_pid = next.pid;
					Scheduler.current_start_time = next.end_t;
					if (Scheduler.should_preempt(current)){
						em.add_event(next.end_t + Scheduler.find_time_slice(current), TC, next.pid);
					}
					else {
						em.add_event(next.end_t + current->phases[0].cpu_b, CPUC, next.pid);
					}
					cout << "PID :: " << current->pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
				}
				break;
			}
		case 3 :
			{
				// if next event is completion of IO burst
				cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: IO burst completed" << endl;
				bool is_completed = current->reduce_phase();
				if (is_completed) {
					// process completed
					cout << "PID :: " << next.pid << " TIME :: " << next.end_t << " EVENT :: Process completed" << endl;
				}
				else {
					if (!Scheduler.cpu_busy) {
						Scheduler.current_pid = next.pid;
						Scheduler.current_start_time = next.end_t;
						if (Scheduler.should_preempt(current)){
							em.add_event(next.end_t + Scheduler.find_time_slice(current), TC, next.pid);
						}
						else {
							em.add_event(next.end_t + current->phases[0].cpu_b, CPUC, next.pid);
						}
						cout << "PID :: " << current->pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
					}
					else if (current->start_priority > p_list[Scheduler.current_pid-1].start_priority) {
						p_list[Scheduler.current_pid-1].change_phase(next.end_t - Scheduler.current_start_time);
						Scheduler.add_pid_to_ready_queue(&p_list[Scheduler.current_pid-1]);
						Scheduler.current_pid = next.pid;
						Scheduler.current_start_time = next.end_t;
						if (Scheduler.should_preempt(current)){
							em.add_event(next.end_t + Scheduler.find_time_slice(current), TC, next.pid);
						}
						else {
							em.add_event(next.end_t + current->phases[0].cpu_b, CPUC, next.pid);
						}
						cout << "PID :: " << current->pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
					}
					else {
						Scheduler.add_pid_to_ready_queue(current);
					}
				}
				break;
			}
		case 4 :
			{
				// time slice completed
				cout << "PID :: " << current->pid << " TIME :: " << next.end_t << " EVENT :: Time slice completed" << endl;
				if (Scheduler.current_pid != next.pid) break;
				Scheduler.demote(current);
				p_list[Scheduler.current_pid-1].change_phase(next.end_t - Scheduler.current_start_time);
				Scheduler.add_pid_to_ready_queue(&p_list[Scheduler.current_pid-1]);
				int next_pid = Scheduler.next_process();
				if (next_pid == -1) {
					Scheduler.cpu_busy = false;
				}
				else {
					current = &p_list[next_pid-1];
					Scheduler.current_pid = next.pid;
					Scheduler.current_start_time = next.end_t;
					if (Scheduler.should_preempt(current)){
						em.add_event(next.end_t + Scheduler.find_time_slice(current), TC, next.pid);
					}
					else {
						em.add_event(next.end_t + current->phases[0].cpu_b, CPUC, next.pid);
					}
					cout << "PID :: " << current->pid << " TIME :: " << next.end_t << " EVENT :: Process dispatched" << endl;
				}
				break;
			}
		}
	}
	return 0;
}
