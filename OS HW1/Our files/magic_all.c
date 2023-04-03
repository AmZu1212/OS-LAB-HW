//new magic syscalls
#include <string.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/list.h>
#define SECRET_MAXSIZE 32


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

	if (p->enchanted_p.holding_wand) {
		return -1; //fail, already holding wand
	}
	
	if (secret == NULL) {
		return -2; //fail, secret is NULL
	}

	if (strlen(secret) == 0) {	
		return -3; //fail, size of secret is 0
	}


	/*==========================secret checks & copy=========================*/
	
	int len = 0;

	for (; len < SECRET_MAXSIZE; len++) {

		if (secret[len] == '\0') {
			++len;
			break;
		}
	}

	
	p->enchanted_p.my_secret = (char)*malloc(sizeof(char)*len);

	if (p->enchanted_p.my_secret == NULL) {
		return -4; // fail, error in malloc
	}

	strcpy(p->enchanted_p.my_secret, secret);
	
	if (!strcmp(p->enchanted_p.my_secret, secret)) {
		free(p->enchanted_p.my_secret);
		return -2; //fail, error in copy
	}


	/*=========================rest of the fields============================*/

	LIST_HEAD_INIT(p->enchanted_p.secrets);
	p->enchanted_p.power = power;
	p->enchanted_p.holding_wand = 1;
	p->enchanted_p.health = 100;

	return 0; // success
}

int sys_magic_attack(pid_t pid) {
	
	task_t* attacker = current;
	task_t* target = find_task_by_pid(pid);// found in /include/linux/sched.h


	/*=========================checks & errors=================================*/
	if (target == NULL) {
		return -1; // pid doesnt exist
	}

	if (attacker->enchanted_p.holding_wand == 0 ||
		target->enchanted_p.holding_wand == 0) {
		return -2; // one of the processes isnt holding any wand.
	}

	if (attacker->enchanted_p.health || target->enchanted_p.health) {
		return -3; // one of the processes arrived with 0 hp.
	}

	//checking wether if "current my_secret" is already stolen or not
	int listlen = 0;
	list_t head = target->enchanted_p.secret_list;
	while (head != NULL) {

		head = head.next;

		listlen++;
	}
	
	head = target->secret_list;
	
	for (int i = 0; i < listlen; i++)
	{
		if (!strcmp(head.secret_list[i], attacker->enchanted_p.my_secret)) {
			return -5; // target already has stolen attackers secret.
		}
	}

	if (attacker == target) {
		return -5; // attacker is target
	}
	/*======================================================================*/
	
	//assuming this action is valid:

	int dmg = attacker->enchanted_p.power;
	int health = target->enchanted_p.health;
	target->enchanted_p.health = max(health - power, 0);

	return target->enchanted_p.health;
}

int sys_magic_legilimens(pid_t pid) {



}

int sys_magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size) {



}