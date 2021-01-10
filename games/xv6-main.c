#include "game.h"
#include "types.h"
#include "user.h"

int main()
{
    consmode(1);
    game_init();

    short *sndbuf = (short *)malloc(2048);

    // int T = 0;
    int tmp = 0;
    int tmp_t = 0;
    // int flag = 0;
    while (1) {
        // if (T % 30 == 0) printf(1, "frame: %d\n", T);
        // TODO: Replace with vertical retrace
        // printf(1, "1: %d\n", apictimer());
        // printf(1, "2: %d\n", apictimer());
        // int i = apictimer();
        int cur = apictimer();
        if(cur < tmp){
            // printf(1, "0: %d %d\n", cur, tmp);
            tmp = 0;
        }
        // if(tmp == 0)
        //   printf(1, "what: %d %d\n", j, tmp);
        if(cur - tmp > 5000 && uptime() != tmp_t){
            
            unsigned buttons = 0;
            if (kbd(0))     buttons |= BTN_L;
            if (kbd(2))     buttons |= BTN_R;
            if (kbd(1))     buttons |= BTN_U;
            if (kbd(3))     buttons |= BTN_D;
            if (kbd('c'))   buttons |= BTN_C;
            if (kbd('x'))   buttons |= BTN_X;
            if (kbd('z'))   buttons |= BTN_Z;
            if (kbd(' '))   buttons |= BTN_SP;
            if (kbd('q')) break;
            game_update(buttons);

            // printf(1, "%d\n", T);
            // printf(1, "5s: %d\n", i);
            // printf(1, "j: %d %d\n", cur, tmp);

            void* img = game_draw();
            imgdraw(img);

            // sleep(20);
            tmp = cur;
        }

        if (trsound()) {
            game_audio(1024, sndbuf);
            setsound(sndbuf);
        }

        tmp_t = uptime();
    }

    consmode(0);
    exit();
}
