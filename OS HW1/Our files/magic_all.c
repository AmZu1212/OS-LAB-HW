//new magic syscalls
#include <string.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/list.h>
#define SECRET_MAXSIZE 32

int sys_magic_get_wand(int power, char secret[SECRET_MAXSIZE]){
	
	task_struct* p = current;

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
		free(p->enchanted.my_secret);
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
	


}

int sys_magic_legilimens(pid_t pid) {



}

int sys_magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size) {



}