/*
 ============================================================================
 Name        : automata_convert.h
 Author      : Eduardo Lopes
 Version     :
 Copyright   : MIT license
 Description : FTC task
 ============================================================================
 */

#ifndef AUTOMATA_CONVERT_H_
#define AUTOMATA_CONVERT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 128UL

/**
 * Linked list node with transitions
 */
typedef struct transition transition_t;
struct transition {
  short from;
  short to;
  char symbol;
  int count;
  size_t size;
  transition_t *next;
};

/**
 * AF attributes
 */
typedef struct af {
  short start;
  short *end; // End state set
  size_t num_states;
  short **transitions; // From ... to ...
  char *transition_symbol;
  size_t num_transition;
  char *alphabet;
  size_t alphabet_size;
} af_t;

/**
 * Print a little guide to call the program
 *
 * @err: The program name
 */
void help(char *err);

/**
 * Print automata attributes
 *
 * @automata: Pointer to automata struct
 */
void show_automata(af_t *automata);

/**
 * Parse the file.jff and save each state and transition
 *
 * @stream: XML file
 * @automata: Pointer to automata struct
 */
void automata_file_parser(char *stream, af_t *automata);

/**
 * Initialize each attribute of automata with 0 (or NULL). Do it is a easily way
 * to initialize counters and pointers
 *
 * @automata: Pointer to automata struct
 */
void init_automata(af_t *automata);

/**
 * Parse the .jff file to get states of non deterministic automata
 *
 * @*file: Pointer to .jff file to read
 * @*automata: Pointer to automata struct
 */
void get_states(FILE *file, af_t *automata);

/**
 * Parse the .jff file to get transitions of non deterministic automata
 *
 * @file: Pointer to .jff file to read
 * @automata: Pointer to automata struct
 */
void get_transitions(FILE *file, af_t *automata);

/**
 * Get the symbols used in automata diagram
 *
 * @automata: Pointer to automata struct
 */
void get_alphabet(af_t *automata);

/*
 * Convert non determinism transition and save it on new automata
 *
 * @non_det: Pointer to non deterministic automata struct
 * @det: Pointer to deterministic automata struct
 */
void deterministic_convert(af_t *non_det, af_t *det);

/**
 * Push transition into a linked list
 *
 * @list: Pointer to initial node of the list
 * @transition: Array with transitions, where transiton[0] = from
 * and transition[1] = to
 * @symbol: The symbol used to reach out the transition[1]
 */
void add_transition(transition_t *list, short *transition, char symbol);

/**
 * Find and remove the non deterministic transitions
 *
 */
void remove_non_deterministic_transitions(af_t *det);

/**
 * Create and append all new deterministic transitions to
 * transition_list, and substitute a non deterministic to the
 * new states
 *
 * @automata: Pointer to automata struct
 * @transition_list: List of non deterministic transitions
 */
void create_deterministic_transitions(af_t *det, transition_t *temp_transition,
                                      transition_t *new_transitions);

/**
 *
 */
void link_transitions(af_t *automata, transition_t *transition_list);

/**
 *
 */
short has_non_deterministic_transaction(transition_t *new_automata,
                                        af_t *deterministic);

/**
 *
 */
short is_non_deterministic_transaction(transition_t *new_automata,
                                       af_t *deterministic, int position);

/**
 * Test a given sentence on deterministic automata
 *
 * @automata: Pointer to deterministic automata struct
 * @sentence: Sentence to automata test
 * @return: Final state, if sentence was accept , else -1
 */
short simulate_automata(af_t *automata, char *sentence);

/**
 * Return the next reachable state with given symbol
 *
 * @state: List of transitions
 * @symbol: List of transitions symbols
 * @size: Total size of transition array
 * @index: Actual index of transition array
 * @symbol: Symbol to parse and get next state
 */
short get_next_state(short **state, char *symbols, size_t size, int index,
                     char symbol);

/**
 * Test if state are in final set
 *
 * @state: Actual state on automata
 * @end: Final states set
 */
short is_final_state(short state, short *end);

/**
 * Create a .jff file contain the description of given automata
 *
 * @automata: Pointer to automata struct
 */
void create_automata_file(af_t *automata, char *stream);

/**
 * Free memory of automata
 *
 * @automata: The automata want to be free :)
 */
void free_af(af_t *automata);

void new_automata(af_t *automata, transition_t *temp_transition,
                  transition_t *new_transitions, short new_state);

#endif /* AUTOMATA_CONVERT_H_ */
