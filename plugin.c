#include <stdio.h>

int maxlength;		// Maximum password length to try
int last;		// Last character position, zero-based
int lastid;		// Character index in the last position
int id[0x7f];		// Current character indices for other positions
int charset[0x100], c0;	// Character set
char word[8];

void init()
{
	int minlength;
	int i, c;

	minlength = 1;	// Initial password length to try, must be at least 1
	maxlength = 7;	// Must be at least same as minlength

	i = 0;
	c = ' ';			// start with space (ASCII 32) and
	while (c <= 0x7e)		// proceed for all printable ASCII
		charset[i++] = c++;

/* Zero-terminate it, and cache the first character */
	charset[i] = 0;
	c0 = charset[0];

	last = minlength - 1;
	i = 0;
	while (i <= last) {
		id[i] = 0;
		word[i++] = c0;
	}
	lastid = -1;
	word[i] = 0;
}

void generate()
{
	int i;

/* Handle the typical case specially */
	if (word[last] = charset[++lastid]) return;

	lastid = 0;
	word[i = last] = c0;
	while (i--) {			// Have a preceding position?
		if (word[i] = charset[++id[i]]) return;
		id[i] = 0;
		word[i] = c0;
	}

	if (++last < maxlength) {	// Next length?
		id[last] = lastid = 0;
		word[last] = c0;
		word[last + 1] = 0;
	} else				// We're done
		word[0] = 0;
}

void restore()
{
	int i, c;

/* Calculate the current length and infer the character indices */
	last = 0;
	while (c = word[last]) {
		i = 0; while (charset[i] != c && charset[i]) i++;
		if (!charset[i]) i = 0;	// Not found
		id[last++] = i;
	}
	lastid = id[--last];
}

void main(void) {
	unsigned long long i, j;
	unsigned long long stop=21600000000;


	init();
	generate();
//	while(word[0]){
	j=0;
	while(j < 2){
		printf("%s ", word);
		for(i=0;i < stop;i++) {
			generate();
		}
		printf("- %s\n", word);
		j++;
	}

}
