#include <string.h>
#include <conio.h>
#include <fileio.h>
#include <sched.h>
#include <geekos/syscall.h>

//int Get_CallCount();
//int Get_FileCount();

//DEF_SYSCALL(Get_CallCount, SYS_GETCALLCOUNT, int, (void),, SYSCALL_REGS_0)
//DEF_SYSCALL(Get_FileCount, SYS_GETFILECOUNT, int, (void),, SYSCALL_REGS_0)

bool match_password(char *password, char *input, int size){
	
	int posPass=0, posInp=0, len_count = 0;
	for (; len_count < size; len_count++){
		char c1 = *(password+posPass);
		char c2 = *(input+posInp);

		if(c1==c2){
			if(c1 == '\0' || c1 == '\n'){
				return true;
			}
			else{
				posPass++;
				posInp++;
				continue;
			}
		}
		//Print("c1: %c c2: %c\n", c1, c2);
		if(c1=='\n' && c2=='\0') return true;
		if(c1=='\0' && c2 == '\n') return true;

		return false;
	}
	return false;
}

int main(int argc __attribute__ ((unused)), char **argv
         __attribute__ ((unused))){
	
	const char *PasswordFile = "/c/Password.txt";
	int pass_size = 100;
	char password[pass_size];
	char input[pass_size];
	int status;
	bool matched;

	int file_descriptor = Open(PasswordFile, O_READ);
	
	if(file_descriptor!=0){
		Print("Error while opening passwor file\n");
		return -1;
	}
	
	status = Read(file_descriptor, password, pass_size);
	if(status==0){
		Print("unable to read password\n");
		return -1;
	}
	
	status = Close(file_descriptor);
	if(status!=0){
		Print("Unable to Close Password.txt\n");
		return -1;
	}

	Print_String("Please enter your password: ");
	Read_Line(input , pass_size);

	matched = match_password(password, input, pass_size);

	if(matched){
		return 0;
	}
	else{
		return 1;
	}
}
