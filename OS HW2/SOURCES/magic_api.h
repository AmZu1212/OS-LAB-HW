//magic_api.h, user warper functions
#define SECRET_MAXSIZE 32 
#include <errno.h>
/*
*	Attach a wand to the current process with the given power level and
*	secret and set the process’s health level to 100. If secret is shorter
*	than SECRET_MAXSIZE it will be terminated by a NULL character.
*	After this system call returns, the process can attack and be attacked
*	by other processes until its health reaches zero.
*	Calling this when the process already has a wand returns an error.
*
*	Return value:
*		On failure: -1
*		On success: 0
*
*	Error Cypher:
*		EEXIST | The process already has a wand
*		EFAULT | Error copying secret from user or secret is NULL
*		EINVAL | Length of secret is zero
*		ENOMEM | Cannot allocate memory
*/
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
		: "m" (power), "m"(secret)
	);


	if (res < 0) {
		errno = -res;
		res = -1;
	}

	return res; // 
}

/*
*	Attack the given process. If successful, the target process’ health
*	level is decreased by the current process’s wand power.
*	Health level should be capped at zero and not become negative.
*	A process cannot attack itself or a process that has stolen its secret
*
*	Return value:
*		On failure: -1
*		On success: New health of pid (int)
*
*	Error Cypher:
*		ESRCH        | Process pid doesn’t exist
*		EPERM        | Either the sender or pid doesnt have a wand
*		EHOSTDOWN    | Either the sender or target process health is 0
*		ENOMEM       | Cannot allocate memory
*		ECONNREFUSED | Target process is the current process or
*							has stolen the current process wand secret
*/
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
		errno = -res;
		res = -1;
	}

	return res; // 
}

/*
*	Copy the wand secret from the target process and store it in the list of
*	stolen secrets. If a process reads its own mind, nothing happens.
*
*	Return value:
*		On failure: -1
*		On success: 0
*
*	Error Cypher:
*		ESRCH  | Process pid doesn’t exist
*		EPERM  | Either the sending process or pid doesn’t have a wand
* 		EEXIST | Secret for pid was already read
*		ENOMEM | Cannot allocate memory
*		EFAULT | Error writing to user buffer
*/
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
		errno = -res;
		res = -1;
	}

	return res; // 
}

/*
*	Write the first size secrets into the secrets array pointed to by
*	secrets and return the number of remaining secrets
*	(i.e. TOTAL_SECRETS-size).
*	Each entry of secrets should contain the secret from one process and
*	should be terminated with NULL if the secret length is shorter
*	than SECRET_MAXSIZE. If size is more than the total number of available
*	secrets, secrets should be padded with empty entries (empty strings)
*	until entry size. If size=0, don’t touch secrets and return the total
*	number of secrets.
*
*	Return value:
*		On failure: -1
*		On success: The number of remaining secrets
*
*	Error Cypher:
*		EFAULT | secrets is NULL or error writing to user buffer
*		EPERM  | The current process doesn’t have a wand
*		ENOMEM | Cannot allocate memory
*/
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
		: "m" (secrets), "m"(size)
	);


	if (res < 0) {
		errno = -res;
		res = -1;
	}

	return res; // FAIL
}

/*
*	Description:
*	Gives the calling process exclusive CPU access for "seconds" seconds.
*
*	Return value:
*		On failure: -1
*		On success: 0
*
*	Error Cypher:
*		EPERM  | The calling process doesn’t have a wand.
*		ENOMEM | Error allocating memory.
*/
int magic_clock(unsigned int seconds)
{
	int res;
	__asm__
	(
		"pushl %%eax;"
		"pushl %%ebx;"
		"movl $247, %%eax;"
		"movl %1, %%ebx;"
		"int $0x80;"
		"movl %%eax,%0;"
		"popl %%ebx;"
		"popl %%eax;"
		: "=m" (res)
		: "m" (pid)
	);


	if (res < 0) {
		errno = -res;
		res = -1;
	}

	return res; // 
}
