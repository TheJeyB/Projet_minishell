/** @file parser.c
 * @brief Implementation of command line parsing
 * @author ROMDHANE Iskander
 * @author HADJADJ Mohammed Hadjaissa
 * @date 2025-26
 * @details Implémentation des fonctions d'analyse des lignes de commande.
 */

#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

#include "parser.h"
#include "processus.h"

/** @brief Fonction de suppression des espaces inutiles au début et à la fin d'une chaîne de caractères.
 * @param str Chaîne de caractères à traiter.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 */
int trim(char* str) {
    int i = 0;

	while (str[i] == ' ') {
		i++;
	}
	memmove(str , str+i , strlen(str + i) + 1);
	int f = strlen(str) - 1;
	while (f >= 0 && str[f] == ' ') f--;
	str[f+1] = '\0';

	printf("nouveau chaine sans espace : %s \n" , str);
	return 0;
}

/** @brief Fonction de nettoyage d'une chaîne de caractères en supprimant les doublons d'espaces.
 * @param str Chaîne de caractères à nettoyer.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 */
int clean(char* str) {
 char* s = str;
	while((s = strstr( s , "  "))){
		memmove(s , s + 1 , strlen(s + 1) +1);
	}
	printf("nouveau chaine sans double espace :%s \n" , str);
	return 0 ;
}

/** @brief Fonction d'ajout de caractères d'espacement autour de tous les caractères de la chaîne *s* présents dans *str*.
 * @param str Chaîne de caractères à traiter.
 * @param s Chaîne de caractères contenant les séparateurs.
 * @param max Taille maximale de la chaîne *str*.
 * @return int 0 en cas de succès, -1 en cas d'erreur (dépassement de taille).
 * @details Cette fonction ajoute un espace avant et après chaque occurrence d'un caractère de *s* dans *str*.
 *    Si l'ajout d'espaces dépasse la taille maximale *max*, la fonction retourne -1.
 */
int separate_s(char* str, char* s, size_t max) {
char find[2] = { s[0], '\0' };
    char *ch = str;
    while ((ch = strstr(ch, find))) {
        size_t len = strlen(str);
        if (len + 2 >= max) break;

        if (ch == str) {
            if (*(ch + 1) != ' ') {
                memmove(ch + 2, ch + 1, len - (ch - str));
                *(ch + 1) = ' ';
            }
            ch += 2;
        }

        else if (*(ch + 1) == '\0') {
            if (*(ch - 1) != ' ') {
                memmove(ch + 1, ch, len - (ch - str) + 1);
                *(ch - 1) = ' ';
                ch++;
            }
        }

        else {
            if (*(ch - 1) != ' ') {
                memmove(ch + 1, ch, len - (ch - str) + 1);
                *ch = ' ';
                ch++;
                len++;
            } else {
                ch++;
            }

            if (*(ch + 1) != ' ') {
                memmove(ch + 2, ch + 1, len - (ch - str));
                *(ch + 1) = ' ';
                ch += 2;
                len++;
            } else {
                ch++;
            }
        }
    }

    printf("str seperate 1 :%s\n", str);
    return 0;
}

/** @brief Fonction de remplacement de toutes les occurrences de la sous-chaîne *s* par la sous-chaîne *t* dans la chaîne *str*.
 * @param str Chaîne de caractères à traiter.
 * @param s Sous-chaîne à remplacer.
 * @param t Sous-chaîne de remplacement.
 * @param max Taille maximale de la chaîne *str*.
 * @return int 0 en cas de succès, -1 en cas d'erreur (dépassement de taille).
 * @details Cette fonction remplace toutes les occurrences de la sous-chaîne *s* par la sous-chaîne *t* dans la chaîne *str*.
 *    Si le remplacement dépasse la taille maximale *max*, la fonction retourne -1.
 */
