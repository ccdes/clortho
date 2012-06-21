#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned long long startr=576470088000001;
unsigned long long endr=576472224000001;

char word[8];
char charset[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*@#!$%^";
int base;
int leng = 8;

void init(void)
{
	base = strlen(charset);
}	

void generate()
{
	int apos;
	unsigned long long count;
	count=startr;
	apos=leng;
	word[--apos]=0;
	while (count) {
		word[apos]=charset[ count % base];
		count /= base;
		apos--;
	}
	while (apos + 1) {
		word[apos]=charset[0];
		apos--;
	}
	startr++;
		
}

int main(int argc, char *argv[]) {

	leng=atoi(argv[1]);
	startr=strtoull(argv[2],NULL,0);
	endr=strtoull(argv[3],NULL,0);
	init();
	while (startr <= endr) {
		generate();
		printf("%s\n",  word);
	}
	return 0;
}
