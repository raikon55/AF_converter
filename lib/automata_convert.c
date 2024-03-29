/*
 ============================================================================
 Name        : automata_convert.c
 Author      : Eduardo Lopes
 Version     :
 Copyright   : MIT license
 Description : FTC task
 ============================================================================
 */

#include "../include/automata_convert.h"

void help(char *err) {
  char *str = strrchr(err, '/');
  fprintf(
      stdout,
      "Runtime error\n"
      "Use: %s file.jff\n"
      "Use a JFLAP file in the first argument\n"
      "The JFLAP file should contain non-deterministic automata specification\n",
      &str[1]);
}

void show_automata(af_t *automata) {
  fprintf(stdout,
          "Initial state: %i\n"
          "Total of states: %lu\n"
          "Total of transitions: %lu\n"
          "Alphabet size: %lu\n",
          automata->start, automata->num_states, automata->num_transition,
          automata->alphabet_size);

  for (size_t i = 0; i < automata->num_transition; i++) {
    fprintf(stdout, "Transition %li: From %i to %i with %c\n", i + 1,
            automata->transitions[i][0], automata->transitions[i][1],
            automata->transition_symbol[i]);
  }

  for (size_t i = 0; i < automata->alphabet_size; i++) {
    fprintf(stdout, "Symbol %li: %c\n", i + 1, automata->alphabet[i]);
  }

  int cnt = 0;
  while (automata->end[cnt] != -1) {
    fprintf(stdout, "End state %i: %i\n", cnt + 1, automata->end[cnt]);
    cnt++;
  }
}

void automata_file_parser(char *stream, af_t *automata) {
  FILE *file;

  if ((file = fopen(stream, "r")) != NULL) {
    get_states(file, automata);
    fseek(file, SEEK_SET, 0);
    get_transitions(file, automata);

    fclose(file);

    get_alphabet(automata);
  } else {
    puts("Can't open jff file");
  }
}

void init_automata(af_t *automata) {
  automata->start = 0;
  automata->end = NULL;
  automata->num_states = 0;
  automata->num_transition = 0;
  automata->transition_symbol = NULL;
  automata->transitions = NULL;
  automata->alphabet = NULL;
  automata->alphabet_size = 0;
}

void get_states(FILE *file, af_t *automata) {
  size_t id_state, i = 0;

  char *buffer = (char *)calloc(MAX_BUFFER_SIZE, sizeof(char)), *tmp;

  // List all final states
  short *final_states = (short *)calloc(MAX_BUFFER_SIZE, sizeof(short));

  // Get all states, and register the initial and finals states
  do {
    fgets(buffer, MAX_BUFFER_SIZE, file);

    if ((tmp = strstr(buffer, "<state")) != NULL) { // Find 'state' on string read on the .xml
      sscanf(tmp, "<state id=\"%lu\" name=", &id_state); // Get id state
      automata->num_states++;
    }

    if (strstr(buffer, "<initial/>") != NULL)
      automata->start = id_state; // Is initial state?

    if (strstr(buffer, "<final/>") != NULL)
      final_states[i++] = id_state; // Is final state?
  } while (!strstr(buffer, "<transition>"));

  while (i < MAX_BUFFER_SIZE)
    final_states[i++] = -1;
  // final_states size is too big that necessary, this loop just limit it
  automata->end = (short *)calloc(automata->num_states + 1, sizeof(short));
  for (i = 0; i < automata->num_states; i++)
    automata->end[i] = final_states[i];
  automata->end[i] = -1;

  free(buffer);
  free(final_states);
}

void get_transitions(FILE *file, af_t *automata) {
  char *buffer = (char *)calloc(MAX_BUFFER_SIZE, sizeof(char));
  char symbol[MAX_BUFFER_SIZE];
  short transition[MAX_BUFFER_SIZE][2];

  do {
    fgets(buffer, MAX_BUFFER_SIZE, file);

    if (strstr(buffer, "<transition>") != NULL) {
      fscanf(file,
             "\t\t\t<from>%hi</from>\n\r\t\t\t<to>%hi</to>\n\r\t\t\t<read>%c</"
             "read>",
             &transition[automata->num_transition][0],
             &transition[automata->num_transition][1],
             &symbol[automata->num_transition]);

      automata->num_transition++;
    }
  } while (!feof(file));

  automata->transition_symbol =
      (char *)calloc(automata->num_transition, sizeof(char));
  automata->transitions =
      (short **)calloc(automata->num_transition, sizeof(short *));

  for (size_t i = 0; i < automata->num_transition; i++) {
    automata->transitions[i] = (short *)calloc(2, sizeof(short));
    automata->transitions[i][0] = transition[i][0];
    automata->transitions[i][1] = transition[i][1];
    automata->transition_symbol[i] = symbol[i];
  }

  free(buffer);
}

