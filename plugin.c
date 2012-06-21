#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned long long startr=576470088000001;
unsigned long long endr=576472224000001;

char word[9];
char charset[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*@#!$%^";
int base;

void init(void)
{
	base = strlen(charset);
}	

void i2p(unsigned long long num, char *word)
{
	int apos, bpos;
	int range[30];
	apos=0;
	if (num == 0) {
		word[0]=charset[0];
		word[1]=0;
		return;
	}
	while (num) {
		range[apos]= num % base;
		num /= base;
		apos++;
	}
	bpos=0;
	while (apos) {
		word[bpos]=charset[range[apos-1]];
		bpos++;
		apos--;
	}
	word[bpos]=0;
	return;
}


void generate()
{
	unsigned long long countr;
	countr=startr;
	while (countr <= endr) {
		i2p(countr, word);
		printf("%s\n", word);
		countr++;
	}
		
}


int main(int argc, char *argv[]) {

	startr=strtoull(argv[1],NULL,0);
	endr=strtoull(argv[2],NULL,0);
	init();
	generate();
	return 0;
}
