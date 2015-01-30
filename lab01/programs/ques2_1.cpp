#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
	for ( long int i = 0 ; i <= 100000000 ; i++ ) {
		long int x = 1 ;
		for ( long int j = 1 ; j <= 5 ; j++ ) 
			x *= i ;
		cout << x << "\n" ;
	}
	
		
	sleep(300);
	return 0;
}
