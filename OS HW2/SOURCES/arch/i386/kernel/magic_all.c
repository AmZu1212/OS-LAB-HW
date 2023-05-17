/*==== DEEFINES & STRUCTS ====*/
#include <linux/string.h>					// for strcmp, etc.
#include <linux/types.h>					// idk honestly
#include "../../../include/linux/sched.h"	// for struct task_struct
#include "../../../include/linux/list.h"	// for list_t
#include <linux/slab.h>						// for kmalloc, GFP_KERNEL
#include <asm/uaccess.h>
#include <linux/errno.h>

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
*	secret and set the processs health level to 100. If secret is shorter
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
*		EEXIST | The process already has a wand
*		EFAULT | Error copying secret from user or secret is NULL
*		EINVAL | Length of secret is zero
*		ENOMEM | Cannot allocate memory
*/
int sys_magic_get_wand(int power, char secret[SECRET_MAXSIZE]){
	
	struct task_struct* p = current;

	/*==== Errors & Checks ====*/
	if (p->holding_wand) {
		//fail, already holding wand
		return -EEXIST; 
	}
	
	if (secret == NULL) {
		//fail, secret is NULL
		return -EFAULT; 
	}

	if (secret[0] == '\0') {
		//fail, size of secret is 0
		return -EINVAL; 
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
		return -ENOMEM; // fail, error in malloc
	}


	//Copy secret to task
	strcpy(p->my_secret, secret);
	

	/*==== Validating strcpy() ====*/
	if (strcmp(p->my_secret, secret)) {
		kfree(p->my_secret);
		return -EFAULT; //fail, error in copy
	}
	

	/*==== Initializing the other variables ====*/
	p->power = power;
	p->holding_wand = 1;
	p->health = 100;

	
	return 0; // SUCCESS
}

