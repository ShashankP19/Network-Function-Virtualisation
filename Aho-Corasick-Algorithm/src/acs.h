#ifndef _AHO_CORASICK
#define _AHO_CORASICK

#define NUM_SYMBOLS 256
#define NULL_CHARACTER '\0'
#define FAILED_STATE -1

typedef struct output_node
{
	struct output_node *next;
	char *keyword;
} output_node;

typedef struct ac_dfa
{	
	//  transition table
	int **transition;
	// max state currently in table
	int num_states; 
	// Failure states
	int *failure;
	// Move Function
	int **move;

	// List of accepting states
	output_node *output;
	// The max state for the list of accepting states
	int accept_len;
} ac_dfa;


ac_dfa *initialize_matching(int num_keywords, char *keywords[]);
int perform_match(ac_dfa *dfa, char *line);
void destroy_goto(ac_dfa *);

#endif
