/* ================================================================
   core.c — реализация. Без stdio, malloc, float, рекурсии.
   ================================================================ */
#include "core.h"

static int idx(const CoreGame *g, int x, int y) { return y * g->size + x; }
static int in(const CoreGame *g, int x, int y) {
    return x >= 0 && y >= 0 && x < g->size && y < g->size;
}

static unsigned long rng_state = 1;

void core_seed(unsigned long seed) { rng_state = seed ? seed : 1; }

static unsigned long core_rand(void) {
    unsigned long x = rng_state; /* xorshift32 */
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}
static int rand_range(int n) { return (int)(core_rand() % (unsigned long)n); }

void core_reset(CoreGame *g, int size, int mines) {
    int i;
    if (size < 4) size = 4;
    if (size > CORE_MAXSIZE) size = CORE_MAXSIZE;
    if (mines < 1) mines = 1;
    if (mines > size * size - 1) mines = size * size - 1;

    g->size = size;
    g->mines = mines;
    g->started = 0;
    g->dead = 0;
    for (i = 0; i < CORE_MAXCELLS; i++) g->cell[i] = 0;
}

/* Классическое правило: безопасна ТОЛЬКО стартовая клетка (sx,sy),
   как в оригинальном Windows Minesweeper. */
static void place_mines(CoreGame *g, int sx, int sy) {
    int placed = 0;
    int safe_i = idx(g, sx, sy);
    while (placed < g->mines) {
        int x = rand_range(g->size);
        int y = rand_range(g->size);
        int i = idx(g, x, y);
        if (i == safe_i) continue;
        if (g->cell[i] & CORE_BIT_MINE) continue;
        g->cell[i] |= CORE_BIT_MINE;
        placed++;
    }
}

static void calc_counts(CoreGame *g) {
    int x, y, dx, dy;
    for (y = 0; y < g->size; y++) {
        for (x = 0; x < g->size; x++) {
            int c = 0;
            int i = idx(g, x, y);
            if (g->cell[i] & CORE_BIT_MINE) continue;
            for (dy = -1; dy <= 1; dy++)
                for (dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    if (in(g, x + dx, y + dy) &&
                        (g->cell[idx(g, x + dx, y + dy)] & CORE_BIT_MINE))
                        c++;
                }
            g->cell[i] = (core_u8)((g->cell[i] & 0x07) | (c << CORE_COUNT_SHIFT));
        }
    }
}

int core_open(CoreGame *g, int x, int y) {
    static int stackx[CORE_MAXCELLS];
    static int stacky[CORE_MAXCELLS];
    int sp = 0;

    if (!in(g, x, y)) return 0;

    if (!g->started) {
        place_mines(g, x, y);
        calc_counts(g);
        g->started = 1;
    }

    {
        int i = idx(g, x, y);
        if ((g->cell[i] & CORE_BIT_OPEN) || (g->cell[i] & CORE_BIT_FLAG)) return 0;
        if (g->cell[i] & CORE_BIT_MINE) {
            g->cell[i] |= CORE_BIT_OPEN;
            g->dead = 1;
            return -1;
        }
    }

    stackx[sp] = x; stacky[sp] = y; sp++;

    while (sp > 0) {
        int cx, cy, i, dx, dy;
        sp--;
        cx = stackx[sp]; cy = stacky[sp];
        i = idx(g, cx, cy);
        if (g->cell[i] & CORE_BIT_OPEN) continue;
        g->cell[i] |= CORE_BIT_OPEN;

        if ((g->cell[i] >> CORE_COUNT_SHIFT) == 0) {
            for (dy = -1; dy <= 1; dy++)
                for (dx = -1; dx <= 1; dx++) {
                    int nx = cx + dx, ny = cy + dy, ni;
                    if (dx == 0 && dy == 0) continue;
                    if (!in(g, nx, ny)) continue;
                    ni = idx(g, nx, ny);
                    if (!(g->cell[ni] & CORE_BIT_OPEN) &&
                        !(g->cell[ni] & CORE_BIT_MINE) &&
                        !(g->cell[ni] & CORE_BIT_FLAG) &&
                        sp < CORE_MAXCELLS) {
                        stackx[sp] = nx; stacky[sp] = ny; sp++;
                    }
                }
        }
    }
    return 1;
}

int core_toggle_flag(CoreGame *g, int x, int y) {
    int i;
    if (!in(g, x, y)) return -1;
    i = idx(g, x, y);
    if (g->cell[i] & CORE_BIT_OPEN) return -1;
    g->cell[i] ^= CORE_BIT_FLAG;
    return (g->cell[i] & CORE_BIT_FLAG) ? 1 : 0;
}

int core_is_win(const CoreGame *g) {
    int i, n = g->size * g->size;
    for (i = 0; i < n; i++)
        if (!(g->cell[i] & CORE_BIT_MINE) && !(g->cell[i] & CORE_BIT_OPEN))
            return 0;
    return 1;
}

int core_is_open(const CoreGame *g, int x, int y) { return (g->cell[idx(g,x,y)] & CORE_BIT_OPEN) ? 1 : 0; }
int core_is_flag(const CoreGame *g, int x, int y) { return (g->cell[idx(g,x,y)] & CORE_BIT_FLAG) ? 1 : 0; }
int core_is_mine(const CoreGame *g, int x, int y) { return (g->cell[idx(g,x,y)] & CORE_BIT_MINE) ? 1 : 0; }
int core_count(const CoreGame *g, int x, int y)   { return g->cell[idx(g,x,y)] >> CORE_COUNT_SHIFT; }

int core_flags_used(const CoreGame *g) {
    int i, n = g->size * g->size, c = 0;
    for (i = 0; i < n; i++) if (g->cell[i] & CORE_BIT_FLAG) c++;
    return c;
}