/*
*	Attack the given process. If successful, the target process health 
*	level is decreased by the current processs wand power. 
*	Health level should be capped at zero and not become negative.
*	A process cannot attack itself or a process that has stolen its secret
*
*	Return value:
*		On failure: Negative integer
*		On success: New health of pid (int)
*
*	Error Cypher:
*		ESRCH        | Process pid doesnt exist
*		EPERM        | Either the sender or pid doesnt have a wand
*		EHOSTDOWN    | Either the sender or target process health is 0
*		ENOMEM       | Cannot allocate memory
*		ECONNREFUSED | Target process is the current process or has 
*                           stolen the current process wand secret
*/
int sys_magic_attack(pid_t pid) {
	
	struct task_struct* attacker = current;

	struct task_struct* target = find_task_by_pid(pid);


	/*==== Errors & Checks ====*/

	if (target == NULL) {
		// fail, pid doesnt exist
		return -ESRCH; 
	}

	if (attacker->holding_wand == 0 || target->holding_wand == 0) {
		// fail, One of the processes isnt holding any wand.
		return -EPERM; 
	}

	if (!(attacker->health) || !(target->health)) {
		// fail, One of the processes arrived with 0 hp.
		return -EHOSTDOWN;
	}
		
	if (attacker == target) {
		// fail, attacker is target
		return -ECONNREFUSED; 
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
			return -ECONNREFUSED;
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
*		ESRCH  | Process pid doesnt exist
*		EPERM  | Either the sending process or pid doesnt have a wand
* 		EEXIST | Secret for pid was already read
*		ENOMEM | Cannot allocate memory
*		EFAULT | Error writing to user buffer
*/
int sys_magic_legilimens(pid_t pid) {

	struct task_struct* attacker = current;
	struct task_struct* target = find_task_by_pid(pid);

	

	/*==== Errors & Checks ====*/
	if (target == attacker) {
		// attacker is also target, so nothing happens.
		return 0; // SUCCESS
	}

	if (target == NULL) {
		// fail, pid doesnt exist
		return -ESRCH; 
	}

	if (attacker->holding_wand == 0 || target->holding_wand == 0) {
		// fail, One of the processes isnt holding any wand.
		return -EPERM; 
	}

	

	/*==== Check if attacker already has target's secret ====*/
	struct list_head* iterator = attacker->secrets_ptr->next;
	struct secrets_list* current_secret;
	while (iterator != attacker->secrets_ptr) {
		current_secret = list_entry(iterator, struct secrets_list, list);
		
		if (strcmp(current_secret->secret, target->my_secret) == 0) {
			// fail, attacker already stole this target's secret
			return -EEXIST;
		}
		iterator = iterator->next;

	}


	/*==== Add target's secret to attackers list using strcpy? ====*/
	struct secrets_list *newSec;
	newSec = kmalloc(sizeof(struct secrets_list), GFP_KERNEL);
	if (newSec == NULL) {
		// fail, kmalloc() failed...
		return -ENOMEM;
	}

	if (strcpy(newSec->secret, target->my_secret) == NULL) {
		// fail, strcpy() failed...
		kfree(newSec);
		return -EFAULT;
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
*	until entry size. If size=0, dont touch secrets and return the total
*	number of secrets.
*
*	Return value:
*		On failure: Negative integer
*		On success: The number of remaining secrets
*
*	Error Cypher:
*		EFAULT | secrets is NULL or error writing to user buffer
*		EPERM  | The current process doesnt have a wand
*		ENOMEM | Cannot allocate memory
*/
int sys_magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size) {

	struct task_struct *p = current;

	/*==== Errors & Checks ====*/
	if (p->holding_wand == 0) {
		// fail, process not holding a wand
		return -EPERM;
	}

	/*==== LOOP FOR TESTING LIST STRINGS ====*/
	struct list_head* itr = p->secrets_ptr->next;
	struct secrets_list* current_secret;
	while (itr != p->secrets_ptr) {
		printk("entered while loop\n");
		current_secret = list_entry(itr, struct secrets_list, list);
		printk("current's secret is [ %s ]\n", current_secret->secret);
		
		itr = itr->next;

	}
	//========================================


	/*==== Getting secret_list length ====*/
	int list_len = 0;
	struct list_head* iterator;
	list_for_each(iterator, p->secrets_ptr) {
		list_len++;
	}


	if (size == 0) {
		// return size of the secrets list
		return list_len;//SUCCESS
	}

	if (secrets == NULL) {
		// fail, secret is NULL
		return -EFAULT;
	}
	
	
	/*==== Copying secrets to secret_list ====*/
	struct list_head *ptr;
    ptr = p->secrets_ptr->next; // iterator for list secrets 
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
		if (copy_to_user(secrets[i], curr_entry->secret, SECRET_MAXSIZE) != 0) {
		// fail, copy_to_user() failed...
			return -EFAULT;
		}

		stolen++;
        ptr = ptr->next;
		--counter;
	}
	return (list_len-stolen); // SUCCESS
}


/*
*	Description:
*	Gives the calling process exclusive CPU access for "seconds" seconds.
*
*	Return value:
*		On failure: Negative integer
*		On success: 0
*
*	Error Cypher:
*		EPERM  | The calling process doesn’t have a wand
*		ENOMEM | Error allocating memory
*/
int sys_magic_clock(unsigned int seconds) 
{
	printk("Entered magic_clock with %d seconds\n", seconds);
	struct task_struct* p = current;
	if (p->holding_wand == 0) {
		// fail, The process isn't holding a wand.
		printk("process not holding wand...\n");
		return -EPERM;
	}



	unsigned int newjiffies = seconds * HZ;
	printk("new calculated jiffies is: %d\n", newjiffies);

	// Set the scheduling policy to FIFO and priority to 0 (realtime)
	int err = sched_setscheduler(p, SCHED_FIFO, &(struct sched_param){.sched_priority = 0 });
	if (err != 0) {
		printk("UPGRADE priority failed...\n");
		return -ENOMEM; // MAYBE DIFFERENT RETURN VALUE?
	}

	printk("new priority = 0 is set\n");

	p->magic_time = seconds;
	p->time_slice = newjiffies; // check later for needed include

	printk("BEFORE magic_clock schedule call\n");
	schedule();
	printk("AFTER magic_clock schedule call\n");

	return 0;

}