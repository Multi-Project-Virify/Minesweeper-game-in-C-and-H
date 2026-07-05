#include <stdio.h>
#include <time.h>
#include "core.h"
#include "config.h"
#include "platform.h"

static void render(const CoreGame *g, const Config *cfg) {
    int x, y;

    platform_clear();

    printf("Платформа: %s\n\n", PLATFORM_NAME);

    /* заголовок с номерами столбцов */
    printf("    ");
    for (x = 0; x < g->size; x++) printf("%2d", x);
    printf("\n");

    if (cfg->border) {
        printf("   +");
        for (x = 0; x < g->size; x++) printf("--");
        printf("-+\n");
    }

    for (y = 0; y < g->size; y++) {
        printf("%2d %c", y, cfg->border ? '|' : ' ');
        for (x = 0; x < g->size; x++) {
            if (!core_is_open(g, x, y)) {
                printf(" %s", core_is_flag(g, x, y) ? cfg->flag : cfg->closed);
            } else if (core_is_mine(g, x, y)) {
                printf(" %s", cfg->mine);
            } else {
                int c = core_count(g, x, y);
                if (c == 0) printf(" %s", cfg->empty);
                else printf(" %d", c);
            }
        }
        printf(" %c\n", cfg->border ? '|' : ' ');
    }

    if (cfg->border) {
        printf("   +");
        for (x = 0; x < g->size; x++) printf("--");
        printf("-+\n");
    }

    printf("\nМины: %d   Флагов: %d   Осталось найти: %d\n",
           g->mines, core_flags_used(g), g->mines - core_flags_used(g));
}

int main(void) {
    Config cfg;
    CoreGame game;
    int alive = 1, won = 0;

    config_defaults(&cfg);
    config_load(&cfg, "config.txt");

    platform_init();
    core_seed((unsigned long)time(NULL));
    core_reset(&game, cfg.size, cfg.mines);

    printf("Сапёр. Платформа определена автоматически: %s\n", PLATFORM_NAME);
    printf("Команды: o x y (открыть), f x y (флаг), q (выход)\n");
    printf("Настройки поля/символов — в файле config.txt (без пересборки).\n");

    while (alive) {
        char cmd;
        int x, y, n;

        render(&game, &cfg);
        printf("\n> ");

        n = scanf(" %c", &cmd);
        if (n != 1) break;

        if (cmd == 'q') {
            break;
        } else if (cmd == 'o' || cmd == 'f') {
            n = scanf(" %d %d", &x, &y);
            if (n != 2) {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                printf("Нужно два числа: %c x y\n", cmd);
                continue;
            }
            if (x < 0 || x >= game.size || y < 0 || y >= game.size) {
                printf("Координаты вне поля (0..%d)\n", game.size - 1);
                continue;
            }
            if (cmd == 'o') {
                int r = core_open(&game, x, y);
                if (r == -1) {
                    alive = 0;
                } else if (core_is_win(&game)) {
                    won = 1;
                    alive = 0;
                }
            } else {
                core_toggle_flag(&game, x, y);
            }
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Неизвестная команда. Используй o/f/q.\n");
        }
    }

    render(&game, &cfg);
    if (won) printf("\nПОБЕДА! Все безопасные клетки открыты.\n");
    else if (game.dead) printf("\nВЗРЫВ. Игра окончена.\n");
    else printf("\nВыход из игры.\n");

    return 0;
}