void get_alphabet(af_t *automata) {
  char alphabet[automata->num_transition];
  size_t count = 0;

  memcpy(alphabet, automata->transition_symbol, sizeof(alphabet));

  for (size_t i = 0; i < automata->num_transition; i++) {
    for (size_t j = i + 1; j < automata->num_transition; j++) {
      if (alphabet[i] == alphabet[j]) {
        alphabet[j] = -1;
      }
    }
    if (alphabet[i] != -1) {
      count++;
    }
  }

  automata->alphabet = (char *)calloc(count, sizeof(char));
  automata->alphabet_size = count--;

  for (size_t i = 0; i < automata->num_transition; i++) {
    if (alphabet[i] != -1) {
      automata->alphabet[count] = alphabet[i];
      count--;
    }
  }
}

void deterministic_convert(af_t *non_det, af_t *det) {
  transition_t *temp_transition = (transition_t *)malloc(sizeof(transition_t));

  memcpy(det, non_det, sizeof(af_t));

  for (size_t i = 0; i < det->num_transition; i++) {
    add_transition(temp_transition, det->transitions[i],
                   det->transition_symbol[i]);
  }

  while (has_non_deterministic_transaction(temp_transition, det)) {
    remove_non_deterministic_transitions(det);

    free(temp_transition);
    temp_transition = (transition_t *)malloc(sizeof(transition_t));
    for (size_t i = 0; i < det->num_transition; i++) {
      add_transition(temp_transition, det->transitions[i],
                     det->transition_symbol[i]);
    }
  }
}

void add_transition(transition_t *list, short *transition, char symbol) {
  transition_t *new = (transition_t *)malloc(sizeof(transition_t));

  new->from = transition[0];
  new->to = transition[1];
  new->symbol = symbol;
  new->count = 0;

  new->next = list->next;
  list->next = new;

  list->size++;
}

void remove_non_deterministic_transitions(af_t *automata) {
  size_t size = automata->num_transition;
  transition_t *temp_transition = (transition_t *)malloc(sizeof(transition_t)),
               *new_transitions = (transition_t *)malloc(sizeof(transition_t));
  char temp_symbol[size];
  short new_state = automata->num_states;

  temp_transition->next = new_transitions->next = NULL;
  temp_transition->size = new_transitions->size = 0;

  for (size_t i = 0; i < automata->num_transition; i++) {
    add_transition(temp_transition, automata->transitions[i],
                   automata->transition_symbol[i]);
  }

  memcpy(temp_symbol, automata->transition_symbol, sizeof(temp_symbol));

  /*********/
  create_deterministic_transitions(automata, temp_transition, new_transitions);
  /*********/
  link_transitions(automata, new_transitions);
  /*********/
  //    new_automata(automata, temp_transition, new_transitions, new_state);
  /**
   * TODO: Break the code here
   */
  // Create a new stack without non deterministic transitions
  for (size_t i = 0; i < automata->num_transition; i++) {

    for (transition_t *tmp = temp_transition->next; tmp != NULL;
         tmp = tmp->next) {

      if (is_non_deterministic_transaction(tmp, automata, i)) {
        printf("%i\n", new_state);
        automata->transitions[i][1] = new_state;
        break;
      }
    }
    add_transition(new_transitions, automata->transitions[i],
                   automata->transition_symbol[i]);
  }

  for (size_t i = 0; i < automata->num_transition; i++) {
    free(automata->transitions[i]);
  }
  free(automata->transitions);
  free(automata->transition_symbol);

  automata->num_transition = new_transitions->size;

  automata->transitions =
      (short **)calloc(automata->num_transition, sizeof(short *));
  automata->transition_symbol =
      (char *)calloc(automata->num_transition, sizeof(char));

  int cnt = 0;
  for (transition_t *tmp = new_transitions->next; tmp != NULL;
       tmp = tmp->next) {
    automata->transitions[cnt] = (short *)calloc(2, sizeof(short));

    if (automata->transitions[cnt][0] == tmp->from &&
        automata->transition_symbol[cnt] == tmp->symbol) {
      break;
    } else {
      automata->transitions[cnt][0] = tmp->from;
      automata->transitions[cnt][1] = tmp->to;
      automata->transition_symbol[cnt] = tmp->symbol;
    }
    cnt++;
  }
}

