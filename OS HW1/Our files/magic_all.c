/*==== DEEFINES & STRUCTS ====*/
#include <linux/string.h>					// for strcmp, etc.
#include <linux/types.h>					// idk honestly
#include "../../../include/linux/sched.h"	// for struct task_struct
#include "../../../include/linux/list.h"	// for list_t
#include <linux/slab.h>						// for kmalloc, GFP_KERNEL
#include <asm/uaccess.h>
#define SECRET_MAXSIZE 32

struct secrets_list {
	struct list_head list ;
	char secret[SECRET_MAXSIZE];
};


/*==== Functions ====*/
/*	
*	Basically didnt want to include math.h...
*	so we made our own. returns the bigger number between {a,b}
*/
int maxx(int a,int b ){

	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}

/*	
*	Attach a wand to the current process with the given power level and 
*	secret and set the process’s health level to 100. If secret is shorter
*	than SECRET_MAXSIZE it will be terminated by a NULL character.
*	After this system call returns, the process can attack and be attacked 
*	by other processes until its health reaches zero. 
*	Calling this when the process already has a wand returns an error.
*
*	Return value: 
*		On failure: Negative integer
*		On success: 0
*
*	Error Cypher:
*		EEXIST [-1] | The process already has a wand
*		EFAULT [-2] | Error copying secret from user or secret is NULL
*		EINVAL [-3] | Length of secret is zero
*		ENOMEM [-4] | Cannot allocate memory
*/
int sys_magic_get_wand(int power, char secret[SECRET_MAXSIZE]){
	
	struct task_struct* p = current;

	/*==== Errors & Checks ====*/
	if (p->holding_wand) {
		//fail, already holding wand
		return -1; 
	}
	
	if (secret == NULL) {
		//fail, secret is NULL
		return -2; 
	}

	if (secret[0] == '\0') {
		//fail, size of secret is 0
		return -3; 
	}


	/*==== Getting secret's length ====*/
	int len = 0;

	for (; len < SECRET_MAXSIZE; len++) {

		if (secret[len] == '\0') {
			++len;
			break;
		}
	}


	/*==== Prepping current task for strcpy ====*/
	p->my_secret = kmalloc(sizeof(char) * (len), GFP_KERNEL);

	if (p->my_secret == NULL) {
		return -4; // fail, error in malloc
	}


	//Copy secret to task
	strcpy(p->my_secret, secret);
	

	/*==== Validating strcpy() ====*/
	if (strcmp(p->my_secret, secret)) {
		kfree(p->my_secret);
		return -2; //fail, error in copy
	}
	

	/*==== Initializing the other variables ====*/
	p->power = power;
	p->holding_wand = 1;
	p->health = 100;

	
	return 0; // SUCCESS
}

/*
*	Attack the given process. If successful, the target process’ health 
*	level is decreased by the current process’s wand power. 
*	Health level should be capped at zero and not become negative.
*	A process cannot attack itself or a process that has stolen its secret
*
*	Return value:
*		On failure: Negative integer
*		On success: New health of pid (int)
*
*	Error Cypher:
*		ESRCH        [-1] | Process pid doesn’t exist
*		EPERM        [-2] | Either the sender or pid doesnt have a wand
*		EHOSTDOWN    [-3] | Either the sender or target process health is 0
*		ENOMEM       [-4] | Cannot allocate memory
*		ECONNREFUSED [-5] | Target process is the current process or has 
*                           stolen the current process wand secret
*/
int sys_magic_attack(pid_t pid) {
	
	struct task_struct* attacker = current;

	struct task_struct* target = find_task_by_pid(pid);


	/*==== Errors & Checks ====*/

	if (target == NULL) {
		// fail, pid doesnt exist
		return -1; 
	}

	if (attacker->holding_wand == 0 || target->holding_wand == 0) {
		// fail, One of the processes isnt holding any wand.
		return -2; 
	}

	if (!(attacker->health) || !(target->health)) {
		// fail, One of the processes arrived with 0 hp.
		return -3;
	}
		
	if (attacker == target) {
		// fail, attacker is target
		return -5; 
	}


	/*==== Check for if my_secret is already stolen ====*/
	struct list_head *iterator;
	struct list_head *tmp ;
	struct list_head *secrets = target->secrets_ptr;

	list_for_each_safe(iterator, tmp, secrets) {

		struct secrets_list* current_secret;
		current_secret = list_entry(iterator, struct secrets_list, list);
		if (!strcmp(current_secret->secret, attacker->my_secret)) {
			// fail, target already has stolen attackers secret
			return -5;
		}
	};


	/*==== Updating target health =====*/

	int dmg = attacker->power;

	int health = target->health;

	target->health = max(health - dmg, 0);

	return target->health; // SUCCESS
}

