#include "acs.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

void add_keyword(char *keyword, ac_dfa *dfa);
void construct_goto(char *keywords[], int k, ac_dfa *dfa);
void construct_failure(ac_dfa *dfa);
void construct_move(ac_dfa *dfa);

int add_state(ac_dfa *dfa, int state, char input);
void set_output(ac_dfa *dfa, int state, char *output);
ac_dfa *init_goto(void);

ac_dfa *init_goto()
{
    int i;
    ac_dfa *dfa = NULL;

    dfa = malloc(sizeof(*dfa));

    dfa->transition = malloc(sizeof(int *));
    dfa->transition[0] = calloc(NUM_SYMBOLS, sizeof(int));
    dfa->num_states = 1;

    for (i = 0; i < NUM_SYMBOLS; ++i)
        dfa->transition[0][i] = -1;

    dfa->output = NULL;
    dfa->accept_len = 0;

    return dfa;
}

ac_dfa *initialize_matching(int num_keywords, char *keywords[])
{
    ac_dfa *dfa = init_goto();

    construct_goto(keywords, num_keywords, dfa);
    construct_failure(dfa);
    construct_move(dfa);

    return dfa;
}

/**
 *  Adds the given keywords in the DFA to build a trie of the dictionary 
 */
void construct_goto(char *keywords[], int keyword_count, ac_dfa *dfa)
{
    int counter;

    // Add dictionary keywords in the trie
    for (counter = 0; counter < keyword_count; ++counter)
    {
        add_keyword(keywords[counter], dfa);
    }

    // For the start state, all failed transitions should lead to start state
    for (counter = 0; counter < NUM_SYMBOLS; ++counter)
    {
        if (dfa->transition[0][counter] == FAILED_STATE)
        {
            dfa->transition[0][counter] = 0;
        }
    }
}

/**
 * Adds the given keyword in the trie
 */
void add_keyword(char *keyword, ac_dfa *dfa)
{
    int state = 0, index = 0;

    assert(state < dfa->num_states);

    // Traverse forward in the trie till no path exists for the current input character
    while (dfa->transition[state][(int)keyword[index]] != FAILED_STATE)
    {
        assert(state < dfa->num_states);
        state = dfa->transition[state][(int)keyword[index]];
        ++index;
    }

    // Add new states for the remaining characters of the keyword
    for (; keyword[index] != NULL_CHARACTER; ++index)
    {
        state = add_state(dfa, state, keyword[index]);
    }

    set_output(dfa, state, keyword);
}

/**
 * Adds a new state for the input character for a transition from current state
 */
int add_state(ac_dfa *dfa, int state, char input)
{
    assert(state != FAILED_STATE);

    int new_state = dfa->num_states;
    ++dfa->num_states;

    dfa->transition = realloc(dfa->transition, sizeof(int *) * (dfa->num_states));
    assert(dfa->transition != NULL);

    dfa->transition[new_state] = malloc(sizeof(int) * NUM_SYMBOLS);
    assert(dfa->transition[new_state] != NULL);

    int j;
    for (j = 0; j < NUM_SYMBOLS; ++j)
    {
        dfa->transition[new_state][j] = FAILED_STATE;
    }

    // Add transition for the current state on given input character
    dfa->transition[state][(int)input] = new_state;

    return new_state;
}

void set_output(ac_dfa *dfa, int state, char *keyword)
{
    // Check if current state's number is greater than the maximum state number
    // for which output function is already defined
    if (state >= dfa->accept_len)
    {
        int i;

        // Difference between the state's number and maximum state number for
        // which output function is already defined
        int n = state - dfa->accept_len + 1;

        // Allocate memory for the n extra linked list to be added corresponding
        // to the output functions of the n extra states
        if (dfa->output == NULL)
        {
            dfa->output = malloc(n * sizeof(*dfa->output));
        }
        else
        {
            dfa->output = realloc(dfa->output, (dfa->accept_len + n) * sizeof(*dfa->output));
        }

        i = dfa->accept_len;
        dfa->accept_len += n;

        for (; i < dfa->accept_len; ++i)
        {
            dfa->output[i].next = NULL;
        }

        assert(state <= dfa->accept_len);
    }

    output_node *new_output_keyword_node;

    // Allocate memory for the node to add new output keyword
    new_output_keyword_node = (output_node *)malloc(sizeof(*new_output_keyword_node));

    // Allocate memory for new output keyword inside the new node
    new_output_keyword_node->keyword = malloc(strlen(keyword) + 1);

    // Copy the new output keyword to node's variable
    strcpy(new_output_keyword_node->keyword, keyword);

    // Add the new node to the linked list of accept[state]
    new_output_keyword_node->next = dfa->output[state].next;
    dfa->output[state].next = new_output_keyword_node;
}