void link_transitions(af_t *automata, transition_t *transition_list) {
  transition_t *temp_list = transition_list->next;

  while (temp_list != NULL) {

    transition_t *aux = temp_list;

    while (aux != NULL) { // Remove non deterministic

      if (temp_list != aux && temp_list->to != aux->to &&
          temp_list->symbol == aux->symbol) {
        short new[2];

        temp_list->to = new[0] = automata->num_states;
        new[1] =
            get_next_state(automata->transitions, automata->transition_symbol,
                           automata->num_transition, aux->to, aux->symbol);

        if (is_final_state(new[1], automata->end)) {
          short end[automata->num_states], k = 0;

          memcpy(end, automata->end, sizeof(end));
          free(automata->end);

          automata->end = (short *)calloc(automata->num_states, sizeof(short));
          memcpy(automata->end, end, sizeof(end));
          while (automata->end[k] != -1)
            k++;
          automata->end[k] = automata->num_states++;
          automata->end[k + 1] = -1;
        }

        add_transition(transition_list, new, aux->symbol);
      }
      aux = aux->next;
    }
    temp_list = temp_list->next;
  }
  show_automata(automata);
}

void create_deterministic_transitions(af_t *det, transition_t *temp_transition,
                                      transition_t *new_transitions) {
  transition_t *new = temp_transition->next;
  short new_state = det->num_states;
  det->num_states += 1;
  short is_non_deterministic;

  while (new != NULL) {
    size_t i = 0;
    is_non_deterministic = 0;

    // is it non deterministic transitions?
    while (i < det->num_transition && !is_non_deterministic) {
      if (is_non_deterministic_transaction(new, det, i)) {
        is_non_deterministic = 1;
      } else {
        i++;
      }
    }

    if (is_non_deterministic) {

      short non_det_transition[2];

      // Create new transition
      non_det_transition[0] = new_state;
      non_det_transition[1] = get_next_state(
          det->transitions, det->transition_symbol, det->num_transition,
          new->from, det->transition_symbol[i]);

      if (non_det_transition[1] == new->from) {
        non_det_transition[1] = -1;
      }

      for (size_t j = 0; j < det->num_states; j++) {

        if (is_non_deterministic_transaction(new, det, j)) {
          det->transitions[j][1] = new_state;

        } else if (new->to == det->transitions[j][1]) {
          det->transitions[j][1] = new_state;

        } else if (det->transitions[j][0] == non_det_transition[1]) {
          det->transitions[j][0] = new_state;
        }
      }

      if (non_det_transition[1] != -1) {
        add_transition(new_transitions, non_det_transition,
                       det->transition_symbol[i]);

        if (is_final_state(non_det_transition[1], det->end)) {
          int k = 0;
          while (det->end[k] != -1)
            k++;
          det->end[k] = new_state;
        }
      }

      if (is_final_state(new->from, det->end)) {
        int k = 0;
        while (det->end[k] != -1)
          k++;
        det->end[k] = new_state;
      }
      i++;
    }
    new = new->next;
  }
}

short has_non_deterministic_transaction(transition_t *new_automata,
                                        af_t *deterministic) {
  for (size_t i = 0; i < deterministic->num_transition; i++) {
    if (is_non_deterministic_transaction(new_automata, deterministic, i)) {
      return 1;
    }
  }
  return 0;
}

short is_non_deterministic_transaction(transition_t *new_automata,
                                       af_t *deterministic, int position) {
  if (new_automata->from == deterministic->transitions[position][0] &&
      new_automata->to != deterministic->transitions[position][1] &&
      new_automata->symbol == deterministic->transition_symbol[position]) {
    return 1;
  }

  return 0;
}

