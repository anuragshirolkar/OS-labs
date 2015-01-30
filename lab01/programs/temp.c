#include <stdio.h>
#include <unistd.h>

int main(void)
{
	printf("%li\n", sysconf(_SC_CLK_TCK));
	return 0;
}
