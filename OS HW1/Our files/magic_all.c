//new magic syscalls
#include <string.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/list.h>
#define SECRET_MAXSIZE 32

struct secrets_list {
	list_t node;
	string secret[SECRET_MAXSIZE];
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

	p->my_secret = (char)*malloc(sizeof(char)*len);

	if (p->my_secret == NULL) {
		return -4; // fail, error in malloc
	}

	strcpy(p->my_secret, secret);
	
	if (!strcmp(p->my_secret, secret)) {
		free(p->my_secret);
		return -2; //fail, error in copy
	}


	/*=========== the rest of teh variables ===========*/
	LIST_HEAD_INIT(p->secret_ptr);
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


	/*=========== check for if my_secret is already stolen ===========*/
	
	//this part isnt 100% right need to polish it
	int targetlistlen = 0;
	list_t head = target->stolen_secrets.node;
	while (head != NULL) {

		head = head.next;

		targetlistlen++;
	}
	
	head = target->stolen_secrets.node;
	
	secrets_list_t target_list;

	target_list = target->stolen_secrets;

	for (int i = 0; i < listlen; i++)
	{
		if (!strcmp(target->stolen_secrets.secret, attacker->my_secret)) {
			return -5; // target already has stolen attackers secret.
		}
	}

	if (attacker == target) {
		return -5; // attacker is target
	}
	//until here

	/*=========== assuming everything is valid ===========*/
	int dmg = attacker->enchanted_p.power;

	int health = target->enchanted_p.health;

	target->enchanted_p.health = max(health - power, 0);

	return target->enchanted_p.health;
}

int sys_magic_legilimens(pid_t pid) {



}

int sys_magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size) {



}