short simulate_automata(af_t *automata, char *sentence) {
  short state = automata->start, j = 0;

  while (sentence[j] != '\0') {
    state = get_next_state(automata->transitions, automata->transition_symbol,
                           automata->num_transition, state, sentence[j]);
    j++;
  }

  return is_final_state(state, automata->end);
}

short get_next_state(short **state, char *symbols, size_t size, int actual,
                     char symbol) {
  size_t i = 0, round = 0;

  while (round < 2) {
    if (symbol == symbols[i]) {
      if (actual == state[i][0]) {
        return state[i][1];
      }
    }

    if (i >= size) { // Go to initial element of array
      i = 0;
      round++;
    } else {
      i++;
    }
  }

  return (short)actual;
}

short is_final_state(short state, short *end) {
  short i = 0;

  while (state != end[i] && state != -1 && end[i++] != -1);

  if (state == end[i])
    return 1;
  else
    return 0;
}

void create_automata_file(af_t *automata, char *stream) {
  FILE *file;

  if ((file = fopen(stream, "w")) != NULL) {
    // Header
    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\" "
          "standalone=\"no\"?><!--Created with JFLAP 6.4.--><structure>\n"
          "\t<type>fa</type>\n"
          "\t<automaton>\n"
          "\t\t<!--The list of states.-->\n",
          file);

    for (size_t i = 0; i < automata->num_states; i++) {
      fprintf(file,
              "\t\t<state id=\"%li\" name=\"q%li\">\n"
              "\t\t\t<x>%3.02f</x>\n"
              "\t\t\t<y>%3.02f</y>\n",
              i, i, (float)(rand() % 200), (float)(rand() % 200));
      if (i == 0) {
        fputs("\t\t\t<initial/>\n", file);
      }
      if (is_final_state(i, automata->end)) {
        fputs("\t\t\t<final/>\n", file);
      }
      fputs("\t\t</state>\n", file);
    }

    fputs("\t\t<!--The list of transitions.-->\n", file);
    for (size_t i = 0; i < automata->num_transition; i++) {
      fprintf(file,
              "\t\t<transition>\n"
              "\t\t\t<from>%i</from>\n"
              "\t\t\t<to>%i</to>\n"
              "\t\t\t<read>%c</read>\n"
              "\t\t</transition>\n",
              automata->transitions[i][0], automata->transitions[i][1],
              automata->transition_symbol[i]);
    }

    fputs("\t</automaton>\n"
          "</structure>",
          file),
        fclose(file);

  } else {
    puts("Can't open the jff file");
  }
}

void free_af(af_t *automata) {
  free(automata->end);
  free(automata->transition_symbol);
  for (size_t i = 0; i < automata->num_transition; i++) {
    free(automata->transitions[i]);
  }
  free(automata->transitions);
  free(automata->alphabet);
  free(automata);
}

void new_automata(af_t *automata, transition_t *temp_transition,
                  transition_t *new_transitions, short new_state) {
  // Create a new stack without non deterministic transitions
  for (size_t i = 0; i < automata->num_transition; i++) {

    for (transition_t *tmp = temp_transition->next; tmp != NULL;
         tmp = tmp->next) {

      if (is_non_deterministic_transaction(tmp, automata, i)) {
        automata->transitions[i][1] = new_state;
        break;
      }
    }
    add_transition(new_transitions, automata->transitions[i],
                   automata->transition_symbol[i]);
  }

  for (size_t i = 0; i < automata->num_transition; i++) {
    free(automata->transitions[i]);
  }
  free(automata->transitions);
  free(automata->transition_symbol);

  automata->num_transition = new_transitions->size;

  automata->transitions =
      (short **)calloc(automata->num_transition, sizeof(short *));
  automata->transition_symbol =
      (char *)calloc(automata->num_transition, sizeof(char));

  int cnt = 0;
  for (transition_t *tmp = new_transitions->next; tmp != NULL;
       tmp = tmp->next) {
    automata->transitions[cnt] = (short *)calloc(2, sizeof(short));

    if (automata->transitions[cnt][0] == tmp->from &&
        automata->transition_symbol[cnt] == tmp->symbol) {
      break;
    } else {
      automata->transitions[cnt][0] = tmp->from;
      automata->transitions[cnt][1] = tmp->to;
      automata->transition_symbol[cnt] = tmp->symbol;
    }
    cnt++;
  }
}
