#include <conio.h> //This is not C/C++ header file, this is in GeekOS
#include <sched.h>
int main()
{
	int xyz, abc;
	xyz = Get_Time_Of_Day();
	Get_NewTOD(&abc);
	Print("Output from old get time of day syscall : %d\nOutput from new get time of day syscall : %d\n", xyz, abc);
	return 0;
}
