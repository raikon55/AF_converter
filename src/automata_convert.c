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

void help(char* err)
{
    char* str = strrchr(err, '/');
    fprintf(stdout,
    "Erro na execução\n"
    "Uso: %s arquivo.jff\n"
    "Junto a chamada do programa, passe como argumento um arquivo do programa"
    "JFLAP.\nEsse arquivo deve ser conter informações sobre um automâto não "
    "determinisco.\n", &str[1]);
}

void show_automata(af_t* automata)
{
    fprintf(stdout, "Initial state: %i\n"
           "Total of states: %lu\n"
           "Total of transitions: %lu\n"
           "Alphabet size: %lu\n",
           automata->start, automata->num_states, automata->num_transition,
           automata->alphabet_size);

    for ( int i = 0; i < automata->num_transition; i++) {
        fprintf(stdout, "Transition %i: From %i to %i with %c\n", i+1,
                automata->transitions[i][0], automata->transitions[i][1],
                automata->transition_symbol[i]);
    }

    for ( int i = 0; i < automata->alphabet_size; i++ ) {
        fprintf(stdout, "Symbol %i: %c\n", i+1, automata->alphabet[i]);
    }

    int cnt = 0;
    while ( automata->end[cnt] != -1 ) {
        fprintf(stdout, "End state %i: %i\n", cnt+1, automata->end[cnt]);
        cnt++;
    }
}

