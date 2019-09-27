/*
 * automata_convert.h
 *
 *  Created on: 30 de ago de 2019
 *      Author: hitchhiker
 */

#ifndef AUTOMATA_CONVERT_H_
#define AUTOMATA_CONVERT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TODO Retirar função help daqui */
void help(char* err);

/**
 * Register attributes of AFN
 */
typedef struct af {
    unsigned short start;
    unsigned short* end;             // End state set
    unsigned short num_states;
    unsigned short** transitions;   // From ... to ...
    unsigned char* transition_symbol;
    unsigned short num_transition;
} af_t;

void help(char* err);

/**
 * Print automata attributes
 */
void  show_automata(af_t* automata);

/**
 * Parse the file.jff and save each state and transition
 *
 * @stream: File with xml format
 *
 * Return struct contain all states and transitions read in the file.
 */
void non_deterministic_parser(char* stream, af_t* automata);

/**
 * Initialize each attribute of automata with 0 ( or NULL). Do it is a easily way
 * to initialize counters and pointers
 *
 * @automata: automata struct
 */
void init_automata(af_t* automata);

/**
 * Parse the .jff file to get states of non deterministic automata
 *
 * @*file: Pointer to .jff file to read
 * @*automata: Pointer to automata struct
 */
void get_states(FILE* file, af_t* automata);

/**
 * Parse the .jff file to get transitions of non deterministic automata
 *
 * @file: Pointer to .jff file to read
 * @automata: Pointer to automata struct
 */
void get_transitions(FILE* file, af_t* automata);

/**
 * Convert
 */
af_t* deterministic_convert(af_t* non_det);


#endif /* AUTOMATA_CONVERT_H_ */
