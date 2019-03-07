#include <stdio.h>
#include <string.h>
#include "../src/acs.h"

int main(int ac, char **av)
{
	struct ac_dfa *dfa;
	char buf[BUFSIZ];

	if (1 == ac)
		return 1;

	dfa = initialize_matching(ac - 1, &av[1]);
	printf("Enter input string: ");

	while (fgets(buf, sizeof(buf), stdin))
	{
		buf[strlen(buf) - 1] = '\0';
		if (!perform_match(dfa, buf))
			printf("======No Match Found=====\n");
	}

	destroy_goto(dfa);

	return 0;
}