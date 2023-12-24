#include <locale.h>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int Title()
{
	int h, w, key;

	getmaxyx(stdscr, h, w);

	erase();
	mvaddstr(h/2-11, ((w/2)-25), " ##   ##   #####            ##   ##  #######   ##### ");
	mvaddstr(h/2-10, ((w/2)-25), " ##   ##  ##   ##           ##   ##   ##   #  ##   ##");
	mvaddstr(h/2-9, ((w/2)-25),  "  ## ##   #                 ##   ##   ## #    ##   ##");
	mvaddstr(h/2-8, ((w/2)-25),  "  ## ##    #####            ##   ##   ####    ##   ##");
	mvaddstr(h/2-7, ((w/2)-25),  "   ###         ##           ##   ##   ## #    ##   ##");
	mvaddstr(h/2-6, ((w/2)-25),  "   ###    ##   ##           ##   ##   ##      ##   ##");
	mvaddstr(h/2-5, ((w/2)-25),  "    #      #####             #####   ####      ##### ");
	refresh();

	timeout(-1);
	key = getch();

	return (key);
}

int main(void) 
{
	int key;
	initscr();	
	noecho();	
	cbreak();	
	curs_set(0);	
	timeout(-1);	

	Title();

	endwin();
	return (0);
}