/*
*	Copy the wand secret from the target process and store it in the list of
*	stolen secrets. If a process reads its own mind, nothing happens.
*	
*	Return value:
*		On failure: Negative integer
*		On success: 0
*
*	Error Cypher:
*		ESRCH  [-1] | Process pid doesn’t exist
*		EPERM  [-2] | Either the sending process or pid doesn’t have a wand
* 		EEXIST [-3] | Secret for pid was already read
*		ENOMEM [-4] | Cannot allocate memory
*		EFAULT [-5] | Error writing to user buffer
*/
int sys_magic_legilimens(pid_t pid) {

	struct task_struct* attacker = current;
	struct task_struct* target = find_task_by_pid(pid);

	/*==== Errors & Checks ====*/
	if (target == attacker) {
		// attacker is also target, so nothing happens.
		return 0;//SUCCESS
	}

	if (target == NULL) {
		// fail, pid doesnt exist
		return -1; 
	}

	if (attacker->holding_wand == 0 || target->holding_wand == 0) {
		// fail, One of the processes isnt holding any wand.
		return -2; 
	}


	/*==== Check if attacker already has target's secret ====*/
	struct list_head* iterator = attacker->secrets_ptr->next;
	struct secrets_list* current_secret;
	while (iterator != attacker->secrets_ptr) {

		current_secret = list_entry(iterator, struct secrets_list, list);

		if (!strcmp(current_secret->secret, attacker->my_secret)) {
			// fail, attacker already stole this target's secret
			return -3;
		}
		iterator = iterator->next;

	}


	/*==== Add target's secret to attackers list using ====*/
	struct secrets_list *newSec;
	newSec = kmalloc(sizeof(struct secrets_list), GFP_KERNEL);
	if (newSec == NULL) {
		//fail, kmalloc() failed...
		return -4;
	}

	if (copy_to_user(newSec->secret, target->my_secret, SECRET_MAXSIZE)) {
		//fail, copy_to_user() failed...
		kfree(newSec);
		return -5;
	}

	list_add_tail(&newSec->list, attacker->secrets_ptr);
	return 0; // SUCCESS
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
*		On failure: Negative integer
*		On success: The number of remaining secrets
*
*	Error Cypher:
*		EFAULT [-1] | secrets is NULL or error writing to user buffer
*		EPERM  [-2] | The current process doesn’t have a wand
*		ENOMEM [-4] | Cannot allocate memory
*/
int sys_magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size) {

	struct task_struct *p = current;
	
	/*==== Errors & Checks ====*/
	if (secrets == NULL) {
		// fail, secret is NULL
		return -1;
	}
	
	if (p->holding_wand == 0) {
		// fail, process not holding a wand
		return -2;
	}

	/*==== Getting secret_list length ====*/
	int list_len = 0;
	struct list_head *iterator;
	list_for_each(iterator, p->secrets_ptr) {
		list_len++;
	}

	if (size == 0) { 
		// return size of the secrets list
		return list_len;//SUCCESS
	}
	
	/*==== Copying secrets to secret_list ====*/
	struct list_head *ptr;
    ptr = p->secrets_ptr; // iterator for list secrets 
    int i, stolen = 0, counter = list_len;

    for ( i = 0; i < size; i++) {
		
		if (counter == 0) {
			// nothing to copy...
			// fill the rest with empty strings
			int j = list_len;
			for (; j < size; j++) { 
				
				secrets[j][0] = '\0';
			}
			break;
		}
		
        struct secrets_list *curr_entry;
        curr_entry = list_entry(ptr, struct secrets_list, list);
		
		if (copy_to_user(secrets[i], curr_entry->secret, SECRET_MAXSIZE)) {
		// fail, copy_to_user() failed...
			return -1;
		}

		stolen++;
        ptr = ptr->next;
		--counter;
	}
	
	return (list_len-stolen); // SUCCESS
}
