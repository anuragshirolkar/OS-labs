#include <iostream>
#include <vector>
#include <queue>


using namespace std;

enum event_type {
	PA = 1, //Process Admitted
	CPUC = 2, // IO Completed
	IOC = 3 // CPU burst completed
};

struct event{
	int end_t;	//event occurrence time 
	event_type type;	//event type
	int pid;	//process id
	int priority;

	void print() {
		cout << "printing event: " << end_t << " " << type << " " << pid << endl;
	}
};

class comp{
public:
 	int operator() ( const event& p1, const event &p2)
 	{
		if(p1.end_t == p2.end_t) return p1.priority < p2.priority;
 		return p1.end_t>p2.end_t;
 	}
};

class event_mgnr {
	public:
		priority_queue<event, vector<event>, comp> event_table;

	//function for adding an event in the event table
		void add_event(int end_t, event_type type, int pid, int priority)
	{
		event ev;
		ev.end_t = end_t;
		ev.type = type;
		ev.pid = pid;
		ev.priority = priority;
		event_table.push(ev);
	}

	//Is event table empty..?
	bool is_empty()
	{
		return event_table.empty();
	}
	
	//function for returning the top_most entry of the event table
	event next_event()
	{
		event ev = event_table.top();
		event_table.pop();
		return ev;
	}

};
