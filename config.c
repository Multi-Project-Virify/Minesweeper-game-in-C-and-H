#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void config_defaults(Config *cfg) {
    cfg->size = 8;
    cfg->mines = 10;
    cfg->border = 1;
    strncpy(cfg->closed, "#", CFG_SYM_LEN);
    strncpy(cfg->flag,   "F", CFG_SYM_LEN);
    strncpy(cfg->mine,   "X", CFG_SYM_LEN);
    strncpy(cfg->empty,  ".", CFG_SYM_LEN);
}

static void trim(char *s) {
    char *start = s;
    char *end;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
    end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end - 1))) { end--; *end = '\0'; }
}

void config_load(Config *cfg, const char *path) {
    FILE *f = fopen(path, "r");
    char line[256];

    if (!f) return; /* нет файла -> остаёмся на значениях по умолчанию */

    while (fgets(line, sizeof(line), f)) {
        char *eq;
        char *key, *val;
        /* убрать комментарии после # (кроме случая, когда сам # - значение символа,
           поэтому комментарии разрешены только строкой целиком, начинающейся с ';') */
        if (line[0] == ';' || line[0] == '\n' || line[0] == '\r') continue;

        eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        key = line;
        val = eq + 1;
        /* убрать перенос строки в конце значения */
        val[strcspn(val, "\r\n")] = '\0';
        trim(key);
        trim(val);
        if (key[0] == '\0' || val[0] == '\0') continue;

        if (strcmp(key, "size") == 0) {
            int v = atoi(val);
            if (v >= 4 && v <= 24) cfg->size = v;
        } else if (strcmp(key, "mines") == 0) {
            int v = atoi(val);
            if (v >= 1) cfg->mines = v;
        } else if (strcmp(key, "border") == 0) {
            cfg->border = atoi(val) ? 1 : 0;
        } else if (strcmp(key, "closed") == 0) {
            strncpy(cfg->closed, val, CFG_SYM_LEN - 1);
            cfg->closed[CFG_SYM_LEN - 1] = '\0';
        } else if (strcmp(key, "flag") == 0) {
            strncpy(cfg->flag, val, CFG_SYM_LEN - 1);
            cfg->flag[CFG_SYM_LEN - 1] = '\0';
        } else if (strcmp(key, "mine") == 0) {
            strncpy(cfg->mine, val, CFG_SYM_LEN - 1);
            cfg->mine[CFG_SYM_LEN - 1] = '\0';
        } else if (strcmp(key, "empty") == 0) {
            strncpy(cfg->empty, val, CFG_SYM_LEN - 1);
            cfg->empty[CFG_SYM_LEN - 1] = '\0';
        }
        /* неизвестные ключи молча игнорируются - не ломают программу */
    }

    fclose(f);

    /* финальная защита: мин не может быть больше клеток минус одна */
    if (cfg->mines > cfg->size * cfg->size - 1)
        cfg->mines = cfg->size * cfg->size - 1;
}