#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char **argv){
	ofstream fout(argv[1]);
	while(true){
		fout << "a";
	}
	return 0;
}
