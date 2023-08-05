# Elaborato system call
**Laboratorio di Sistemi Operativi**<br/>
**II anno**<br/>
**Università degli Studi di Verona**<br/>
**A.A. 2022/2023**
<br/><br/>
Simulazione del gioco Forza 4 implementata tramite system call SystemV in ambiente Unix/Linux.<br/>
Per giocare è necessario avviare un'istanza di F4Server e poi aprire due istanze di F4Client da cui i giocatori possono sfidarsi (una sola in caso di autoplay).<br/>
``./F4Server [righe] [colonne]`` ``./F4Client [nome giocatore]``<br/>
Una volta avviata la partita, ad ogni il giocatore deve selezionare la colonna dove posizionare la propria pedina entro 30 secondi, dopodiché attenderà la mossa avversaria.<br/>
Il gioco termina quando uno dei due giocatori posiziona 4 pedine consecutive in una qualsiasi direzione.<br/>
Maggiori dettagli implementativi si possono trovare nel file [Relazione.pdf](https://github.com/Barba02/F4/blob/main/Relazione.pdf).
<br/><br/><br/>
_Filippo Barbieri<br/>
Alessio Brighenti<br/>
Islam Taouri_