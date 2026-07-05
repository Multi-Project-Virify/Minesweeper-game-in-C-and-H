/* ================================================================
   platform.h — код САМ определяет, под какой ОС собирается,
   и подстраивает мелочи консоли (очистка экрана, поддержка UTF-8
   на Windows). Это и есть честная версия "автоматической адаптации":
   один и тот же main.c компилируется где угодно, а этот файл
   на этапе компиляции выбирает нужный код под конкретную ОС.
   ================================================================ */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define PLATFORM_NAME "Windows"

    static void platform_init(void) {
        /* включаем поддержку UTF-8 в консоли Windows,
           чтобы символы вроде # . не превращались в кракозябры */
        SetConsoleOutputCP(CP_UTF8);
    }
    static void platform_clear(void) {
        if (system("cls") != 0) printf("\n\n");
    }

#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__) || defined(__FreeBSD__)
    #include <stdlib.h>
    #define PLATFORM_NAME "Unix-like (Linux/macOS/BSD)"

    static void platform_init(void) { /* ничего не требуется */ }
    static void platform_clear(void) {
        if (system("clear") != 0) printf("\n\n");
    }

#else
    /* неизвестная/встраиваемая платформа: работаем без очистки
       экрана и без системных вызовов — программа всё равно
       соберётся и будет играбельна, просто без "красивого" сброса. */
    #define PLATFORM_NAME "Unknown/embedded (fallback)"

    static void platform_init(void) { }
    static void platform_clear(void) { printf("\n\n"); }

#endif

#endif /* PLATFORM_H */