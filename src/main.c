/*
 ============================================================================
 Name        : main.c
 Author      : Eduardo Lopes
 Version     :
 Copyright   : MIT license
 Description : FTC task
 ============================================================================
 */

#include "automata_convert.h"

int main(int argc, char* argv[]) {
    if ( argc <= 1 ) {
        help(argv[0]);
        return EXIT_FAILURE;
    }

    af_t* non_det = (af_t*) malloc(sizeof(af_t));
    init_automata(non_det);

    /*
     * Read the .xml files to receive the AFN, and put it
     * on the struct
     */
    non_deterministic_parser(argv[1], non_det);
    show_automata(non_det);
    /*
     * Call the function to parse the AFN and return AFD
     */
    af_t* det = (af_t*) malloc(sizeof(af_t));
    init_automata(det);
    deterministic_convert(non_det, det);
    show_automata(det);

    /*
     * Call function to simulate AFD
     */
    char* buffer = "01010010100";

    if ( simulate_automata(det, buffer) ) {
        puts("Sentença aceita!");
    } else {
        puts("Sentença não aceita!");
    }

    create_automata_file(det, "test.jff");

    free(non_det);
    free_af(det);

    return 0;
}
