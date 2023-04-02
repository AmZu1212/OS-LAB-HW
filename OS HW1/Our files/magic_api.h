//magic_api.h
//warper functions		
#include <include/linux/errno.h> 
#define SECRET_MAXSIZE 32 

int magic_get_wand(int power, char secret[SECRET_MAXSIZE])
{ 
	int res;
	 __asm__ 
	( 
		"pushl %%eax;"
		"pushl %%ebx;"
		"pushl %%ecx;"
		"movl $243, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"int $0x80;"
		"movl %%eax,%0;"
		"popl %%ecx;"
		"popl %%ebx;"
		"popl %%eax;"
		: "=m" (res)
		: "m" (power),"m"(secret) 
	); 
	
	 
	if (res < 0) {
		switch (res) {
		case -1:
			errno = EEXIST; // The process already has a wand
			break;

		case -2:
			errno = EFAULT; // Error copying secret from user or secret is NULL
			break;

		case -3:
			errno = EINVAL; // Length of secret is zero
			break;

		case -4:
			errno = ENOMEM; // Cannot allocate memory
			break;
		}
	} else {

		return res; // Success
	}

	res = -1;
	return res; // Fail
}

int magic_attack(pid_t pid)
{ 
	int res;
	 __asm__ 
	( 
		"pushl %%eax;"
		"pushl %%ebx;"
		"movl $244, %%eax;"
		"movl %1, %%ebx;"
		"int $0x80;"
		"movl %%eax,%0;"
		"popl %%ebx;"
		"popl %%eax;"
		: "=m" (res)
		: "m" (pid) 
	); 
	
	 if (res < 0) {
		 switch (res) {
		 case -1:
			 errno = ESRCH; // The process PID doesnt exist
			 break;

		 case -2:
			 errno = EPERM; // Either the sending process or pid doesn’t have a wand
			 break;

		 case -3:
			 errno = EHOSTDOWN; // Either the sending or target process’ health is zero
			 break;

		 case -4:
			 errno = ENOMEM; // Cannot allocate memory
			 break;

		 case -5:
			 errno = ECONNREFUSED; //  Target process is the current process or has
			                       //  stolen the current process’ wand secret
			 break;
		 }
	 }
	 else {

		 return res; // Success
	 }

	 res = -1;
	 return res; // Fail
}

int magic_legilimens(pid_t pid)
{ 
	int res;
	 __asm__ 
	( 
		"pushl %%eax;"
		"pushl %%ebx;"
		"movl $245, %%eax;"
		"movl %1, %%ebx;"
		"int $0x80;"
		"movl %%eax,%0;"
		"popl %%ebx;"
		"popl %%eax;"
		: "=m" (res)
		: "m" (pid) 
	); 
	
	 if (res < 0) {
		 switch (res) {
		 case -1:
			 errno = ESRCH; // The process PID doesnt exist
			 break;

		 case -2:
			 errno = EPERM; // Either the sending process or pid doesn’t have a wand
			 break;

		 case -3:
			 errno = EEXIST; // Secret for pid was already read
			 break;

		 case -4:
			 errno = ENOMEM; // Cannot allocate memory
			 break;

		 }
	 }
	 else {

		 return res; // Success
	 }

	 res = -1;
	 return res; // Fail
}

int magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size)
{ 
	int res;
	 __asm__ 
	( 
		"pushl %%eax;"
		"pushl %%ebx;"
		"pushl %%ecx;"
		"movl $246, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
		"int $0x80;"
		"movl %%eax,%0;"
		"popl %%ecx;"
		"popl %%ebx;"
		"popl %%eax;"
		: "=m" (res)
		: "m" (secrets),"m"(size) 
	); 
	
	 if (res < 0) {
		 switch (res) {
		 case -1:
			 errno = EFAULT; // secrets is NULL or error writing to user buffer
			 break;

		 case -2:
			 errno = EPERM; // The current process doesn’t have a wand
			 break;

		 
		 case -4:
			 errno = ENOMEM; // Cannot allocate memory
			 break;

		 }
	 }
	 else {

		 return res; // Success
	 }

	 res = -1;
	 return res; // Fail
}