int replace(char* str, const char* s, const char* t, size_t max) {
char* pos = strstr(str , s);

	char* src = pos + strlen(s);
	char* dst = pos + strlen(t);
	int nb = strlen(src) + 1;
	int new_size = strlen(str) - strlen(s) + strlen(t);
	if (new_size > max) {
		nb = nb - (max - new_size);
 	        return -1;

	}

	int nb_cp = strlen(t);
	if( pos + strlen(t) > str + max) {
		int diff = (pos + strlen(t)) - (str + max +1);
		if (diff < 0) diff=0;
		nb_cp -= diff;
	}
	memmove(dst , src , nb);
	memcpy(pos , t , nb_cp);
	  return 0;
}

/** @brief Fonction de substitution des variables d'environnement dans une chaîne de caractères.
 * @param str Chaîne de caractères à traiter.
 * @param max Taille maximale de la chaîne *str*.
 * @return int 0 en cas de succès, -1 en cas d'erreur (dépassement de taille).
 * @details Cette fonction remplace toutes les occurrences de variables d'environnement au format $VAR ou ${VAR} par leur valeur dans la chaîne *str*.
 *    Si une variable n'existe pas, elle est remplacée par une chaîne vide.
 *    Si le remplacement dépasse la taille maximale *max*, la fonction retourne -1.
 */
int substenv(char* str, size_t max) {
    if (!str || max == 0)
        return -1;

    char buffer[max];
    size_t i = 0;
    size_t j = 0;

    while (str[i] != '\0' && j < max - 1) {
        if (str[i] != '$') {
            buffer[j++] = str[i++];
            continue;
        }

        i++;

        if (str[i] == '\0') {
            buffer[j++] = '$';
            break;
        }

        char varname[256];
        size_t k = 0;

        if (str[i] == '{') {
            i++;
            while (str[i] != '\0' && str[i] != '}' && k < sizeof(varname) - 1) {
                varname[k++] = str[i++];
            }
            varname[k] = '\0';

            if (str[i] != '}') {
                buffer[j++] = '$';
                i -= (k + 1);
                continue;
            }

            i++;

        } else {
            while (isalnum((unsigned char)str[i]) || str[i] == '_') {
                if (k < sizeof(varname) - 1) {
                    varname[k++] = str[i];
                }
                i++;
            }
            varname[k] = '\0';
        }

        if (k == 0) {
            buffer[j++] = '$';
            continue;
        }

        const char* value = getenv(varname);

        if (value) {
            while (*value != '\0' && j < max - 1) {
                buffer[j++] = *value++;
            }
        }
    }

    buffer[j] = '\0';
    strncpy(str, buffer, max);
    str[max - 1] = '\0';

    return 0;
}

/** @brief Fonction de découpage d'une chaîne de caractères en tokens selon un séparateur.
 * @param str Chaîne de caractères à découper. Attention, cette chaîne est modifiée par la fonction.
 * @param sep Caractère séparateur.
 * @param tokens Tableau de chaînes de caractères pour stocker les tokens extraits. Le tableau est terminé par un pointeur NULL.
 * @param max Taille maximale du tableau *tokens*.
 * @return int Nombre de tokens extraits, -1 en cas d'erreur (dépassement de taille).
 * @details Cette fonction découpe la chaîne *str* en tokens en utilisant le caractère *sep* comme séparateur.
 *    Les tokens extraits sont stockés dans le tableau *tokens*.
 *    Si le nombre de tokens dépasse la taille maximale *max*, la fonction retourne -1.
 */
int strcut(char* str, char sep, char** tokens, size_t max) {
    if (!str || !tokens) return -1;
    
    int cp = 0;
    int i = 0;
    int start = 0;
    int in_quotes = 0;
    
    while (str[i] != '\0' && cp < max) {
        if (str[i] == '"') {
            in_quotes = !in_quotes;
        }
        
        if (str[i] == sep && !in_quotes) {
            str[i] = '\0';
            if (i > start) {
                tokens[cp++] = &str[start];
            }
            start = i + 1;
        }
        i++;
    }
    
    if (start < i && cp < max) {
        tokens[cp++] = &str[start];
    }
    
    tokens[cp] = NULL;
    return cp;
}





