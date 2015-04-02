#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
using namespace std;



int main()
{
	// FILE * f;
	// f = fopen(".", "w+");
	// fseek(f, 10, SEEK_SET);
	// fprintf(f, "%05d\n", -12);
	// fseek(f, 50, SEEK_SET);
	// fprintf(f, "%05d\n", -12);
	// fseek(f, 10, SEEK_SET);
	// int a;
	// fscanf(f, "%d", &a);
	// printf("%05d\n", a);
	char *a = "\0hey\0\0";
	int i;
	for(i = 0; i < 4; i++)
		printf("%c", a[i]);
}
