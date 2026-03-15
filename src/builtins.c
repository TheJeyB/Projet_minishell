/** @file builtins.c
 * @brief Implementation of built-in commands
 * @author ROMDHANE Iskander
 * @author HADJADJ Mohammed Hadjaissa
 * @date 2025-26
 * @details Implémentation des fonctions des commandes intégrées.
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <stdio.h>
#include <ctype.h>

#include "builtins.h"
#include "processus.h"

/** @brief Fonction de vérification si une commande est une commande "built-in".
 * @param cmd Nom de la commande à vérifier.
 * @return int 1 si la commande est intégrée, 0 sinon.
 * @details Les commandes intégrées sont a minima: cd, exit, export, unset, pwd.
 */
int is_builtin(const processus_t* cmd) {
    if (cmd == NULL || cmd->argv == NULL || cmd->argv[0] == NULL)
        return 0;

    const char* name = cmd->argv[0];
    return (
        strcmp(name, "cd") == 0 ||
        strcmp(name, "exit") == 0 ||
        strcmp(name, "export") == 0 ||
        strcmp(name, "unset") == 0 ||
        strcmp(name, "pwd") == 0 ||
        strcmp(name, "true") == 0 ||
        strcmp(name, "false") == 0
    );
}

/** @brief Fonction d'exécution d'une commande intégrée.
 * @param cmd Pointeur vers la structure de commande à exécuter.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 */
int exec_builtin(processus_t* cmd) {
    if (cmd == NULL || cmd->argv == NULL || cmd->argv[0] == NULL)
        return -1;

    const char* name = cmd->argv[0];

    if (strcmp(name, "cd") == 0)
        return builtin_cd(cmd);
    else if (strcmp(name, "exit") == 0)
        return builtin_exit(cmd);
    else if (strcmp(name, "export") == 0)
        return builtin_export(cmd);
    else if (strcmp(name, "unset") == 0)
        return builtin_unset(cmd);
    else if (strcmp(name, "pwd") == 0)
        return builtin_pwd(cmd);
    else if (strcmp(name, "true") == 0)
        return 0;
    else if (strcmp(name, "false") == 0)
        return 1;

    return -1;
}

/** Fonctions spécifiques aux commandes intégrées. */
/** @brief Fonction d'exécution de la commande "cd".
 * @param cmd Pointeur vers la structure de commande à exécuter.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 * @details Déplace le CWD du processus vers le répertoire spécifié dans le premier argument de la commande.
 *  Si aucun argument n'est fourni, le CWD est déplacé vers le répertoire HOME de l'utilisateur.
 *  En cas d'erreur (répertoire inexistant, permission refusée, etc.), un message d'erreur est affiché sur *cmd->stderr* et la fonction retourne un code d'erreur.
 */
int builtin_cd(processus_t* cmd) {
    if (!cmd || !cmd->argv || !cmd->argv[0]) return -1;

    char* path = NULL;
    
    if (cmd->argv[1] == NULL || strcmp(cmd->argv[1], "~") == 0) {
        struct passwd* pw = getpwuid(getuid());
        if (!pw) {
            dprintf(cmd->stderr_fd, "cd: cannot get home directory\n");
            return -1;
        }
        path = pw->pw_dir;
    } else {
        path = cmd->argv[1];
    }

    if (!path) {
        dprintf(cmd->stderr_fd, "cd: path not found\n");
        return -1;
    }

    if (chdir(path) != 0) {
        dprintf(cmd->stderr_fd, "cd: %s: No such file or directory\n", path);
        return -1;
    }

    return 0;
}

/** @brief Fonction d'exécution de la commande "exit".
 * @param cmd Pointeur vers la structure de commande à exécuter.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 * @details Termine le shell avec le code de sortie spécifié dans le premier argument de la commande.
 *  Si aucun argument n'est fourni, le shell se termine avec le code de sortie 0.
 *  En cas d'erreur (argument non numérique, etc.), un message d'erreur est affiché sur *cmd->stderr* et la fonction retourne un code d'erreur
 */
int builtin_exit(processus_t* cmd) {
    int code = 0;

    if (cmd->argv[1]) {
        char* endptr;
        code = strtol(cmd->argv[1], &endptr, 10);
        if (*endptr != '\0') {
            dprintf(cmd->stderr_fd, "exit: numeric argument required\n");
            return -1;
        }
    }

    exit(code);
}

/** @brief Fonction d'exécution de la commande "export".
 * @param cmd Pointeur vers la structure de commande à exécuter.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 * @details Ajoute ou modifie une variable d'environnement dans l'environnement du shell. En cas d'erreur (format invalide, etc.), un message d'erreur est affiché sur *cmd->stderr* et la fonction retourne un code d'erreur.
 */
int builtin_export(processus_t* cmd) {
    for (int i = 1; cmd->argv[i]; i++) {
        char* arg = cmd->argv[i];
        char* eq = strchr(arg, '=');

        if (!eq || eq == arg) {
            dprintf(cmd->stderr_fd, "export: invalid format: %s\n", arg);
            return -1;
        }

        size_t name_len = eq - arg;

        if (name_len > 1023) {
             dprintf(cmd->stderr_fd, "export: variable name too long\n");
             return -1;
        }
        char name[name_len + 1];

        memcpy(name, arg, name_len);
        name[name_len] = '\0';

        char* value = eq + 1;

        size_t value_len = strlen(value);
        while (value_len > 0 && isspace((unsigned char)value[value_len - 1])) {
            value[value_len - 1] = '\0';
            value_len--;
        }
        
        if (setenv(name, value, 1) != 0) {
            dprintf(cmd->stderr_fd, "export: failed to set %s\n", name);
            return -1;
        }
    }

    return 0;
}

/** @brief Fonction d'exécution de la commande "unset".
 * @param cmd Pointeur vers la structure de commande à exécuter.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 * @details Supprime une variable d'environnement de l'environnement du shell. En cas d'erreur (variable inexistante, etc.), un message d'erreur est affiché sur *cmd->stderr* et la fonction retourne un code d'erreur.
 */
int builtin_unset(processus_t* cmd) {
    for (int i = 1; cmd->argv[i]; i++) {
        if (unsetenv(cmd->argv[i]) != 0) {
            dprintf(cmd->stderr_fd, "unset: failed to unset %s\n", cmd->argv[i]);
            return -1;
        }
    }
    return 0;
}

/** @brief Fonction d'exécution de la commande "pwd".
 * @param cmd Pointeur vers la structure de commande à exécuter.
 * @return int 0 en cas de succès, -1 en cas d'erreur.
 * @details Affiche le répertoire de travail actuel (CWD) du processus sur la sortie standard *cmd->stdout*. En cas d'erreur, un message d'erreur est affiché sur *cmd->stderr* et la fonction retourne un code d'erreur.
 */
int builtin_pwd(processus_t* cmd) {
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        dprintf(cmd->stderr_fd, "pwd: error retrieving current directory\n");
        return -1;
    }

    dprintf(cmd->stdout_fd, "%s\n", buffer);
    return 0;
}
