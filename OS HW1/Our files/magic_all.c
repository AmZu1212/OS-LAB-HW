//new magic syscalls
//arch/i386/kernel/magic
#include <linux/string.h>	// for strcmp, etc.
#include <linux/types.h>	// idk honestly
#include "../../../include/linux/sched.h"	// for struct task_Struct
#include "../../../include/linux/list.h"		// for list_t
#include <linux/slab.h>		// for kmalloc,GFP_KERNEL
#include <linux/uaccess.h>
//#include <linux/gfp.h>	// for GFP_KERNEL
#define SECRET_MAXSIZE 32


struct secrets_list {
	struct list_head list ;
	char secret[SECRET_MAXSIZE];
};


int maxx(int a,int b ){

	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}

int sys_magic_get_wand(int power, char secret[SECRET_MAXSIZE]){
	
	struct task_struct* p ;
	p = current;

	/*=========== error checks ===========*/
	if (p->holding_wand) {
		return -1; //fail, already holding wand
	}
	
	if (secret == NULL) {
		return -2; //fail, secret is NULL
	}

	if (strlen(secret) == 0) {	
		return -3; //fail, size of secret is 0
	}


	/*=========== copying secret -> my_secret ===========*/
	int len = 0;

	for (; len < SECRET_MAXSIZE; len++) {

		if (secret[len] == '\0') {
			++len;
			break;
		}
	}

	p->my_secret = kmalloc(sizeof(char)*(len), GFP_KERNEL); // should be 31 + 1!

	if (p->my_secret == NULL) {
		return -4; // fail, error in malloc
	}

	strcpy(p->my_secret, secret);
	
	if (strcmp(p->my_secret, secret)) {
		kfree(p->my_secret);
		return -2; //fail, error in copy
	}

	 
	/*=========== the rest of the variables ===========*/
	//struct list_head *new_head;
	//new_head = kmalloc(sizeof(struct list_head), GFP_KERNEL);
	//if (new_head == NULL) {
	//	return -4; // fail, error in malloc
	//}

	//INIT_LIST_HEAD(new_head);

	//p->secrets_ptr = new_head;
	p->power = power;
	p->holding_wand = 1;
	p->health = 100;

	return 0; // success
}
//fix includes from linux
int sys_magic_attack(pid_t pid) {
	
	struct task_struct* attacker = current;

	struct task_struct* target = find_task_by_pid(pid);// found in /include/linux/sched.h


	/*=========== error checks ===========*/

	if (target == NULL) {
		return -1; // pid doesnt exist
	}

	if (attacker->holding_wand == 0 ||
		target->holding_wand == 0) {
		return -2; // one of the processes isnt holding any wand.
	}

	if (!(attacker->health) || !(target->health)) {
		return -3; // one of the processes arrived with 0 hp.
	}

	if (attacker == target) {
		return -5; // attacker is target
	}


	/*=========== check for if my_secret is already stolen ===========*/ // problem: check syntax later ?

	struct list_head *iterator;
	struct list_head *tmp ;
	struct list_head *secrets = target->secrets_ptr;

	list_for_each_safe(iterator, tmp, secrets) {

		struct secrets_list *current_secret = list_entry(iterator, struct secrets_list, list);
		if (!strcmp(current_secret->secret, attacker->my_secret)) {
			return -5; // target already has stolen attackers secret.
		}
	};

	/*=========== assuming everything is valid ===========*/

	int dmg = attacker->power;

	int health = target->health;

	target->health = max(health - dmg, 0);

	return target->health;
}
//need to update my_secret to a pointer type instead. (done)
int sys_magic_legilimens(pid_t pid) {

	struct task_struct* attacker = current;

	struct task_struct* target = find_task_by_pid(pid);

	/*=========== error checks ===========*/

	if (target == attacker) {
		// attacker is also target, so nothing happens.
		return 0; 
	}

	if (target == NULL) {
		// pid doesnt exist
		return -1; 
	}

	if (attacker->holding_wand == 0 ||
		target->holding_wand == 0) {
		return -2; // one of the processes isnt holding any wand.
	}

	/* check if attacker already has target's secret. (return -3)*/
	struct list_head* iterator = attacker->secrets_ptr->next;
	struct secrets_list* current_secret;
	while (iterator != attacker->secrets_ptr) {

		current_secret = list_entry(iterator, struct secrets_list, list);

		if (!strcmp(current_secret->secret, attacker->my_secret)) {
			//attacker already stole this target's secret
			return -3;
		}
		iterator = iterator->next;

	}

	/* add target's secret to attackers list using */
	struct secrets_list new_secret;
	strcpy(new_secret.secret,target->my_secret);
	list_add_tail(&new_secret.list, attacker->secrets_ptr);


	return 0; // success
}

int sys_magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size) {

	
	struct task_struct* p = current;
	if (p->holding_wand == 0 || secrets == NULL) {
		//process not holding a wand or secret is null
		return -2;
	}

	int list_len = 0;

	struct list_head* iterator;

	list_for_each(iterator, p->secrets_ptr) {

		list_len++;
	}

	if (size == 0) { // returns size of secrets list
		return list_len;
	}


	int counter = list_len;

	struct list_head *ptr;
    ptr = p->secrets_ptr->next ; // iterator for list secrets 
    int i;
	int stolen = 0;//=================================================================================
    for ( i = 0; i < size; i++) {
		
		if (!(counter)) {
			int j;
			for ( j = list_len; j < size; j++) { // fill the rest with empty strings
				strcpy(secrets[j], "");
			}
			break;
		}
		
		/*copy i secret using entry + head.next*/
		//check user-kernel copying methods
        struct secrets_list *cur_entry;
        cur_entry = list_entry(ptr, struct secrets_list, list);
		
		if (copy_to_user(secrets[i], cur_entry->secret, SECRET_MAXSIZE)) {
		//copy_to_user() failed...
			return -1;
		}

		
		stolen++;//=======================================================================================
        // check success of strcpy
        ptr = ptr->next;

		--counter;
	}
	
	return (list_len-stolen); // success | problem : if list_len is shorter than size, what do we return? //=================================
}
