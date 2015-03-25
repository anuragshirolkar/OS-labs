
#include <geekos/syscall.h>
#include <fmtout.h>
#include <string.h>
#include <conio.h>

int Count_SysCall();
int Count_FileOpenCount();

DEF_SYSCALL(Count_SysCall, SYS_COUNTCALL, int, (void),, SYSCALL_REGS_0)
DEF_SYSCALL(Count_FileOpenCount, SYS_FILEOPENCOUNT, int, (void),, SYSCALL_REGS_0)
DEF_SYSCALL(Count_ReadCount, SYS_READSYSCOUNT, int, (void),, SYSCALL_REGS_0)

int main(){
	char buff[50];
	int x,y,z;
	Read_Line(buff, 50);
	x = Count_SysCall();
	y = Count_FileOpenCount();
	z = Count_ReadCount();
	Print("Sys call count: %d\nFile Open Count: %d\nRead System Calls count: %d\n", x,y,z);
	return 0;
}