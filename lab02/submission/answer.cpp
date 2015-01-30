#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <string>
#include <set>
#include <unistd.h>
#include <map>
#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <signal.h>
#include <wait.h>
#include <unistd.h>
using namespace std;

pair<string,string> split(string email) {
	string val = "", domain = "";
	bool val_ended = false;
	for (int i = 0; i < email.size(); i++) {
		if (email[i] == '@') val_ended = true;
		else if (val_ended) domain += email[i];
		else val += email[i];
	}
	if(val_ended)
	return pair<string, string> (val, domain);
	else return pair<string, string> ("", email);
}


int pid;
int parent_pid;
string domain_name, email_name;
map<string, int> domains;
set<string> email_names;
char *sh_action;
char *sh_email;
char *sh_email_rec;
char *sh_message;
char *sh_email_body;
int *sh_rec_pid;

static void hdl (int sig, siginfo_t *siginfo, void *context)
{
	string action(sh_action);
	string email(sh_email);
	pair<string, string> splitted = split(email);
	email_name = splitted.first;
	domain_name = splitted.second;
	string message = "";
	if (action == "add_email") {
		set<string>::iterator it = email_names.find(email_name);
		if (it == email_names.end()) {
			message = "Child process " + domain_name + " - Email address "+ email +" added successfully.";
		}
		else {
			message = "Child process " + domain_name + " - Email address already exists.";
		}
	}
	else if (action == "search_email") {
		set<string>::iterator it = email_names.find(email_name);
		if (it == email_names.end()) {
			message = "Parent process - Could not find mail address "+ email +".";
		}
		else {
			message = "Parent process - Found the email address"+ email +".";
		}
	}
	else if (action == "delete_email") {
		set<string>::iterator it = email_names.find(email_name);
		if (it == email_names.end()) {
			message = "Parent process - child process " + domain_name + " could not find the email address " + email;
		}
		else {
			message = "Child process - child process " + domain_name + " deleted email address " + email;
			email_names.erase(it);
		}
	}
	else if (action == "send_email") {
		string email_rec(sh_email_rec);
		string email_body(sh_email_body);
		set<string>::iterator it = email_names.find(email_name);
		if (it == email_names.end()) {
			cout << "Child process - User " << email_name << " does not exist." << endl;
		}
		else {
			cout << "Child process - Email sent from " << email << " to " << email_rec << endl;
			strcpy(sh_action, "rec_email");
			kill(sh_rec_pid[0], SIGUSR1);
		}
	}
	else if (action == "rec_email") {
		string email_rec(sh_email_rec);
		string email_body(sh_email_body);
		string email_name_rec = split(email_rec).first;
		set<string>::iterator it = email_names.find(email_name_rec);
		if (it == email_names.end()) {
			cout << "Child process - User " << email_name << " does not exist." << endl;
		}
		else {
			cout << "Child process - Email received at process " << sh_rec_pid[0] << " for user " << split(email_rec).first << endl;
		}
	}
	if (message.size()) {
		memcpy(sh_message, message.c_str(), message.size()+1);
		kill(parent_pid, SIGUSR2);
	}
}

void hdl_main(int sig, siginfo_t *siginfo, void *context) {
	cout << string(sh_message) << endl;
}


int main(void)
{
	parent_pid = getpid();
	int shmid, pid;
	shmid =  shmget(IPC_PRIVATE, 100*sizeof(char), IPC_CREAT | 0666);
	sh_action = (char*)shmat(shmid, NULL, 0);
	shmid =  shmget(IPC_PRIVATE, 100*sizeof(char), IPC_CREAT | 0666);
	sh_email = (char*)shmat(shmid, NULL, 0);
	shmid =  shmget(IPC_PRIVATE, 100*sizeof(char), IPC_CREAT | 0666);
	sh_email_rec = (char*)shmat(shmid, NULL, 0);
	shmid =  shmget(IPC_PRIVATE, 100*sizeof(char), IPC_CREAT | 0666);
	sh_message = (char*)shmat(shmid, NULL, 0);
	shmid =  shmget(IPC_PRIVATE, 1000*sizeof(char), IPC_CREAT | 0666);
	sh_email_body = (char*)shmat(shmid, NULL, 0);
	shmid =  shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
	sh_rec_pid = (int*)shmat(shmid, NULL, 0);
	struct sigaction act;
	memset (&act, '\0', sizeof(act));
	act.sa_sigaction = &hdl;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &act, NULL);


	struct sigaction act1;
	memset (&act, '\0', sizeof(act1));
	act1.sa_sigaction = &hdl_main;
	act1.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR2, &act1, NULL);

	while (true) {
		usleep(1000);
		string action, email, email_rec, email_body, domain_name_rec, email_name_rec;
		cin >> action >> email;
		if (action == "stop"){
			for (map<string, int> ::iterator it = domains.begin(); it != domains.end(); it++) {
				kill(it->second, SIGKILL);
				waitpid(-1, NULL,0);
			}
			break;
		};
		if (action == "send_email") {
			cin >> email_rec >> email_body;
			pair<string, string> p_rec = split(email_rec);
			domain_name_rec = p_rec.second;
			email_name_rec = p_rec.first;
		}
		pair<string, string> p = split(email);
		domain_name = p.second;
		email_name = p.first;
		
		if (action == "send_email") {
			map<string, int>::iterator it = domains.find(p.second);
			map<string, int>::iterator it_rec = domains.find(domain_name_rec);
			if (it == domains.end() || it_rec == domains.end()) {
				cout << "Parent process - the sender/receiver does not exist." << endl;
			}
			else {
				memcpy(sh_action, action.c_str(), action.size()+1);
				memcpy(sh_email, email.c_str(), email.size()+1);
				memcpy(sh_email_rec, email_rec.c_str(), email_rec.size()+1);
				memcpy(sh_email_body, email_body.c_str(), email_body.size()+1);
				sh_rec_pid[0] = it_rec->second;
				kill(it->second,SIGUSR1);
			}
			continue;
		}

		map<string, int>::iterator it = domains.find(p.second);
		if (it == domains.end()) {
			if (action == "search_email" || action == "delete_email" || action == "delete_domain") {
				cout << "Parent process - Domain does not exist" << endl;
			}
			else  {
				pid = fork();
				if (pid == 0) break;
				domains[p.second] = pid;
			}
		}
		else {
			if (action == "delete_domain") {
				pid = it->second;
				domains.erase(it);
				kill(pid, SIGKILL);
				waitpid(-1, NULL,0);
				cout << "Parent process - Domain " << email << " with PID - " << pid << " deleted." << endl;
			}
			memcpy(sh_action, action.c_str(), action.size()+1);
			memcpy(sh_email, email.c_str(), email.size()+1);
			kill(it->second,SIGUSR1);
		}
	}
	if (pid == 0) {
		email_names.insert(email_name);
		string message = "Child process " + domain_name + " - Email address "+ email_name+"@"+domain_name +" added successfully.";
		memcpy(sh_message, message.c_str(), message.size()+1);
		kill(parent_pid, SIGUSR2);
		while(1) sleep(1000);
	}
	return 0;
}

