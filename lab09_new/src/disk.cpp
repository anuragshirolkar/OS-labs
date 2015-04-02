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
#include <cstring>
using namespace std;

#define FILENAME "disk"
#define BLOCKSIZE 1024
#define NBLOCKS 1024

struct disk
{
	FILE * fdisk;

	disk();
	void initialize();
	void mkdir(int, const char[]);
	int get_next_free_block();
	void ls(int);
	void setupdir(int, int);
	int cd(int, const char[]);
	int get_root();
	int open(int, const char[]);
	void setupfile(int);
	void seek(int, int);
	int read(int, int, int, char[]);
	int write(int, int, int, const char[]);
};


disk::disk() {
	fdisk = fopen("disk", "w+");
}

void disk::initialize() {
	fprintf(fdisk, "%04d\n", 1024);
	fprintf(fdisk, "%04d\n", 6);
	fprintf(fdisk, "%04d\n", 5);
	fseek(fdisk, BLOCKSIZE, SEEK_SET);
	fprintf(fdisk, "%04d\n", 1017);
	for (int i = 1023; i >= 7; i--) {
		fprintf(fdisk, "%04d\n", i);
	}
	fseek(fdisk, 6*BLOCKSIZE, SEEK_SET);
	fprintf(fdisk, "%04d\n", 0);
	fprintf(fdisk, "%04d\n", 0);
	fprintf(fdisk, "%04d\n", -1);
	return;
}

int disk::get_next_free_block() {
	int curlocn = ftell(fdisk);
	fseek(fdisk, BLOCKSIZE, SEEK_SET);
	int nfreeblocks;
	fscanf(fdisk, "%d\n", &nfreeblocks);
	fseek(fdisk, BLOCKSIZE, SEEK_SET);
	fprintf(fdisk, "%04d\n", nfreeblocks-1);
	fseek(fdisk, BLOCKSIZE+nfreeblocks*5, SEEK_SET);
	int nextfreeblock;
	fscanf(fdisk, "%d\n", &nextfreeblock);
	fseek(fdisk, curlocn, SEEK_SET);
	return nextfreeblock;
}

void disk::mkdir(int dirblock, const char dirname[]) {
	fseek(fdisk, dirblock*BLOCKSIZE, SEEK_SET);
	int size, ndirs;
	fscanf(fdisk, "%d\n%d\n", &size, &ndirs);
	fseek(fdisk, dirblock*BLOCKSIZE+15, SEEK_SET);
	for (int i = 0; i < size; i++) {
		char name[100];
		int type, locnblock;
		fscanf(fdisk, "%s %d %d\n", name, &type, &locnblock);
		if (strcmp(name, dirname) == 0) {
			printf("folder %s already exists.\n", dirname);
			return;
		}
	}
	int locnblock = get_next_free_block();
	fprintf(fdisk, "%s 1 %d\n", dirname, locnblock);
	fseek(fdisk, dirblock*BLOCKSIZE, SEEK_SET);
	fprintf(fdisk, "%04d\n%04d\n", size+1, ndirs+1);
	setupdir(locnblock, dirblock);
	return;
}

void disk::ls(int dirblock) {
	fseek(fdisk, dirblock*BLOCKSIZE, SEEK_SET);
	int size, ndirs;
	fscanf(fdisk, "%d\n%d\n", &size, &ndirs);
	fseek(fdisk, dirblock*BLOCKSIZE+15, SEEK_SET);
	for (int i = 0; i < size; i++) {
		char name[100];
		int type, locnblock;
		fscanf(fdisk, "%s %d %d\n", name, &type, &locnblock);
		if (type) printf("%s/\n", name);
		else printf("%s\n", name);
	}
}

void disk::setupdir(int dirblock, int parent) {
	fseek(fdisk, dirblock*BLOCKSIZE, SEEK_SET);
	fprintf(fdisk, "%04d\n%04d\n%04d\n", 0, 0, parent);
}

int disk::cd(int dirblock, const char dirname[]) {
	if (strcmp(dirname, "..") == 0) {
		fseek(fdisk, dirblock*BLOCKSIZE+10, SEEK_SET);
		int parent;
		fscanf(fdisk, "%d\n", &parent);
		if (parent == -1) return dirblock;
		else return parent;
	}
	fseek(fdisk, dirblock*BLOCKSIZE, SEEK_SET);
	int size, ndirs;
	fscanf(fdisk, "%d\n%d\n", &size, &ndirs);
	fseek(fdisk, dirblock*BLOCKSIZE+15, SEEK_SET);
	for (int i = 0; i < size; i++) {
		char name[100];
		int type, locnblock;
		fscanf(fdisk, "%s %d %d\n", name, &type, &locnblock);
		if (strcmp(name, dirname) == 0) {
			if (type == 1) return locnblock;
			else {
				printf("%s is not a directory.\n", dirname);
				return -1;
			}
		}
	}
	printf("No file or directory named %s\n", dirname);
	return -2;
}

int disk::get_root() {
	int curlocn = ftell(fdisk);
	fseek(fdisk, 5, SEEK_SET);
	int rootlocn;
	fscanf(fdisk, "%d\n", &rootlocn);
	return rootlocn;
}

int disk::open(int dirblock, const char filename[]) {
	fseek(fdisk, dirblock*BLOCKSIZE, SEEK_SET);
	int size, ndirs;
	fscanf(fdisk, "%d\n%d\n", &size, &ndirs);
	fseek(fdisk, dirblock*BLOCKSIZE+15, SEEK_SET);
	for (int i = 0; i < size; i++) {
		char name[100];
		int type, locnblock;
		fscanf(fdisk, "%s %d %d\n", name, &type, &locnblock);
		if (strcmp(name, filename) == 0) {
			if (type == 1) {
				printf("%s is not a file.\n", filename);
				return -1;
			}
			return locnblock;
		}
	}
	int locnblock = get_next_free_block();
	fprintf(fdisk, "%s 0 %d\n", filename, locnblock);
	fseek(fdisk, dirblock*BLOCKSIZE, SEEK_SET);
	fprintf(fdisk, "%04d\n%04d\n", size+1, ndirs);
	setupfile(locnblock);
	return locnblock;
}

