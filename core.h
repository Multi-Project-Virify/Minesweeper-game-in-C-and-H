/* ================================================================
   core.h — логика игры "Сапёр". Никаких stdio/malloc/рекурсии —
   компилируется любым C-компилятором (C89/C99), на любой ОС.
   Правила такие же, как в оригинальном Windows Minesweeper:
   безопасна только САМАЯ ПЕРВАЯ открытая клетка (не её соседи).
   ================================================================ */
#ifndef CORE_H
#define CORE_H

/* Максимальный размер поля, под который выделена память заранее
   (без malloc). Реальный размер игры может быть меньше — задаётся
   в config.txt во время запуска, без пересборки. */
#define CORE_MAXSIZE 24
#define CORE_MAXCELLS (CORE_MAXSIZE * CORE_MAXSIZE)

typedef unsigned char core_u8;

/* биты состояния клетки */
#define CORE_BIT_MINE 0x01
#define CORE_BIT_OPEN 0x02
#define CORE_BIT_FLAG 0x04
#define CORE_COUNT_SHIFT 3 /* биты 3-6 хранят число соседних мин (0..8) */

typedef struct {
    core_u8 cell[CORE_MAXCELLS];
    int size;       /* реальная сторона поля, <= CORE_MAXSIZE */
    int mines;      /* реальное число мин */
    int started;    /* 0 = мины ещё не расставлены */
    int dead;       /* 1 = подорвался */
} CoreGame;

/* Задать зерно генератора (свой xorshift, не зависит от stdlib rand). */
void core_seed(unsigned long seed);

/* Начать новую игру с заданными size x size и числом мин. */
void core_reset(CoreGame *g, int size, int mines);

/* Открыть клетку. При первом вызове расставляет мины (сама клетка
   гарантированно безопасна — как в оригинальном Сапёре).
   Возвращает: 1 = ок, -1 = мина, 0 = недопустимый ход. */
int core_open(CoreGame *g, int x, int y);

/* Переключить флаг. Возвращает новое состояние (0/1) или -1. */
int core_toggle_flag(CoreGame *g, int x, int y);

/* Победа = все не-минные клетки открыты. */
int core_is_win(const CoreGame *g);

int core_is_open(const CoreGame *g, int x, int y);
int core_is_flag(const CoreGame *g, int x, int y);
int core_is_mine(const CoreGame *g, int x, int y);
int core_count(const CoreGame *g, int x, int y);
int core_flags_used(const CoreGame *g);

#endif /* CORE_H */