/** @brief Fonction d'analyse d'une ligne de commande.
 * @param cmdl Pointeur vers la structure de ligne de commande à remplir.
 * @param line Chaîne de caractères contenant la ligne de commande à analyser.
 * @return int 0 en cas de succès, -1 en cas d'erreur (ligne trop longue, trop de commandes, etc.).
 * @details Cette fonction analyse la ligne de commande *line* et remplit la structure *cmdl* avec les informations extraites.
 *    La ligne de commande est copiée dans *cmdl->command_line* dans la limite de MAX_CMD_LINE caractères.
 *    La ligne est ensuite nettoyée (trim, clean, separate_s, replace, substenv), puis découpée en tokens.
 *    Les tokens sont ensuite utilisés pour remplir les structures processus_t et control_flow_t dans *cmdl*.
 *    Si la ligne dépasse la taille maximale ou si le nombre de commandes dépasse MAX_CMDS, la fonction retourne -1.
 *    Si une erreur est détectée, les descripteurs de fichiers ouverts sont fermés via close_fds(cmdl) avant de retourner -1.
*/
int parse_command_line(command_line_t* cmdl, const char* line) {
 //   strncpy(cmdl->command_line, line, MAX_CMD_LINE - 1);
 //  cmdl->command_line[MAX_CMD_LINE - 1] = '\0';

    if (trim(cmdl->command_line) != 0) {
        return -1;
    }

    if (clean(cmdl->command_line) != 0) {
        return -1;
    }

    if (separate_s(cmdl->command_line, ";", MAX_CMD_LINE) != 0) {
        return -1;
    }

    if (substenv(cmdl->command_line, MAX_CMD_LINE) != 0) {
        return -1;
    }

    int num_tokens = strcut(cmdl->command_line, ' ', cmdl->tokens , MAX_CMD_LINE / 2 + 1);
    if (num_tokens < 0) {
        return -1;
    }

    int token_index = 0;
    int argv_index = 0;
    processus_t* current_proc = add_processus(cmdl, UNCONDITIONAL);

    while (cmdl->tokens[token_index] != NULL) {
        char* token = cmdl->tokens[token_index];
        
        if (strcmp(token, ";") == 0) {
            if (cmdl->tokens[token_index + 1] == NULL) {
                break;
            }
            current_proc = add_processus(cmdl, UNCONDITIONAL);
            argv_index = 0;
            token_index++;
            continue;
        }
        
        if (strcmp(token, "<") == 0) {
            token_index++;
            if (cmdl->tokens[token_index] == NULL) {
                fprintf(stderr, "Erreur de syntaxe: fichier attendu après '<'\n");
                close_fds(cmdl);
                return -1;
            }
            int fd = open(cmdl->tokens[token_index], O_RDONLY);
            if (fd < 0) {
                perror("open");
                close_fds(cmdl);
                return -1;
            }
            current_proc->stdin_fd = fd;
            if (add_fd(cmdl, fd) != 0) {
                close(fd);
                close_fds(cmdl);
                return -1;
            }
            token_index++;
            continue;
        }
        
        if (strcmp(token, ">") == 0) {
            token_index++;
            if (cmdl->tokens[token_index] == NULL) {
                fprintf(stderr, "Erreur de syntaxe: fichier attendu après '>'\n");
                close_fds(cmdl);
                return -1;
            }
            int fd = open(cmdl->tokens[token_index], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                close_fds(cmdl);
                return -1;
            }
            if (add_fd(cmdl, fd) != 0) {
                close(fd);
                close_fds(cmdl);
                return -1;
            }
            current_proc->stdout_fd = fd;
            token_index++;
            continue;
        }

        if (strcmp(token, ">>") == 0) {
            token_index++;
            if (cmdl->tokens[token_index] == NULL) {
                fprintf(stderr, "Erreur de syntaxe: fichier attendu après '>>'\n");
                close_fds(cmdl);
                return -1;
            }
            int fd = open(cmdl->tokens[token_index], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("open");
                close_fds(cmdl);
                return -1;
            }
            if (add_fd(cmdl, fd) != 0) {
                close(fd);
                close_fds(cmdl);
                return -1;
            }
            current_proc->stdout_fd = fd;
            token_index++;
            continue;
        }

        if (strcmp(token, "2>") == 0) {
            token_index++;
            if (cmdl->tokens[token_index] == NULL) {
                fprintf(stderr, "Erreur de syntaxe: fichier attendu après '2>'\n");
                close_fds(cmdl);
                return -1;
            }
            int fd = open(cmdl->tokens[token_index], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                close_fds(cmdl);
                return -1;
            }
            if (add_fd(cmdl, fd) != 0) {
                close(fd);
                close_fds(cmdl);
                return -1;
            }
            current_proc->stderr_fd = fd;
            token_index++;
            continue;
        }

        if (strcmp(token, "2>>") == 0) {
            token_index++;
            if (cmdl->tokens[token_index] == NULL) {
                fprintf(stderr, "Erreur de syntaxe: fichier attendu après '2>>'\n");
                close_fds(cmdl);
                return -1;
            }
            int fd = open(cmdl->tokens[token_index], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("open");
                close_fds(cmdl);
                return -1;
            }
            if (add_fd(cmdl, fd) != 0) {
                close(fd);
                close_fds(cmdl);
                return -1;
            }
            current_proc->stderr_fd = fd;
            token_index++;
            continue;
        }

        if (strcmp(token, "|") == 0) {
            int pipefd[2];
            if (pipe(pipefd) < 0) {
                perror("pipe");
                close_fds(cmdl);
                return -1;
            }
            if (add_fd(cmdl, pipefd[0]) != 0 || add_fd(cmdl, pipefd[1]) != 0) {
                close(pipefd[0]);
                close(pipefd[1]);
                close_fds(cmdl);
                return -1;
            }

            current_proc->stdout_fd = pipefd[1];
            current_proc = add_processus(cmdl, UNCONDITIONAL);
            if (current_proc == NULL) {
                fprintf(stderr, "Error: Too many commands\n");
                close_fds(cmdl);
                return -1;
            }

            current_proc->stdin_fd = pipefd[0];
            argv_index = 0;
            token_index++;
            continue;
        }

        if (strcmp(token, "&&") == 0) {
             if (cmdl->tokens[token_index + 1] == NULL) break;
             current_proc = add_processus(cmdl, ON_SUCCESS);
             argv_index = 0;
             token_index++;
             continue;
        }

        if (strcmp(token, "||") == 0) {
             if (cmdl->tokens[token_index + 1] == NULL) break;
             current_proc = add_processus(cmdl, ON_FAILURE);
             argv_index = 0;
             token_index++;
             continue;
        }

        if (strcmp(token, "&") == 0) {
           current_proc->is_background = 1;
           token_index++;
           continue;
        }

        if (strcmp(token, "!") == 0) {
            current_proc->invert = 1;
            token_index++;
            continue;
        }
        
        
       // token = remove_quotes(token);
        if (token) {
   	 size_t len = strlen(token);

    	if (len >= 2 && token[0] == '"' && token[len - 1] == '"') {
        token[len - 1] = '\0';  
        token = token + 1;    
   		 }
		}
        
        // ---  QUOTE REMOVAL  ---

        if (argv_index >= MAX_ARGS - 1) {
            fprintf(stderr, "Erreur: trop d'arguments pour une commande (max %d)\n", MAX_ARGS - 1);
            close_fds(cmdl);
            return -1;
        }
        
        if (argv_index == 0) {
            current_proc->path = token;
        }
        current_proc->argv[argv_index++] = token;
        token_index++;
    }
    
    return 0;
}
