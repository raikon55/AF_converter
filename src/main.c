/*
 ============================================================================
 Name        : automata_convert.c
 Author      : Eduardo Lopes
 Version     :
 Copyright   : MIT license
 Description : FTC task
 ============================================================================
 */

#include <stdio.h>
#include "automata_convert.h"

/* TODO Descrição do trabalho
 * -> Receber uma setença e um arquivo com a descrição do AFN OK
 * -> Converter um AFN para um AFD que reconheça a mesma linguagem
 * -> Retorna um arquivo .xml com a descrição do AFD gerado
 * -> Simular o AFD gerado como PdC se foi uma conversão válida
 *
 * XXX Usar o padrão do simulador JFLAP 7.0
 */

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
    //show_automata(non_det);

    /*
     * Call the function to parse the AFN and return AFD
     */
    af_t* det = deterministic_convert(non_det);
    show_automata(det);

    /*
     * Call function to simulate AFD
     */

    return 0;
}