void disk::setupfile(int locnblock) {
	fseek(fdisk, locnblock*BLOCKSIZE, SEEK_SET);
	fprintf(fdisk, "%04d\n", -1);
}

void disk::seek(int fileblock, int seek_to) {
	int tot_blocks = (seek_to + 1018)/1019;
	int cur_block = fileblock;
	for (int i = 0; i < tot_blocks-1; i++) {
		fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
		int next;
		fscanf(fdisk, "%d\n", &next);
		if (next == -1) {
			next = get_next_free_block();
			fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
			fprintf(fdisk, "%04d\n", next);
			cout << "block " << next << " added to the file" << endl;
			setupfile(next);
		}
		cur_block = next;
	}
	
}

int disk::read(int fileblock, int current_byte, int nbytes, char result[]) {
	int read_from_block = current_byte/1019;
	int seek_in_block = current_byte%1019;
	int cur_block = fileblock;
	for (int i = 0; i < read_from_block; i++) {
		if (cur_block == -1) return -1;
		fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
		fscanf(fdisk, "%d\n", &cur_block);
	}
	int to_read, have_read;
	if (seek_in_block + nbytes <= 1019) {
		if (cur_block == -1) return -1;
		fseek(fdisk, cur_block*BLOCKSIZE+seek_in_block+5, SEEK_SET);
		fread(result, 1, nbytes, fdisk);
		cout << "reading from block " << cur_block << endl;
		return 0;
	}
	else {
		fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
		int next;
		fscanf(fdisk, "%d\n", &next);
		fseek(fdisk, cur_block*BLOCKSIZE+seek_in_block+5, SEEK_SET);
		fread(result, 1, 1019-seek_in_block, fdisk);
		cout << "reading from block " << cur_block << endl;
		to_read = nbytes - (1019-seek_in_block);
		have_read = 1019-seek_in_block;
		cur_block = next;
	}
	while (to_read) {
		if (cur_block == -1) return -1;
		fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
		cout << "reading from block " << cur_block << endl;
		fscanf(fdisk, "%d\n", &cur_block);
		if (to_read <= 1019) {
			fread(result+have_read, 1, to_read, fdisk);
			have_read += to_read;
			to_read = 0;
		}
		else {
			fread(result+have_read, 1, 1019, fdisk);
			to_read -= 1019;
			have_read += 1019;
		}
	}
	return 0;
	
	
	// int to_read = nbytes, have_read = 0;
	// int cur_block = fileblock;
	// int to_seek = current_byte;
	// while (to_read) {
	// 	if (cur_block == -1) return -1;
	// 	fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
	// 	fscanf(fdisk, "%d\n", &cur_block);
	// 	if (to_seek == 0) {
	// 		if (to_read <= 1019) {
	// 			fread(result+have_read, 1, to_read, fdisk);
	// 			have_read += to_read;
	// 			to_read = 0;
	// 		}
	// 		else {
	// 			fread(result+have_read, 1, 1019, fdisk);
	// 			to_read -= 1019;
	// 			have_read += 1019;
	// 		}
	// 	}
	// 	if (to_seek <= 1019) {
	// 		have_seeked += to_seek;
	// 		to_read = 0;
	// 	}
	// 	else {
	// 		fread(result+have_read, 1, 1019, fdisk);
	// 		to_read -= 1019;
	// 		have_read += 1019;
	// 	}
	// }
	// return 0;
}

int disk::write(int fileblock, int current_byte, int nbytes, const char buffer[]) {
	seek(fileblock, current_byte+nbytes);
	int write_from_block = current_byte/1019;
	int seek_in_block = current_byte%1019;
	int cur_block = fileblock;
	for (int i = 0; i < write_from_block; i++) {
		if (cur_block == -1) return -1;
		fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
		fscanf(fdisk, "%d\n", &cur_block);
	}
	int to_write, have_written;
	if (seek_in_block + nbytes <= 1019) {
		if (cur_block == -1) return -1;
		fseek(fdisk, cur_block*BLOCKSIZE+seek_in_block+5, SEEK_SET);
		fwrite(buffer, 1, nbytes, fdisk);
		return 0;
	}
	else {
		fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
		int next;
		fscanf(fdisk, "%d\n", &next);
		fseek(fdisk, cur_block*BLOCKSIZE+seek_in_block+5, SEEK_SET);
		fwrite(buffer, 1, 1019-seek_in_block, fdisk);
		to_write = nbytes - (1019-seek_in_block);
		have_written = 1019-seek_in_block;
		cur_block = next;
	}
	while (to_write) {
		fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
		int next;
		fscanf(fdisk, "%d\n", &next);
		if (to_write <= 1019) {
			fwrite(buffer+have_written, 1, to_write, fdisk);
			have_written += to_write;
			to_write = 0;
		}
		else {
			if (next == -1){
				int locnblock = get_next_free_block();
				setupfile(locnblock);
				fseek(fdisk, cur_block*BLOCKSIZE, SEEK_SET);
				fprintf(fdisk, "%04d\n", locnblock);
				cur_block = locnblock;
			}
			else {
				cur_block = next;
			}
			fwrite(buffer+have_written, 1, 1019, fdisk);
			to_write -= 1019;
			have_written += 1019;
		}
	}
	return 0;
}
