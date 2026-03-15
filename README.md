## Description
Implémentation d’un **shell Unix minimaliste** en langage **C**.  
Le shell supporte l’exécution de commandes avec leurs **opérateurs logiques**, les **redirections d’entrées/sorties** et des **commandes intégrées**, à partir d’un squelette de code fourni.

## Réalisations principales
- Gestion des **commandes en avant-plan et arrière-plan (`&`)**
- Exécutions **conditionnelles (`&&`, `||`)** et **inconditionnelles (`;`)**
- Redirections **I/O** :
  - Entrée : `<`
  - Sortie : `>`, `>>`
  - Erreur : `2>&1`
  - Tubes : `|`
- Commandes intégrées :
  - `cd`
  - `exit`
  - `export`
  - `unset`
  - `pwd`
- Documentation du code via **Doxygen**

## Compétences développées
- Programmation système **bas niveau**
- Gestion des **processus Unix**
- **Analyse et extension de code existant**
