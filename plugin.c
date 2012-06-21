#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned long long startr=576470088000001;
unsigned long long endr=576472224000001;

char word[8];
char charset[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789*@#!$%^";
int base;
int leng;

void init(void)
{
	base = strlen(charset);
	leng = 1;
}	

void generate()
{
	int apos;
	unsigned long long count;
	if (startr == 0) {
		word[0]=charset[0];
		word[1]=0;
		startr++;
		return;
	}
	expand:
		count=startr;
		apos=leng;
		word[--apos]=0;
		while (count) {
			if (apos < 0) {
				leng++;
				goto expand;
			}
			word[apos]=charset[ count % base];
			count /= base;
			apos--;
		}
	startr++;
		
}

int main(int argc, char *argv[]) {

	startr=strtoull(argv[1],NULL,0);
	endr=strtoull(argv[2],NULL,0);
	init();
	while (startr <= endr) {
		generate();
		printf("%s\n", word);
	}
	return 0;
}
