/*
 ============================================================================
 Name        : automata_convert.c
 Author      : Eduardo Lopes
 Version     :
 Copyright   : MIT license
 Description : FTC task
 ============================================================================
 */

#include "automata_convert.h"

#define MAX_BUFFER_SIZE 128UL

void help(char* err)
{
    char* str = strrchr(err, '/');
    fprintf(stdout,
    "Erro na execução\n"
    "Uso: %s arquivo.jff\n"
    "Junto a chamada do programa, passe como argumento um arquivo do programa"
    "JFLAP.\nEsse arquivo deve ser conter informações sobre um automâto não"
    "determinisco.\n", &str[1]);
}

void  show_automata(af_t* automata)
{
    printf("Initial state: %u\n"
           "Total of states: %u\n"
           "Total of transitions: %u\n",
           automata->start, automata->num_states, automata->num_transition);

    for ( int i = 0; i < automata->num_transition; i++) {
        printf("Transition %i: From %u to %u with %c\n", i,
                automata->transitions[i][0], automata->transitions[i][1],
                automata->transition_symbol[i]);
    }
}

void non_deterministic_parser(char* stream, af_t* automata)
{
    FILE* file;

    if ( (file = fopen(stream, "r")) != NULL ) {
        get_states(file, automata);
        get_transitions(file, automata);

        fclose(file);
    }
}

void init_automata(af_t* automata)
{
    automata->start = 0;
    automata->end = NULL;
    automata->num_states = 0;
    automata->num_transition = 0;
    automata->transition_symbol = NULL;
    automata->transitions = NULL;
}

void get_states(FILE* file, af_t* automata)
{
    unsigned short id_state,
                   i = 0;

    char* buffer = (char*) calloc(MAX_BUFFER_SIZE, sizeof(char)),
        * tmp;

    // List all final states
    unsigned short* final_states =
            (unsigned short*) calloc(MAX_BUFFER_SIZE, sizeof(unsigned short));

    //Get all states, and register the initial and finals states
    do {
        fgets(buffer, MAX_BUFFER_SIZE, file);

        if ( (tmp = strstr(buffer, "<state")) != NULL ) {       //Find 'state' on string read on the .xml
            sscanf(tmp, "<state id=\"%hu\" name=", &id_state);  // Get id state
            automata->num_states++;
        }

        if ( strstr(buffer, "<initial/>") != NULL ) automata->start = id_state; // Is initial state?

        if ( strstr(buffer, "<final/>") != NULL ) final_states[i++] = id_state; // Is final state?
    } while( ! strstr(buffer, "<transition>") );

    // final_states size is too big that necessary, this loop just limit it
    automata->end = (unsigned short*) calloc(automata->num_states, sizeof(unsigned short));
    for ( i = 0; i < automata->num_states; i++) automata->end[i] = final_states[i];

    free(buffer);
    free(final_states);
}

void get_transitions(FILE* file, af_t* automata)
{
    char* buffer = (char*) calloc(MAX_BUFFER_SIZE, sizeof(char));
    unsigned char symbol[MAX_BUFFER_SIZE];
    unsigned short transition[99][2];

    do {
        fgets(buffer, MAX_BUFFER_SIZE, file);

        if ( strstr(buffer, "<transition>") != NULL ) {
            fscanf(file,
                    "\t\t\t<from>%hu</from>\n\r\t\t\t<to>%hu</to>\n\r\t\t\t<read>%c</read>",
                    &transition[automata->num_transition][0],
                    &transition[automata->num_transition][1],
                    &symbol[automata->num_transition]);

            automata->num_transition++;
        }
    } while( ! feof(file) );

    automata->transition_symbol =
            (unsigned char*) calloc(automata->num_transition, sizeof(unsigned char));
    automata->transitions =
            (unsigned short**) calloc(automata->num_transition, sizeof(unsigned short*));

    for ( int i = 0; i < automata->num_transition; i++) {
        automata->transitions[i] = (unsigned short*) calloc(2, sizeof(unsigned short));
        automata->transitions[i][0] = transition[i][0];
        automata->transitions[i][1] = transition[i][1];
        automata->transition_symbol[i] = symbol[i];
    }

    free(buffer);
}

af_t* deterministic_convert(af_t* non_det)
{
    int count = 0;
    unsigned short tmp_transition[non_det->num_transition][2];
    unsigned char tmp_symbol[non_det->num_transition];

    /*
     * List non deterministic transitions
     */
    for (int j = 0; j < non_det->num_transition; j++ ) {
        for (int i = j+1; i < non_det->num_transition; i++) {
            if ( (non_det->transitions[j][0] == non_det->transitions[i][0])
              && (non_det->transition_symbol[j] == non_det->transition_symbol[i]) ) {
                // Yes, this part is horrible
                // Probably the worst code in this program
                tmp_transition[count][0] = non_det->transitions[i][0];
                tmp_transition[count][1] = non_det->transitions[i][1];
                tmp_symbol[count++] = non_det->transition_symbol[i];
                // Sorry
                tmp_transition[count][0] = non_det->transitions[j][0];
                tmp_transition[count][1] = non_det->transitions[j][1];
                tmp_symbol[count++] = non_det->transition_symbol[j];
            }
        }
    }

    /*
     * Convert non determinism transition and save it on new automata
     */
    af_t* det;
    init_automata(det);

    det->start = non_det->start;
    det->num_states = non_det->num_states;

    return det;
}
