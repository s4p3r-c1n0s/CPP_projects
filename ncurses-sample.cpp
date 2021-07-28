/*
 gcc $(ncursesw5-config --cflags --libs) SUDOKU.CPP 
*/

#include<fstream>
#include<iostream>
#include<curses.h>
#include<locale.h>
#include<cmath>
#include<cstdio>
#include<cstdlib>
#include<cstring>

int main() {
	setlocale(LC_ALL, "");

	initscr();
	cbreak();
	noecho();

	printw("Euro\n");

	printw("€\n");              // literal Unicode
	printw("\u20ac\n");         // escaped Unicode (C99)

	unsigned char cross=218;	
	printw("%lc\n", L'€');      // wint_t
	printw("%lc\n", cross);      // wint_t
	printw("%ls\n", L"€");      // wchar_t

	printw("\xe2\x82\xac\n");   // utf-8 encoded


	int nlines = 55;
	int ncols = 55;
	int x0 = 5;
	int y0 = 5;
	char ch = 'h';
	refresh();
	WINDOW* win = newwin(nlines, ncols, y0, x0);
	box(win, 0, 0);  // added for easy viewing
	for (unsigned char i = 187; i <= 188; i++)
	{
	    wprintw(win, "%lc-%i,\n", i , i);

	}	
	wrefresh(win);
	/*
	wmove(win, 10, 10);
	wrefresh(win);
	waddch(win, ch);
	wrefresh(win);
	waddstr(win, "Umang Jeet Singh Pahwa");	
	wclear(win);
	wrefresh(win);
	*/
	int gch = getch();
	endwin();
	return 0;	
}


