#include <stdio.h>
#include <string.h>
#include <strings.h>

int main(int a, char **v)
{

	/* v[1] has the string in it ... */
	char *buf=v[1];
	int i=0;
	int slashes=0;

	for(i=0; i < strlen(v[1]); i++)
	{
		printf(" char number %i is %c\n", i, buf[i]);
		if (buf[i] == ' ') { printf(" space at i=%d\n",i); }
		if (buf[i] == '\/'){ slashes++; }
		if (slashes == 2) {
		  printf(" index %d we saw // maybe server follows here",i);
		}
	}

}
