# Documentació dels jocs de prova del projecte *spaceships*

Aquesta és la documentació dels jocs de prova. Explica què prova cada un i com configurar-lo.

## Informació general
 
Els jocs de prova es troben al fitxer carpeta tests/jplib.c. Es basen en dues funcions principals *jp_all* i *jp_rank*

**Com uso els jocs de prova?**
Cal incloure una de les dues funcions al codi d'*user.c* en funció del que desitjem:
*jp_all*: Permet efectuar tots els jocs de prova.
*jp_rank(int inici, int final)*: Permet efectuar els jocs de prova des del joc *inici* al joc *final*.

## Descripció dels jocs de prova
Jocs organitzats per nivells de l'enunciat:

### Nivell 1
**1. Prova de tecles i escenaris:** Aquest test implementa un petit joc per mostrar el correcte funcionament del teclat i les mostres en pantalla. Permet moure una @ (arroba) amb les tecles vi ('h' esquerra, 'j' avall, 'k' amunt i 'l' dreta). 

**2. Prova de buffer de teclat:** Aquest test pretén provar que el buffer circular funciona correctament. Per això demanarà a l'usuari que entri el seguit de tecles q w e r t y u i o p (corresponent a la primera fila de lletres del teclat) i esperarà la sortida o p e r t y u i en la que les dues primeres lletres has sigut sobreescrites pel funcionament inherent del buffer. El tamany per defecte del buffer és de 8 caràcters.

### Nivell 2
No s'han implementat encara

### Nivell 3
**3. Prova de màxim d'fps:** Aquest test vol demostrar la velocitat màxima que poden assolir els fps. Aquesta es mostrarà per pantalla un cop efectuat el càlcul. El resultat sempre és correcte.

**4. Prova de la garantia al regular els fps:** El procés efectuarà el mateix càlcul que el joc anterior però en aquest cas compartirà la cpu amb el nombre de processos especificat per la constant CHILS que definirà l'usuari que vulgui fer el test. El test és vàlid si el nombre d'fps calculat és igual al nombre d'fps que l'usuari vol garantir. Aquest nombre d'fps a garantir pot ser modificat canviant la constant FPS. Per a un test correcte aquest valor no hauria de ser major a 223. 
