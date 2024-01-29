#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define WALLL (w/4)
#define WALLR ((w/4)*3)
#define SPACE 3 //判定とAAの場所合わせ。良い方法あるかも
#define BOSSSPACE 7
#define N_BULLET 5 //敵の弾の最大数
#define N_UFO 32 //UFOの数
#define N_BEAM 2 //自機の弾の最大数

typedef struct {
        double px, py;
        double vx, vy;
        double sx, sy;
        int    life;
} Object;

void InitObject(Object *obj, double px, double py, double vx, double vy, double sx, double sy, int life)
{
        obj->px = px; obj->py = py;
        obj->vx = vx; obj->vy = vy;
        obj->sx = sx; obj->sy = sy;
        obj->life = life;
}

void InitUFO(Object ufo[], int n, double vx)
{
        int i, h, w;
        int x = 1;
        int y = 0;

        getmaxyx(stdscr, h, w);

        for (i = 0; i < n; i++) {
                InitObject(&ufo[i], (((w/10)*3)+(10*(x))), (2+(4*y)), vx, 0.0, 3.0, 2.0, 1);
                x++;
                if ((x%9) == 0) {
                        y++;
                        x = 1;
                }
        }
}

void InitBullet (Object bullet[], int n)
{

}

void InitBeam (Object beam[], int n)
{
        int i;

        for (i = 0; i < n; i++) {
                InitObject(&beam[i], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0 );
        }
}

int BeamAmo (Object beam[])
{
        if (beam->py <= 0) return 0;
        return 1;
}

void MoveObject(Object *obj)
{
        obj->px += obj->vx;
        obj->py += obj->vy;
}

int Hit (Object *obj1, Object *obj2)
{
        if (fabs(obj1->px - obj2->px) > (obj1->sx + obj2->sx)) return (0);
        if (fabs(obj1->py - obj2->py) > (obj1->sy + obj2->sy)) return (0);
        return (1);
}

void DrawBlocks(int y, int x, char *s)
{
        int w, h;

        getmaxyx(stdscr, h, w);
        if (y < 0) return;
        if (y >= h) return;

        while (*s != '\0') {
        if (x < 0);
        else if (x >= w);
        else if (*s != ' ') mvaddch(y, x, *s);
        s++;
        x++;
        }
}

void DrawWall()
{
        int w, h;

        getmaxyx(stdscr, h, w);
        while (h != -1) {
                mvaddch(h, WALLR, '#');
                mvaddch(h, WALLL, '#');
                h--;
        }
}

void DrawBeam (Object *beam)
{
        int x, y;

        x = (int)(beam->px);
        y = (int)(beam->py);

        mvaddch(y, x, '|');

}

void MoveBeam (Object *beam)
{

        beam->vy = -1;

        MoveObject(beam);

        if (beam->py <= 0) beam->life = 0;

}

void MoveUFO (Object ufo[], int n)
{
        int i, j, w, h;

        n = N_UFO;
        getmaxyx(stdscr, h, w);

        for (i = 0; i < n; i++) {
                if (ufo[i].life == 1) MoveObject(&ufo[i]);
               
                if ((int)ufo[i].px == (WALLR-3)) {
                        for (j = 0; j < N_UFO; j++) {
                                ufo[j].vx = -0.1;
                                ufo[j].py++;
                        }
                }

                if ((int)ufo[i].px == (WALLL+3)) {
                        for (j = 0; j < N_UFO; j++) {
                                ufo[j].vx = 0.1;
                                ufo[j].py++;
                        }
                }
        }
}

void DrawUFO(Object *ufo)
{
        int x, y;

        x = (int)(ufo->px) - SPACE;
        y = (int)(ufo->py);

        DrawBlocks(y-2, x, "  ___  ");
        DrawBlocks(y-1, x, " / @ \\");
        DrawBlocks(y  , x, "<~~~~~>");
        DrawBlocks(y+1, x, " \" \" \"");
}

int RandUFO()
{
        return (1 + (int)(19*rand()/(RAND_MAX + 1.0)));
}

void DrawBreak (Object *obj)
{
        int x, y;

        x = (int)(obj->px) - SPACE;
        y = (int)(obj->py);

        DrawBlocks(y-2, x, " \\ | /");
        DrawBlocks(y-1, x, "-     -");
        DrawBlocks(y  , x, "-     - ");
        DrawBlocks(y+1, x, " / | \\ ");
}

void DrawBOSS(Object *boss)
{
        int x, y;

        x = (int)(boss->px) - BOSSSPACE;
        y = (int)(boss->py);

        DrawBlocks(y-2, x, "   ________/\\________  ");
        DrawBlocks(y-1, x, "  /                  \\ ");
        DrawBlocks(y  , x, " /  @   @   @   @  @  \\");
        DrawBlocks(y+1, x, "<~~~~~~~~~~~~~~~~~~~~~~>");
        DrawBlocks(y+2, x, "   \\=/  \\=/  \\=/  \\=/");
}

