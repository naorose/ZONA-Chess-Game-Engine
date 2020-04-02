#include "stdio.h"
#include "structures.h"
#ifdef WIN32
#include "windows.h"
#else
#include "sys/time.h"
#include "string.h"
#include "sys/select.h"
#include "unistd.h"
#endif

// source: OliThink 5.3.2 (OpenSource Chess Engine) http://home.arcor.de/dreamlike/chess/
int InputWaiting() {
#ifndef WIN32
    fd_set readfds;
    struct timeval tv;
    FD_ZERO (&readfds);
    FD_SET (fileno(stdin), &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    select(16, &readfds, 0, 0, &tv);

    return (FD_ISSET(fileno(stdin), &readfds));
#else
    static int init = 0, pipe;
        static HANDLE inh;
        DWORD dw;
        if (!init) {
          init = 1;
          inh = GetStdHandle(STD_INPUT_HANDLE);
          pipe = !GetConsoleMode(inh, &dw);
          if (!pipe) {
             SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
             FlushConsoleInputBuffer(inh);
           }
         }
         if (pipe) {
           if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
           return dw;
         } else {
           GetNumberOfConsoleInputEvents(inh, &dw);
           return dw <= 1 ? 0 : dw;
         }
#endif
}

void read_input(SEARCH_INFO_STRUCTURE *information) {
    int bits;
    char s[MAX_NO_MOVES_POSITION] = "", *endc;

    if (InputWaiting()) {
        information->ended = T;
        while (bits < 0) {
            bits = read(fileno(stdin), s, 256);
            endc = strchr(s, '\n');
            if (endc) *endc = 0;
            if (strlen(s) > 0) {
                if (!strncmp(s, "quit", 4)) {
                    information->quit = T;
                }
            }
            return;
        }
    }
}

//Gets time depending on windows or unix machines
int getElapsedTimeMilliseconds() {
    #ifdef WIN32
        return GetTickCount();
    #else
        struct timeval time_value;
        gettimeofday(&time_value, NULL);
        return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    #endif
}