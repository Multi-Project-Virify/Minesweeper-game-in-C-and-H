#ifndef CONFIG_H
#define CONFIG_H

#define CFG_SYM_LEN 8 /* хватает под UTF-8 символ типа "▢" (до 4 байт) + запас */

typedef struct {
    int size;
    int mines;
    int border;                 /* 1 = рисовать рамку, 0 = без рамки */
    char closed[CFG_SYM_LEN];   /* символ закрытой клетки */
    char flag[CFG_SYM_LEN];     /* символ флага */
    char mine[CFG_SYM_LEN];     /* символ мины */
    char empty[CFG_SYM_LEN];    /* символ пустой (0 соседних мин) клетки */
} Config;

/* Заполняет cfg значениями по умолчанию (используется, если
   config.txt отсутствует или содержит ошибку — программа
   никогда не падает из-за конфига). */
void config_defaults(Config *cfg);

/* Пытается прочитать path (например "config.txt") и переопределить
   поля cfg. Отсутствующие/некорректные строки просто игнорируются,
   значения по умолчанию остаются в силе. */
void config_load(Config *cfg, const char *path);

#endif