void non_deterministic_parser(char* stream, af_t* automata)
{
    FILE* file;

    if ( (file = fopen(stream, "r")) != NULL ) {
        get_states(file, automata);
        fseek(file, SEEK_SET, 0);
        get_transitions(file, automata);

        fclose(file);

        get_alphabet(automata);
    } else {
        puts("Can't open the jff file");
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
    automata->alphabet = NULL;
    automata->alphabet_size = 0;
}

void get_states(FILE* file, af_t* automata)
{
    short id_state, i = 0;

    char* buffer = (char*) calloc(MAX_BUFFER_SIZE, sizeof(char)),
        * tmp;

    // List all final states
    short* final_states =
            (short*) calloc(MAX_BUFFER_SIZE, sizeof(short));

    //Get all states, and register the initial and finals states
    do {
        fgets(buffer, MAX_BUFFER_SIZE, file);

        if ( (tmp = strstr(buffer, "<state")) != NULL ) {       //Find 'state' on string read on the .xml
            sscanf(tmp, "<state id=\"%hi\" name=", &id_state);  // Get id state
            automata->num_states++;
        }

        if ( strstr(buffer, "<initial/>") != NULL ) automata->start = id_state; // Is initial state?

        if ( strstr(buffer, "<final/>") != NULL ) final_states[i++] = id_state; // Is final state?
    } while( ! strstr(buffer, "<transition>") );

    while ( i < MAX_BUFFER_SIZE ) final_states[i++] = -1;
    // final_states size is too big that necessary, this loop just limit it
    automata->end = (short*) calloc(automata->num_states+1, sizeof(short));
    for ( i = 0; i < automata->num_states; i++) automata->end[i] = final_states[i];
    automata->end[i] = -1;

    free(buffer);
    free(final_states);
}

void get_transitions(FILE* file, af_t* automata)
{
    char* buffer = (char*) calloc(MAX_BUFFER_SIZE, sizeof(char));
    char symbol[MAX_BUFFER_SIZE];
    short transition[MAX_BUFFER_SIZE][2];

    do {
        fgets(buffer, MAX_BUFFER_SIZE, file);

        if ( strstr(buffer, "<transition>") != NULL ) {
            fscanf(file,
                    "\t\t\t<from>%hi</from>\n\r\t\t\t<to>%hi</to>\n\r\t\t\t<read>%c</read>",
                    &transition[automata->num_transition][0],
                    &transition[automata->num_transition][1],
                    &symbol[automata->num_transition]);

            automata->num_transition++;
        }
    } while( ! feof(file) );

    automata->transition_symbol =
            (char*) calloc(automata->num_transition, sizeof(char));
    automata->transitions =
            (short**) calloc(automata->num_transition, sizeof(short*));

    for ( int i = 0; i < automata->num_transition; i++) {
        automata->transitions[i] = (short*) calloc(2, sizeof(short));
        automata->transitions[i][0] = transition[i][0];
        automata->transitions[i][1] = transition[i][1];
        automata->transition_symbol[i] = symbol[i];
    }

    free(buffer);
}

void get_alphabet(af_t* automata)
{
    char alphabet[automata->num_transition];
    size_t count = 0;

    memcpy(alphabet, automata->transition_symbol, sizeof(alphabet));

    for (int i = 0; i < automata->num_transition; i++ ) {
        for (int j = i+1; j < automata->num_transition; j++) {
            if ( alphabet[i] == alphabet[j] ) {
                alphabet[j] = -1;
            }
        }
        if ( alphabet[i] != -1 ) {
            count++;
        }
    }

    automata->alphabet = (char*) calloc(count, sizeof(char));
    automata->alphabet_size = count--;

    for ( int i = 0; i < automata->num_transition; i++) {
        if ( alphabet[i] != -1 ) {
            automata->alphabet[count] = alphabet[i];
            count--;
        }
    }
}

void deterministic_convert(af_t* non_det, af_t* det)
{
    memcpy(det, non_det, sizeof(af_t));
    remove_non_deterministic_transitions(det);
}

void add_transition(transition_t* list, short* transition, char symbol)
{
    transition_t* new = (transition_t*) malloc(sizeof(transition_t));

    new->from = transition[0];
    new->to = transition[1];
    new->symbol = symbol;
    new->count = 0;
    new->next = NULL;

    new->next = list->next;
    list->next = new;

    list->size++;
}

void remove_non_deterministic_transitions(af_t* automata)
{
    size_t size = automata->num_transition;
    transition_t* temp_transition = (transition_t*) malloc(sizeof(transition_t)),
                * new_transitions = (transition_t*) malloc(sizeof(transition_t));
    char temp_symbol[size];
    short new_state = automata->num_states++;

    temp_transition->next = new_transitions->next = NULL;
    temp_transition->size = new_transitions->size = 0;

    for ( int i = 0; i < automata->num_transition; i++) {
        add_transition(temp_transition, automata->transitions[i],
                automata->transition_symbol[i]);
    }

    memcpy(temp_symbol, automata->transition_symbol, sizeof(temp_symbol));

    /*********/
    create_deterministic_transitions(automata, temp_transition, new_transitions);
    /*********/
    link_transitions(automata, new_transitions);
    /*********/

    /**
     * TODO: Break the code here
     */
    // Create a new stack without non deterministic transitions
    for ( int i = 0; i < automata->num_transition; i++) {

        for (transition_t* tmp = temp_transition->next; tmp != NULL; tmp = tmp->next) {

            if ( tmp->from == automata->transitions[i][0]
              && tmp->to != automata->transitions[i][1]
              && tmp->symbol == automata->transition_symbol[i] ) {
                automata->transitions[i][1] = new_state;
                break;
            }

        }
        add_transition(new_transitions, automata->transitions[i],
                automata->transition_symbol[i]);
    }

    for ( int i = 0; i < automata->num_transition; i++ ) {
        free(automata->transitions[i]);
    }
    free(automata->transitions);
    free(automata->transition_symbol);

    automata->num_transition = new_transitions->size;

    automata->transitions = (short**)
            calloc(automata->num_transition, sizeof(short*));
    automata->transition_symbol = (char*)
            calloc(automata->num_transition, sizeof(char));

    int cnt = 0;
    for (transition_t* tmp = new_transitions->next; tmp != NULL; tmp = tmp->next) {
        automata->transitions[cnt] = (short*) calloc(2, sizeof(short));

        if ( automata->transitions[cnt][0] == tmp->from
          && automata->transition_symbol[cnt] == tmp->symbol ) {
            break;
        } else {
            automata->transitions[cnt][0] = tmp->from;
            automata->transitions[cnt][1] = tmp->to;
            automata->transition_symbol[cnt] = tmp->symbol;
        }
        cnt++;
    }
}

void link_transitions(af_t* automata, transition_t* transition_list)
{
    transition_t* temp_list = transition_list->next;

    while ( temp_list != NULL ) {

        transition_t* aux = temp_list;

        while ( aux != NULL ) { // Remove non deterministic

            if ( temp_list != aux && temp_list->to != aux->to
              && temp_list->symbol == aux->symbol ) {
                short new[2];

                temp_list->to = new[0] = automata->num_states;
                new[1] = get_next_state(automata->transitions,
                        automata->transition_symbol, automata->num_transition,
                        aux->to, aux->symbol);

                if ( is_final_state(new[1], automata->end) ) {
                    short end[automata->num_states], k = 0;

                    memcpy(end, automata->end, sizeof(end) );
                    free(automata->end);

                    automata->end = (short*) calloc(automata->num_states,
                            sizeof(short));
                    memcpy(automata->end, end, sizeof(end));
                    while ( automata->end[k] != -1 ) k++;
                    automata->end[k] = automata->num_states++;
                    automata->end[k+1] = -1;
                }

                add_transition(transition_list, new, aux->symbol);
            }
            aux = aux->next;
        }
        temp_list = temp_list->next;
    }
    show_automata(automata);
}

void create_deterministic_transitions(af_t* det, transition_t* temp_transition,
        transition_t* new_transitions)
{
    transition_t* new = temp_transition->next;
    short new_state = det->num_states++;
    short is_non_deterministic;

    while (new != NULL) {
         int i = 0;
         is_non_deterministic = 0;

         // is it non deterministic transitions?
         while ( i < det->num_transition && ! is_non_deterministic ) {
             if ( new->from == det->transitions[i][0]
               && new->to != det->transitions[i][1]
               && new->symbol == det->transition_symbol[i]) {
                 is_non_deterministic = 1;
             } else {
                 i++;
             }
         }

         if ( is_non_deterministic ) {

             short non_det_transition[2];

             // Create new transition
             non_det_transition[0] = new_state;
             non_det_transition[1] = get_next_state(det->transitions,
                     det->transition_symbol, det->num_transition, new->from,
                     det->transition_symbol[i]);

             int k = 0;
             do {

                 if ( new->from == det->transitions[k][0]
                   && new->to != det->transitions[k][1]
                   && new->symbol == det->transition_symbol[k]) {
                     det->transitions[k][1] = new_state;
                 } else if ( new->to == det->transitions[k][1] ) {
                     det->transitions[k][1] = new_state;
                 }

             } while ( k++ < det->num_states );

             if ( non_det_transition[1] != -1 ) {
                 add_transition(new_transitions, non_det_transition,
                         det->transition_symbol[i]);

                 if ( is_final_state(non_det_transition[1], det->end) ) {
                     int k = 0;
                     while ( det->end[k] != -1 ) k++;
                     det->end[k] = new_state;
                 }
             }

             i++;
        }
         new = new->next;
    }
    for ( transition_t* temp = new_transitions->next; temp != NULL; temp = temp->next )
        printf("From %i to %i : %c\n", temp->from, temp->to, temp->symbol);
}

short simulate_automata(af_t* automata, char* sentence)
{
    short state = automata->start, j = 0;

    do {
        state = get_next_state(automata->transitions, automata->transition_symbol,
                automata->num_transition, state, sentence[j]);
    } while ( sentence[++j] != '\0' );

    return is_final_state(state, automata->end);

}

short get_next_state(short** state, char* symbols, size_t size, int actual, char symbol)
{
    short i = 0, round = 0, found = 0;

    while ( round < 2 ) {
        if ( symbols[i] == symbol ) {
            if ( actual == state[i][0] ) {
                found = 1;
                break;
            }
        }

        if ( i >= size ) { // Go to initial element of array
            i = 0;
            round++;
        } else {
            i++;
        }
    }

    return found ? state[i][1] : -1;
}

short is_final_state(short state, short* end)
{
    short i = 0;

    while ( state != end[i] && state != -1 && end[i++] != -1 );

    if (state == end[i]) return 1;
    else return 0;
}

void create_automata_file(af_t* automata, char* stream)
{
    FILE* file;

    if ( (file = fopen(stream, "w")) != NULL ) {
        // Header
        fputs("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><!--Created with JFLAP 6.4.--><structure>\n"
              "\t<type>fa</type>\n"
              "\t<automaton>\n"
              "\t\t<!--The list of states.-->\n", file);

        for ( int i = 0; i < automata->num_states; i++) {
            fprintf(file, "\t\t<state id=\"%i\" name=\"q%i\">\n"
                          "\t\t\t<x>%3.02f</x>\n"
                          "\t\t\t<y>%3.02f</y>\n",
                   i, i, (float) ( rand() % 200) , (float) ( rand() % 200) );
            if ( i == 0 ) {
                fputs("\t\t\t<initial/>\n", file);
            }
            if ( is_final_state(i, automata->end) ) {
                fputs("\t\t\t<final/>\n", file);
            }
            fputs("\t\t</state>\n", file);
        }

        fputs("\t\t<!--The list of transitions.-->\n", file);
        for ( int i = 0; i < automata->num_transition; i++ ) {
            fprintf(file, "\t\t<transition>\n"
                          "\t\t\t<from>%i</from>\n"
                          "\t\t\t<to>%i</to>\n"
                          "\t\t\t<read>%c</read>\n"
                          "\t\t</transition>\n",
                    automata->transitions[i][0], automata->transitions[i][1],
                    automata->transition_symbol[i]);
        }

        fputs("\t</automaton>\n"
              "</structure>", file),
        fclose(file);

    } else {
        puts("Can't open the jff file");
    }

}

void free_af(af_t* automata)
{
    free(automata->end);
    free(automata->transition_symbol);
    for ( int i = 0; i < automata->num_transition; i++)
        free(automata->transitions[i]);
    free(automata->transitions);
    free(automata->alphabet);
    free(automata);
}