void DrawLife(Object *roketto)
{
        int h, w;
        int i = roketto->life;

        getmaxyx(stdscr, h, w);

        mvaddstr(h-5, 2, "#      #####  #####  #####");
        mvaddstr(h-4, 2, "#        #    #      #    ");
        mvaddstr(h-3, 2, "#        #    ####   #### ");
        mvaddstr(h-2, 2, "#        #    #      #    ");
        mvaddstr(h-1, 2, "#####  #####  #      #####");

        while (i != 0) {
                mvaddstr(h-4, (25+(i * 8)), "   A   ");
                mvaddstr(h-3, (25+(i * 8)), "  / \\ ");
                mvaddstr(h-2, (25+(i * 8)), "<  O  >");
                mvaddstr(h-1, (25+(i * 8)), " /WWW\\");
                i--;
        }

}
/*
void DrawScore (int s)
{
        int h, w;

        getmaxyx(stdscr, h, w);

}
*/
void DrawRoketto(Object *roketto)
{
        int x, y;

        x = (int)(roketto->px);
        y = (int)(roketto->py);

        DrawBlocks(y-2, x, "   A   ");
        DrawBlocks(y-1, x, "  / \\ ");
        DrawBlocks(y  , x, "<  O  >");
        DrawBlocks(y+1, x, " /WWW\\");
}

void MoveRoketto(Object *roketto)
{
        int w, h;

        getmaxyx(stdscr, h, w);

        if ((int)(roketto->px) <= (WALLL+2)) roketto->px = (WALLL+2);
        if ((int)(roketto->px) >= (WALLR-8)) roketto->px = (WALLR-8);//8,2は自機のサイズ合わせ
}

void Game()
{
        Object boss;
        Object ufo[N_UFO];
        Object roketto;
        Object bullet;
        Object beam[N_BEAM];
        int h, w, i, j;
        int key, fire;
        int bamo = 0;

        srand(time(NULL));

        getmaxyx(stdscr, h, w);
        InitBeam(beam, N_BEAM);
        InitObject(&roketto, ((w/2)-SPACE), (h-3), 0.0, 0.0, 3.0, 2.0, 2);
        InitObject(&bullet, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
        InitUFO(ufo, N_UFO, 0.1);

        //InitObject(&boss, ((w/2)-9), 25, 0.0, 0.0, 12.0, 3.0, 30); //位置調整の-9


        timeout(0);
        while (1) {
                erase();
                fire = 0;
                DrawWall();
                DrawLife(&roketto);
                DrawRoketto(&roketto);

                for (i = 0; i < N_UFO; i++) {
                        if (ufo[i].life == 1) {
                                DrawUFO(&ufo[i]);
                        } else if (ufo[i].life == 0) {
                                DrawBreak(&ufo[i]);
                                ufo[i].life = -1;
                        } else {
                                InitObject(&ufo[i], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1);
                        }
                }


                for (j = 0; j < N_BEAM; j++) {
                        if (beam[j].life != 0) DrawBeam(&beam[j]);
                }

                refresh();

                key = getch();
                switch (key) {
                case 'd' : roketto.px += 1; break;
                case 'a' : roketto.px -= 1; break;
                case ' ' : fire = 1; break;
                case 'p' : return;
                }

                MoveRoketto(&roketto);
                MoveUFO(ufo, N_UFO);

                if (bamo < N_BEAM) {
                        if (fire == 1) {
                                if (beam[bamo].life != 1) {
                                        InitObject(&beam[bamo], (roketto.px + 3), (roketto.py - 3), 0.0, 0.0, 0.0, 0.0, 1);
                                        bamo++;
                                }
                                else continue;
                        }
                }

                for (j = 0; j < N_BEAM; j++) {
                        for (i = 0; i < N_UFO; i++) {
                                if (beam[j].life != 0) {
                                        if (Hit(&beam[j], &ufo[i]) != 0) {
                                                beam[j].life = 0;
                                                ufo[i].life = 0;
                                                bamo--;
                                                break;
                                        }
                                }

                                //if ((int)ufo[i].py == (h-3)) Title();
                        }
                }

                for (j = 0; j < N_BEAM; j++) {
                        if (beam[j].life != 0) {
                                MoveBeam(&beam[j]);
                                if (BeamAmo(&beam[j]) != 1) bamo--;
                        }
                }

                usleep(20000);
        }
}

/*int GameOver()
{
        int h, w, key;

        getmaxyx(stdscr, h, w);

        erase();
        mvaddstr(h/2-11, ((w/2)-26), "Game Over");
        mvaddstr(h/2-10, ((w/2)-27), "[R] to Restart");

        timeout(-1);

}
*/
int Title()
{
        int h, w, key;

        while(1) {
                getmaxyx(stdscr, h, w);

                erase();
                mvaddstr(h/2-11, ((w/2)-26), " ##   ##   #####            ##   ##  #######   ##### ");
                mvaddstr(h/2-10, ((w/2)-26), " ##   ##  ##   ##           ##   ##   ##   #  ##   ##");
                mvaddstr(h/2-9, ((w/2)-26),  "  ## ##   #                 ##   ##   ## #    ##   ##");
                mvaddstr(h/2-8, ((w/2)-26),  "  ## ##    #####            ##   ##   ####    ##   ##");
                mvaddstr(h/2-7, ((w/2)-26),  "   ###         ##           ##   ##   ## #    ##   ##");
                mvaddstr(h/2-6, ((w/2)-26),  "   ###    ##   ##           ##   ##   ##      ##   ##");
                mvaddstr(h/2-5, ((w/2)-26),  "    #      #####             #####   ####      ##### ");

                mvaddstr(h/2-3, ((w/2)-25),  "             press SPACE to start                    ");
                mvaddstr(h/2-2, ((w/2)-25),  "      [A][D] : move  [SPACE] : shot  [P] : end           ");
                move(0,0);

                timeout(-1);
                key = getch();

                return (key);
        }
}

int main(void)
{
        int key;
        initscr();
        noecho();
        cbreak();
        curs_set(0);
        timeout(-1);

        while (1) {
        key = Title();          // タイトル

        if (key == 'p') break;
        else if (key == ' ') Game();
        else continue;
        }
        endwin();
        return (0);
}
