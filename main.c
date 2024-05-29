#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define WALLL ((w/4)-1)
#define WALLR (((w/4)*3)+1)
#define SPACE 3 //判定とAAの場所合わせ。良い方法あるかも
#define BOSSSPACE 7
#define N_BULLET 5 //敵の弾の最大数
#define N_UFO 32 //UFOの数
#define N_BEAM 2 //自機の弾の最大数
#define R_COOLTIME 5 //自機の射撃間隔
#define U_COOLTIME 3 //UFOの射撃間隔

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
                InitObject(&ufo[i], (w/10*3+10*x), (2+5*y), vx, 0.0, 3.0, 2.0, 1);
                x++;
                if ((x%9) == 0) {
                        y++;
                        x = 1;
                }
        }
}

void InitBullet (Object bullet[], int n)
{
        int i, h, w;

        getmaxyx(stdscr, h, w);

        for (i = 0; i < n; i++) {
                InitObject(&bullet[i], 0.0, (w+1), 0.0, 0.0, 0.0, 0.0, 0 );
        }
}

void InitBeam (Object beam[], int n)
{
        int i, h, w;

        getmaxyx(stdscr, h, w);

        for (i = 0; i < n; i++) {
                InitObject(&beam[i], 0.0, (w+1), 0.0, 0.0, 0.0, 0.0, 0 );
        }
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

int Hit (Object *obj1, Object *obj2)
{
        if (fabs(obj1->px - obj2->px) > (obj1->sx + obj2->sx)) return (0);
        if (fabs(obj1->py - obj2->py) > (obj1->sy + obj2->sy)) return (0);
        return (1);
}

void MoveObject(Object *obj)
{
        obj->px += obj->vx;
        obj->py += obj->vy;
}

int RandBullet(int n)
{
        return (rand() % N_UFO);
}

void MoveBullet (Object *bullet)
{
        int w, h;

        getmaxyx(stdscr, h, w);

        bullet->vy = 0.3;

        MoveObject(bullet);

        if (bullet->py >= h) bullet->life = 0;
}

void DrawBullet (Object *bullet)
{
        int x, y;

        x = (int)(bullet->px);
        y = (int)(bullet->py);

        mvaddch(y, x, '$');
}

void DrawBullets (Object bullet[], int n) 
{
        int i;

        for (i = 0; i < n; i++) {
                attrset(COLOR_PAIR(1));
                DrawBullet(&bullet[i]);
        }
}

void DrawBeam (Object *beam)
{
        int x, y;

        x = (int)(beam->px);
        y = (int)(beam->py);

        mvaddch(y, x, '|');
}

void DrawBeams (Object beam[], int n) 
{
        int j;

        for (j = 0; j < n; j++) {
                attrset(COLOR_PAIR(3));
                DrawBeam(&beam[j]);
        }
}

void MoveBeam (Object *beam)
{
        int w, h;

        getmaxyx(stdscr, h, w);

        beam->vy = -0.8;

        MoveObject(beam);

        if (beam->py <= 0) InitObject(beam, 0.0, (w+1), 0.0, 0.0, 0.0, 0.0, 0 );
}

void MoveUFO (Object ufo[], int n, int s)
{
        int i, j, w, h;

        getmaxyx(stdscr, h, w);

        for (i = 0; i < n; i++) {
                if (ufo[i].px >= (WALLR-5) && ufo[i].life == 1) {
                        for (j = 0; j < N_UFO; j++) {
                                ufo[j].py+=1;
                                ufo[j].vx = -0.05*s;
                        }
                        break;
                } else if (ufo[i].px <= (WALLL+5) && ufo[i].life == 1) {
                        for (j = 0; j < N_UFO; j++) {
                                ufo[j].py+=1;
                                ufo[j].vx = 0.05*s;
                        }
                        break;
                } 
        }

        for (i = 0; i < n; i++) {
                if (ufo[i].life == 1) MoveObject(&ufo[i]);
        }
}

void SpeedUFO (Object ufo[], int n, int s)
{
        int i;

        for (i  = 0; i < n; i++) {
                if (ufo[i].life == 1 && ufo[i].vx < 0) ufo[i].vx = -0.05*s;
                if (ufo[i].life == 1 && ufo[i].vx > 0) ufo[i].vx = 0.05*s;
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
/*
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
*/

void DrawLife(Object *roketto)
{
        int h, w;
        int i = (roketto->life - 1);

        getmaxyx(stdscr, h, w);

        
        mvaddstr(h-5, 2, "#      #####  #####  #####");
        mvaddstr(h-4, 2, "#        #    #      #    ");
        mvaddstr(h-3, 2, "#        #    ####   #### ");
        mvaddstr(h-2, 2, "#        #    #      #    ");
        mvaddstr(h-1, 2, "#####  #####  #      #####");

        while (i > 0) {
                mvaddstr(h-4, (25+(i * 8)), "   A   ");
                mvaddstr(h-3, (25+(i * 8)), "  / \\ ");
                mvaddstr(h-2, (25+(i * 8)), "<  O  >");
                mvaddstr(h-1, (25+(i * 8)), " /WWW\\");
                i--;
        }

}

void DrawRoketto(Object *roketto)
{
        int x, y;

        x = (int)(roketto->px) - SPACE;
        y = (int)(roketto->py);

        DrawBlocks(y-2, x, "   A   ");
        DrawBlocks(y-1, x, "  / \\ ");
        DrawBlocks(y  , x, "<  O  >");
        DrawBlocks(y+1, x, " /WWW\\");
}

void DrawUFOs (Object ufo[], int n) {

        int i;

        for (i = 0; i < n; i++) {
                if (ufo[i].life == 1) {
                        if (i < 8) attrset(COLOR_PAIR(5));
                        else if (i < 16 && i >= 8) attrset(COLOR_PAIR(4));
                        else if (i < 24 && i >= 16) attrset(COLOR_PAIR(2));
                        else attrset(COLOR_PAIR(3));
                        DrawUFO(&ufo[i]);
                        continue;
                }
                if (ufo[i].life == 0) {
                        attrset(COLOR_PAIR(1));
                        DrawBreak(&ufo[i]);
                        InitObject(&ufo[i], 0, 0.0, 0.0, 0.0, 0.0, 0.0, -1);
                        continue;
                }
        }
}

void MoveRoketto(Object *roketto)
{
        int w, h;

        getmaxyx(stdscr, h, w);

        if (roketto->px <= (WALLL+4)) roketto->px = (WALLL+4);
        if (roketto->px >= (WALLR-4)) roketto->px = (WALLR-4);//8,2は自機のサイズ合わせ
}

int GameOver()
{
        int h, w, key;

        while (1) {
                getmaxyx(stdscr, h, w);

                erase();
                attrset(COLOR_PAIR(1));
                mvaddstr(h/2-11, ((w/2)-35), "   ####     ##     ##   ##  #######      #####   ##   ##  #######  ######");
                mvaddstr(h/2-10, ((w/2)-35), "  ##  ##   ####    ### ###   ##   #     ##   ##  ##   ##   ##   #   ##  ##");
                mvaddstr(h/2-9, ((w/2)-35),  " ##       ##  ##   #######   ## #       ##   ##   ## ##    ## #     ##  ##");
                mvaddstr(h/2-8, ((w/2)-35),  " ##       ##  ##   #######   ####       ##   ##   ## ##    ####     #####");
                mvaddstr(h/2-7, ((w/2)-35),  " ##  ###  ######   ## # ##   ## #       ##   ##    ###     ## #     ## ##");
                mvaddstr(h/2-6, ((w/2)-35),  "  ##  ##  ##  ##   ##   ##   ##   #     ##   ##    ###     ##   #   ##  ##");
                mvaddstr(h/2-5, ((w/2)-35),  "   #####  ##  ##   ##   ##  #######      #####      #     #######  #### ##");

                attrset(COLOR_PAIR(2));
                mvaddstr(h/2-3, ((w/2)-8),  "press [R] to Restart");
                mvaddstr(h/2-2,  ((w/2)-8), "press [P] to Title");

                timeout(-1);

                key = getch();
                if (key == 'p') return(0);
                if (key == 'r') return(1);
                else continue;
        }
}

int GameClear(int score)
{
        int h, w, key;

        while (1) {
                getmaxyx(stdscr, h, w);

                erase();
                attrset(COLOR_PAIR(3));
                mvaddstr(h/2-11, ((w/2)-37), "   ####     ##     ##   ##  #######       ####   ####     #######    ##     ######");
                mvaddstr(h/2-10, ((w/2)-37), "  ##  ##   ####    ### ###   ##   #      ##  ##   ##       ##   #   ####     ##  ##");
                mvaddstr(h/2-9, ((w/2)-37),  " ##       ##  ##   #######   ## #       ##        ##       ## #    ##  ##    ##  ##");
                mvaddstr(h/2-8, ((w/2)-37),  " ##       ##  ##   #######   ####       ##        ##       ####    ##  ##    #####");
                mvaddstr(h/2-7, ((w/2)-37),  " ##  ###  ######   ## # ##   ## #       ##        ##   #   ## #    ######    ## ##");
                mvaddstr(h/2-6, ((w/2)-37),  "  ##  ##  ##  ##   ##   ##   ##   #      ##  ##   ##  ##   ##   #  ##  ##    ##  ##");
                mvaddstr(h/2-5, ((w/2)-37),  "   #####  ##  ##   ##   ##  #######       ####   #######  #######  ##  ##   #### ##");

                attrset(COLOR_PAIR(2));
                mvaddstr(h/2-3, ((w/2)-8),  "press [R] to Restart");
                mvaddstr(h/2-2,  ((w/2)-8), "press [P] to Title");
                attrset(COLOR_PAIR(6));
                mvprintw(h/2,  ((w/2)-5),   "Score : %d", score);

                timeout(-1);

                key = getch();
                if (key == 'p') return(0);
                if (key == 'r') return(1);
                else continue;
        }
}

void Game()
{
        Object boss;
        Object ufo[N_UFO];
        Object roketto;
        Object bullet[N_BULLET];
        Object beam[N_BEAM];
        int h, w; //縦横
        int i, j; //for文用
        int key, fire, posi; //
        int flag; //クリアorゲームオーバー
        int dead; //残基切れの判定
        int kill; //倒した数
        int score;
        int rctime = R_COOLTIME;
        int uctime = U_COOLTIME;

RESTART:
        srand(time(NULL));
        flag = 0;
        dead = 0;
        score = 0;
        kill = 1;

        getmaxyx(stdscr, h, w);
        InitObject(&roketto, ((w/2)), (h-3), 0.0, 0.0, 4.0, 2.0, 3);
        InitBeam(beam, N_BEAM);
        InitBullet(bullet, N_BULLET);
        InitUFO(ufo, N_UFO, 0.05);

        timeout(0);
        while (1) {
                erase();
                attrset(COLOR_PAIR(17));
                DrawWall();
                attrset(COLOR_PAIR(6));
                DrawLife(&roketto);
                mvprintw(h-7, 1, "Score:%d", score);
                //UFO描画
                DrawUFOs(ufo, N_UFO);

                //自機描画
                if (dead == 1) {
                        attrset(COLOR_PAIR(1));
                        DrawBreak(&roketto);
                        InitObject(&roketto, ((w/2)-SPACE), (h-3), 0.0, 0.0, 4.0, 2.0, roketto.life);
                        InitBullet(bullet, N_BULLET);
                        InitBeam(beam, N_BEAM);

                        if (roketto.life == 0) flag = 1;

                        timeout(-1);
                        dead = 0;
                } else {
                        attrset(COLOR_PAIR(6));
                        DrawRoketto(&roketto);
                }

                DrawBeams(beam, N_BEAM);
                DrawBullets(bullet, N_BULLET);

                refresh();

                fire = 0;
                key = getch();
                timeout(0);

                if (flag == 1) {
                        if (GameOver() == 1) goto RESTART;
                        else key = 'p';
                } else if (flag == 2) {
                        if (GameClear(score) == 1) goto RESTART;
                        else key = 'p';
                } else flag = 0;

                switch (key) {
                case KEY_RIGHT:
                case 'd' : roketto.px += 1; break;
                case KEY_LEFT:
                case 'a' : roketto.px -= 1; break;
                case KEY_UP:
                case ' ' : fire = 1; break;
                case 'p' : return;
                }

                //自機の射撃判定
                if (rctime <= 0) {
                        if (fire == 1) {
                                for (i = 0; i < N_BEAM; i++) {
                                        if (beam [i].life) continue;

                                        InitObject(&beam [i], roketto.px, (roketto.py - 3), 0.0, 0.0, 0.0, 0.0, 1);
                                        rctime = R_COOLTIME;
                                        break;
                                }
                        }
                }else rctime--;

                //UFOの射撃判定
                if (uctime <= 0) {
                        for (i = 0; i < N_BULLET; i++) {
                                posi = RandBullet(N_UFO);
                                if (ufo[posi].life != 1 || bullet[i].life) continue;
                                
                                InitObject(&bullet[i], ufo[posi].px, ufo[posi].py, 0.0, 0.0, 0, 0, 1);
                                uctime = U_COOLTIME;
                                break;
                        }
                } else uctime--;

                //自機のヒット判定
                for (i = 0; i < N_BULLET; i++) {
                        if (bullet[i].life != 0) {
                                if (Hit(&bullet[i], &roketto) != 0) {
                                        bullet[i].life = 0;
                                        roketto.life--;
                                        dead = 1;
                                }
                        }
                        MoveBullet(&bullet[i]);
                }

                //UFOのヒット判定
                for (j = 0; j < N_BEAM; j++) {
                        for (i = 0; i < N_UFO; i++) {
                                if (beam[j].life != 0) {
                                        if (Hit(&beam[j], &ufo[i]) != 0) {
                                                beam[j].life = 0;
                                                InitObject(&beam[j], 0.0, (w+1), 0.0, 0.0, 0.0, 0.0, 0 );
                                                ufo[i].life = 0;
                                                kill++;
                                                score+=10;
                                                SpeedUFO(ufo, N_UFO, kill);
                                        }
                                }
                                if (ufo[i].py >= (h - 5)) {
                                        InitBeam(&beam[j], N_BEAM);
                                        flag = 1;
                                        break;
                                }
                        }

                        if (beam[j].life != 0) MoveBeam(&beam[j]);
                }

                MoveRoketto(&roketto);
                MoveUFO(ufo, N_UFO, kill);

                if (kill == (N_UFO+1)) {
                        score += (roketto.life * 100);
                        flag = 2;
                }

                usleep(20000);
        }
}

void InitColor()
{
        start_color();

        init_pair(0, COLOR_BLACK, COLOR_BLACK);
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);

        init_pair(10, COLOR_BLACK, COLOR_BLACK);
        init_pair(11, COLOR_RED, COLOR_RED);
        init_pair(12, COLOR_GREEN, COLOR_GREEN);
        init_pair(13, COLOR_YELLOW, COLOR_YELLOW);
        init_pair(14, COLOR_BLUE, COLOR_BLUE);
        init_pair(15, COLOR_MAGENTA, COLOR_MAGENTA);
        init_pair(16, COLOR_CYAN, COLOR_CYAN);
        init_pair(17, COLOR_WHITE, COLOR_WHITE);

        bkgd(COLOR_PAIR(7));
}

int Title()
{
        int h, w, key;

        while(1) {
                getmaxyx(stdscr, h, w);

                erase();
                attrset(COLOR_PAIR(2));
                mvaddstr(h/2-11, ((w/2)-25), " ##   ##   #####            ##   ##  #######   ##### ");
                mvaddstr(h/2-10, ((w/2)-25), " ##   ##  ##   ##           ##   ##   ##   #  ##   ##");
                mvaddstr(h/2-9, ((w/2)-25),  "  ## ##   #                 ##   ##   ## #    ##   ##");
                mvaddstr(h/2-8, ((w/2)-25),  "  ## ##    #####            ##   ##   ####    ##   ##");
                mvaddstr(h/2-7, ((w/2)-25),  "   ###         ##           ##   ##   ## #    ##   ##");
                mvaddstr(h/2-6, ((w/2)-25),  "   ###    ##   ##           ##   ##   ##      ##   ##");
                mvaddstr(h/2-5, ((w/2)-25),  "    #      #####             #####   ####      ##### ");

                attrset(COLOR_PAIR(1));
                mvaddstr(h/2-3, ((w/2)-25),  "             press SPACE to start                    ");
                attrset(COLOR_PAIR(3));
                mvaddstr(h/2-1, ((w/2)-25),  "      Move : [A][D]or[LEFT_KEY][RIGHT_KEY]           ");
                mvaddstr(h/2, ((w/2)-25),    "                  Shot : [SPACE]                     ");
                mvaddstr(h/2+1, ((w/2)-25),  "                   End : [P]                         ");
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
        keypad(stdscr, TRUE);
        InitColor();

        while (1) {
                key = Title();

                if (key == ' ') Game();
                else if (key == 'p') break;
                else continue;
        }
        endwin();
        return (0);
}