void construct_failure(ac_dfa *dfa)
{
    int i;
    struct queue *q;

    assert(dfa != NULL);

    dfa->failure = malloc(dfa->num_states * sizeof(int));

    for (i = 0; i < dfa->num_states; ++i)
        dfa->failure[i] = 0;

    q = init_queue();

    for (i = 0; i < NUM_SYMBOLS; ++i)
    {
        int s = dfa->transition[0][i];

        if (s != 0)
        {
            enqueue(q, s);
            dfa->failure[s] = 0;
        }
    }

    while (!is_empty_queue(q))
    {
        int a;
        int r = dequeue(q);

        for (a = 0; a < NUM_SYMBOLS; ++a)
        {
            int s = dfa->transition[r][a];

            if (s != FAILED_STATE)
            {
                enqueue(q, s);

                int state;
                state = dfa->failure[r];
                while (dfa->transition[state][a] == FAILED_STATE)
                    state = dfa->failure[state];

                dfa->failure[s] = dfa->transition[state][a];

                // output(s) <- output(s) U output(f(s))
                output_node *t;
                t = dfa->output[dfa->failure[s]].next;
                while (t != NULL)
                {
                    set_output(dfa, s, t->keyword);
                    t = t->next;
                }
            }
        }
    }

    free_queue(q);
}

int perform_match(ac_dfa *dfa, char *inp)
{
    int state = 0;
    int match_found = 0;
    int index = 0;
    for (; *inp != NULL_CHARACTER; ++inp, ++index)
    {
        if (isascii(*inp))
        {
            state = dfa->move[state][(int)*inp];

            // Check if it matches some keyword in the current state
            if (dfa->output[state].next)
            {
                output_node *t = dfa->output[state].next;

                while (t != NULL)
                {
                    match_found = 1;
                    printf("Found Keyword: %s from %d to %d\n",
                        t->keyword, 1 + index - (int)strlen(t->keyword),index);
                    t = t->next;
                }
                // break;
            }
        }
    }

    return match_found;
}

void destroy_goto(ac_dfa *dfa)
{
    int i;

    assert(NULL != dfa);

    for (i = 0; i < dfa->num_states; ++i)
        free(dfa->transition[i]);

    free(dfa->transition);

    if (NULL != dfa->output)
    {
        int i;
        for (i = 0; i < dfa->accept_len; ++i)
        {
            if (dfa->output[i].next)
            {
                output_node *n, *t;
                for (n = dfa->output[i].next; n != NULL; n = t)
                {
                    t = n->next;
                    free(n->keyword);
                    n->keyword = NULL;
                    free(n);
                    n = NULL;
                }
                dfa->output[i].next = NULL;
            }
        }
        free(dfa->output);
        dfa->output = NULL;
    }
    if (NULL != dfa->failure)
        free(dfa->failure);
    if (NULL != dfa->move[0])
        free(dfa->move[0]);
    if (NULL != dfa->move)
        free(dfa->move);

    free(dfa);
}

void construct_move(ac_dfa *dfa)
{
    struct queue *q;
    int i, a;

    dfa->move = calloc(sizeof(int *), dfa->num_states);

    dfa->move[0] = calloc(sizeof(int), dfa->num_states * NUM_SYMBOLS);

    for (i = 0; i < dfa->num_states; ++i)
        dfa->move[i] = dfa->move[0] + i * NUM_SYMBOLS;

    q = init_queue();

    for (a = 0; a < NUM_SYMBOLS; ++a)
    {
        dfa->move[0][a] = dfa->transition[0][a];

        if (0 != dfa->transition[0][a])
            enqueue(q, dfa->transition[0][a]);
    }

    while (!is_empty_queue(q))
    {
        int r = dequeue(q);

        for (a = 0; a < NUM_SYMBOLS; ++a)
        {
            int s = dfa->transition[r][a];

            if (FAILED_STATE != s)
            {
                enqueue(q, s);
                dfa->move[r][a] = s;
            }
            else
                dfa->move[r][a] = dfa->move[dfa->failure[r]][a];
        }
    }

    free_queue(q);
}
