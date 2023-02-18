Pentru a retine id-urile taskurilor am implementat o stiva care are pe ultima pozitie un contor.
Daca stiva are un singur element in ea contorul se mareste. Daca s-au terminat taskuri,
id-ul acestora este adaugat in ordine crescatoare in stiva.
Daca stiva are mai mult de un element, pentru a oferi un id nou taskului se ia topul stivei.
Astfel, intotdeauna se va acorda cel mai mic id posibil care este unic.


ADD_TASKS

- creeaza un task care contine informatiile date si i se atribuie un id din stiva
- se parcurge coada de waiting pana cand se ajunge in locul in care trebuie inserat in functie de specificatii (prioritate, timp si id)
- se adauga taskul si se reface coada


GET_TASK

- se parcurge coada cu functiile de find si daca se gaseste id-ul dat se returneaza taskul
- se afiseaza informatiile


GET_THREAD

- se verifica daca id-ul se afla in stiva de threaduri
- daca nu il gaseste, il cauta in coada de running si afiseaza informatiile
- se reface coada


PRINTURI

- se parcurge coada si se afiseaza informatiile
- se reface coada


RUN

- se adauga taskuri din coada de waiting in cea de running pana cand nu mai sunt taskuri in waiting sau threaduri disponibile
- se calculeaza numarul de pasi care se vor face in funtie de T si Q
- intr-o bucla repetitiva:
    - se calculeaza minimul dintre T, Q si maximul de timp din coada de running actuala
    - se adauga acest minim la timpul total executat in run
    - se scade minimul din timpul dat
    - se scade minimul din timpul fiecarui task din coada de running
    - se parcurge din nou coada de running, iar daca un task s-a terminat se efectueaza urmatoarele:
        - se adauga crescator id-ul la stiva de id-uri pentru taskuri
        - se adauga taskul la coada de finished
        - se adauga id-ul threadul inapoi in stiva de id-uri pentru threaduri
    - se adauga taskuri din coada de waiting in cea de running pana cand nu mai sunt taskuri in waiting sau threaduri disponibile
    - se verifica daca coada de waiting si cea de running sunt goale sau daca s-a terminat timpul dat si se iese din functie dupa ce se dezaloca memoria
    - se repeta ciclul daca nu s-a iesit din funtie


FINISH

- functioneza dupa acelasi principiu ca si functia RUN, dar timpul T este inlocuit de timpul maxim din coada de running