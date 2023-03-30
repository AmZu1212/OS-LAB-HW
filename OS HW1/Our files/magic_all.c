//new magic syscalls
#include <string.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/list.h>
char null_character = 0; // ???
int sys_magic_get_wand(int power, char secret[SECRET_MAXSIZE]){
	
	task_struct* p = current;

	if (p.cur_enchanted_process.holding_wand) {
		return -1; //fail, already holding wand
	}
	
	if (secret == NULL) {
		return -2; //fail, secret is NULL
	}

	if (strlen(secret) == 0) {	
		return -3; //fail, size of secret is 0
	}
	
	//need to check secret copy
	/*
	p.cur_enchanted_process.my_secret = (char)malloc(sizeof(char) * strlen(secret));
	
	if (p.cur_enchanted_process.my_secret == NULL) {
		return -4; //fail, error in malloc
	}

	strcpy(p.cur_enchanted_process.my_secret,secret);
	
	if (!strcmp(p.cur_enchanted_process.my_secret, secret)) {
		return -2; //fail, error in copy
	}
	*/

	p.cur_enchanted_process.power = power;
	p.cur_enchanted_process.holding_wand = 1;
	p.cur_enchanted_process.health = 100;
	LIST_HEAD_INIT(p.cur_enchanted_process.secrets); // need to ask how does it work



	return 0; // success
}

int sys_magic_attack(pid_t pid) {
	
}

int sys_magic_legilimens(pid_t pid) {



}

int sys_magic_list_secrets(char secrets[][SECRET_MAXSIZE], size_t size) {



}