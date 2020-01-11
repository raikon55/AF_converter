### Conversor de AFN para AFD

Trabalho da disciplina Fundamentos teoricos da computação. O algoritmo foi baseado no livro
[Introdução à teoria da computação](https://www.estantevirtual.com.br/livros/michael-sipser/introducao-a-teoria-da-computacao/2711039229) do Michael Sipser

Abaixo o enunciado do trabalho


    Um autômato finito não determinístico (AFN) pode ser simulado a partir de
    sua conversão em um autômato finito determinístico (AFD) que reconheça a
    mesma linguagem.

    O objetivo deste trabalho é implementar um programa simule um AFN qualquer
    a partir de sua conversão para um AFD equivalente.

    Para tanto, sua implementação (utilizando a linguagem C, C++ ou Java) deve
    receber como entrada uma sentença e um arquivo contendo a descrição de um
    AFN qualquer.

    Em seguida, ele deve gerar como saída outro arquivo contendo a descrição
    do AFD equivalente e, além disso, simular o AFD equivalente para determinar
    a sentença pertence ou não à linguagem do AFN.

    Além disso, deve-se utilizar como formato para os arquivos de entrada e
    saída o mesmo padrão adotado pelo simulador JFLAP versão 7.0 que pode ser
    encontrado em http://www.jflap.org/ .

O programa atualmente faz:

- [x] Ler um arquivo .jff com o AFN
- [ ] Converter para AFD
- [x] Simular um AFD a partir de uma sentença
- [x] Salvar o AFD em um arquivo .jff
