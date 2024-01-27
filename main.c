#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define WallL (w/4)
#define WallR ((w/4)*3)

typedef struct {
        double px, py;  
        double vx, vy;
        int    life;    
} Object;

void InitObject(Object *obj, double px, double py, double vx, double vy, int life)
{
        obj->px = px; obj->py = py;
        obj->vx = vx; obj->vy = vy;
        obj->life = life;
}

void MoveObject(Object *obj)
{
        obj->px += obj->vx;
        obj->py += obj->vy;
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
                mvaddch(h, WallR, '#');
                mvaddch(h, WallL, '#');
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

void MoveUFO(Object *obj)
{
        int w, h;

        getmaxyx(stdscr, h, w);
        MoveObject(obj);

        sleep(30000);

}

void DrawUFO(Object *ufo)
{
        int x, y;

        x = (int)(ufo->px);
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

void DrawBOSS(Object *boss)
{
        int x, y;

        x = (int)(boss->px);
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

        mvaddstr(h-10, 15, "life : "); //AAに変える

        while (i != 0) {
                mvaddstr(h-4, (5+(i * 8)), "   A   ");
                mvaddstr(h-3, (5+(i * 8)), "  / \\ ");
                mvaddstr(h-2, (5+(i * 8)), "<  O  >");
                mvaddstr(h-1, (5+(i * 8)), " /WWW\\");
                i--;
        }

}

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

        if ((int)(roketto->px) <= (WallL+2)) roketto->px = (WallL+2);
        if ((int)(roketto->px) >= (WallR-8)) roketto->px = (WallR-8);//8,2は自機のサイズ合わせ
}

void Roketto(Object *roketto)
{

        int x;
}

void Game()
{
        Object boss;
        Object ufo;
        Object roketto;
        Object bullet;
        Object beam;
        int h, w;
        int key;
        //int ufos[23]; //ufoの数　縦4、横6
        int ux, uy;

        srand(time(NULL));

        getmaxyx(stdscr, h, w);
        InitObject(&beam, 15, (w/2), 0.0, 0.0, 0);
        InitObject(&roketto, (w/2), (h-5), 0.0, 0.0, 3);
        InitObject(&bullet, 0.0, 0.0, 0.0, 0.0, 0);
        /*
        InitObject(&boss, ((w/2)-9), 25, 0.0, 0.0, 30); //位置調整の-9
        */
        InitObject(&ufo, ((w/10)*3), 5, 0.0, 0.0, 1);

        /*while (i = 0; i++; i <=23) {
                ufos[i] =
        }*/

        timeout(0);
        while (1) {
                erase();
                DrawWall();
                DrawLife(&roketto);
                DrawRoketto(&roketto);
                MoveRoketto(&roketto);

                for (ux = 6; ux--; ux != 0){
                        for (uy = 4; uy--; uy != 0) {
                                InitObject(&ufo, ((((w/10)*3)+5)+(13*ux)), (4+(4*uy)), 0.0, 0.0, 1);
                                DrawUFO(&ufo);
                        }
                }

                if (beam.life != 0) DrawBeam(&beam);
                //DrawBOSS(&boss);

                refresh();

                if (beam.life != 0) {
                        MoveBeam(&beam);
                }

                key = getch();
                switch (key) {
                case 'd' : roketto.px += 1; break;   
                case 'a' : roketto.px -= 1; break;   
                case ' ' : 
                        if (beam.life != 0) break;
                        else {
                                        InitObject(&beam, (roketto.px + 3), (roketto.py - 3), 0.0, 0.0, 1);
                                        break;
                        }   
                case 'p' : return;                  
                }
                usleep(20000);
        }
}

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
