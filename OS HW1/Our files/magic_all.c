//new magic syscalls
#include <string.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/list.h>
#define SECRET_MAXSIZE 32

struct secrets_list {
	char secret[SECRET_MAXSIZE];
	list_t node;
};

typedef secrets_list secrets_list_t;




int max(int a, int b) {

	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}

int sys_magic_get_wand(int power, char secret[SECRET_MAXSIZE]){
	
	task_t* p = current;

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

	p->my_secret = (char*)malloc(sizeof(char)*(len)); // problem: is it 32 or 32 + 1 ? , also malloc or kmalloc?

	if (p->my_secret == NULL) {
		return -4; // fail, error in malloc
	}

	strcpy(p->my_secret, secret);
	
	if (strcmp(p->my_secret, secret)) {
		free(p->my_secret);
		return -2; //fail, error in copy
	}

	 
	/*=========== the rest of the variables ===========*/
	LIST_HEAD_INIT(p->secrets_ptr);
	p->power = power;
	p->holding_wand = 1;
	p->health = 100;

	return 0; // success
}

int sys_magic_attack(pid_t pid) {
	
	task_t* attacker = current;

	task_t* target = find_task_by_pid(pid);// found in /include/linux/sched.h


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

	list_t* iterator;

	list_for_each(iterator, target->secrets_ptr) {

		secrets_list_t* current = list_entry(iterator,secrets_list_t, node);

		if (!strcmp(current->secret, attacker->my_secret)) {
			return -5; // target already has stolen attackers secret.
		}
	};

	/*=========== assuming everything is valid ===========*/

	int dmg = attacker->power;

	int health = target->health;

	target->health = max(health - dmg, 0);

	return target->health;
}

int sys_magic_legilimens(pid_t pid) {

	task_t* attacker = current;

	task_t* target = find_task_by_pid(pid);

	/*=========== error checks ===========*/

	if (target == attacker) {
		return 0; // attacker is also target, so nothing happens.
	}

	if (target == NULL) {
		return -1; // pid doesnt exist
	}

	if (attacker->holding_wand == 0 ||
		target->holding_wand == 0) {
		return -2; // one of the processes isnt holding any wand.
	}
	/* check if attacker already has target's secret. (return -3)*/ // maybe make it a function


	//problem: a bunch of stuff
	/* add target's secret to attackers list using */
	secrets_list_t new_secret;
	new_secret.secret = target->my_secret;// maybe need to malloc this?
	//dont forgt emonem error check
	LIST_HEAD_INIT(new_secret.node); // maybe not even needed?

	list_add_tail(&new_secret.node, attacker->secrets_ptr);


	return 0; // success
}

int sys_magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size) {

	
	task_t* p = current;

	int list_len = 0;

	list_t* iterator;

	list_for_each(iterator, p->secrets_ptr) {

		list_len++;
	}

	if (size = 0) { // returns size of secrets list
		return list_len;
	}


	int counter = list_len;
	for (int i = 0; i < size; i++) {
		
		--counter;

		if (!(counter)) {

			for (int j = list_len; j < size; j++) { // fill the rest with empty strings
				strcpy(secrets[j][], "");
			}
			break;
		}
		/*copy i secret using entry + head.next*/
	}
	
	
	


	return (list_len-size); // success | problem : if list_len is shorter than size, what do we return?
}