# esercizio-2020-5-22-ping-pong



Scrivere un programma che realizza un "ping-pong" tra due processi utilizzando una coppia di pipe, una per ogni direzione. 

Il contatore (di tipo int) viene incrementato ad ogni ping ed ad ogni pong e viene trasmesso attraverso la pipe.


Quanto il contatore raggiunge il valore MAX_VALUE il programma termina.

```
proc_padre manda a proc_figlio il valore 0 attraverso pipeA.
proc_figlio riceve il valore 0, lo incrementa (=1) e lo manda a proc_padre attraverso pipeB.
proc_padre riceve il valore 1, lo incremente (=2) e lo manda a proc_figlio attraverso pipeA.
proc_figlio riceve il valore 2 .....
```
fino a MAX_VALUE, quando termina il programma.

```
#define MAX_VALUE 1000000
```
