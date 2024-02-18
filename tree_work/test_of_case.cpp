#include <stdio.h>

int main()
{
	int variable=-2;

	switch(variable)
	{
		case 0:
			printf("in the zero case.\n");
			variable=1;
			break;//this step will skip out all judgemnt.
		case 1:
			printf("in the ONE case.\n");
			variable=-1;
			break;
		case -2:
		case -1:
			printf("in the minus ONE case.\n");
			variable=0;
			break;
	}
	return 0;
}