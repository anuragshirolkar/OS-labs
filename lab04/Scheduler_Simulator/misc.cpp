#include <sstream>
#include <fstream>
#include <string>
#include "scheduler.cpp"

void handling_PROCESS_SPEC_file(vector<process> &p_list, event_mgnr &em){
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
			em.add_event(proc.admission,1,proc.pid);	//event type "1" represents "process admission event"

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