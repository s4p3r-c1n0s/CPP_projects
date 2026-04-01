/*
g++ -lncursesw WSUDOKU.CPP
*/

//INCLUDED HEADER FILES
/******************************************************************/
#include<fstream>
#include<iostream>
#include<curses.h>
#include<cmath>
#include<locale.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cctype>
#include<ctime>
#include<unistd.h>
#include<vector>
/******************************************************************/
const int m=9,v=22;
WINDOW* win;
const char* const kSudokuFile="sudoku.dat";
bool g_has_colors=false;
const short kPairNormal=1;
const short kPairEntry=2;
const short kPairPlaceholder=3;
inline void init_ui_colors()
{
	if(!has_colors())
		return;
	start_color();
#ifdef NCURSES_VERSION
	use_default_colors();
#endif
	init_pair(kPairNormal,COLOR_WHITE,-1);
	init_pair(kPairEntry,COLOR_RED,-1);
	init_pair(kPairPlaceholder,COLOR_BLUE,-1);
	g_has_colors=true;
}
inline void style_normal()
{
	if(g_has_colors)
		wattrset(win,COLOR_PAIR(kPairNormal));
	else
		wattrset(win,A_NORMAL);
}
inline void style_entry()
{
	if(g_has_colors)
		wattrset(win,COLOR_PAIR(kPairEntry)|A_BOLD);
	else
		wattrset(win,A_BOLD);
}
inline void style_placeholder()
{
	if(g_has_colors)
		wattrset(win,COLOR_PAIR(kPairPlaceholder)|A_BOLD);
	else
		wattrset(win,A_BOLD);
}
inline void style_for_cell(int val)
{
	if(val>9)
	{
		char c=static_cast<char>(val);
		if(c>='1' && c<='9')
			style_entry();
		else
			style_placeholder();
		return;
	}
	style_normal();
}
inline void move_xy(int x,int y)
{
	int rows=0,cols=0;
	getmaxyx(win,rows,cols);
	if(rows<=0 || cols<=0)
		return;
	if(x<0) x=0;
	if(y<0) y=0;
	if(x>=cols) x=cols-1;
	if(y>=rows) y=rows-1;
	wmove(win,y,x);
}
inline void seed_rng_once()
{
	static bool seeded=false;
	if(seeded)
		return;
	unsigned int seed=static_cast<unsigned int>(std::time(nullptr));
	seed^=static_cast<unsigned int>(std::clock());
	seed^=static_cast<unsigned int>(getpid());
	std::srand(seed);
	seeded=true;
}
/******************************************************************/
class sudoku;
long score(int a[m][m],int w[m][m],int level);
void solve(int a[m][m],int s[m][m]);
void logic(int a[m][m], int z[m*m][m]);
void copier(int sc[m][m],int s[m][m],int zc[m*m][m],int z[m*m][m]);
int verify(int s[m][m]);
int sumer(int s[m][m]);
int check(int s[m][m]);
void render_board(int a[m][m]);
void enterin(int a[m][m]);
void draw_sudoku_grid(int p);
int char_to_digit(char c);
void intro();
void play(int a[m][m],int w[m][m],int level);
void endgame(int a[m][m],int w[m][m],int level);
void draw_grid_char(int i,int j,char c);
void table(int n);
void prompt_level(int &level);
void instructions();
void write(std::fstream &f);			//NEW FUNC
void create(int a[m][m]);   			//NEW FUNC
void render_preview_board(int a[m][m]);      		//NEW FUNC
int usable_puzzle(sudoku &su,int required_level=0);
void ensure_sudoku_file_layout();
/******************************************************************/
//CLASS NAME : sudoku
//DETAILS : IT GROUPS ALL THE ELEMENTS RELATED TO A SUDOKU
//          & CONTROLS ALL THE FUNCTIONS RELATED TO THEM
/******************************************************************/
class sudoku
{
	int sno;
	int level;
	int a[m][m];
	public:
	void input(int sz);
	void input2(int sz);				//NEW FUNC
	/******************************************************************/
	//FUNCTION TO DISPLAY DATA MEMBERS OF CLASS sudoku
	/******************************************************************/
	void output()
	{
		int s[m][m];
		std::cout<<"Serial No:"<<sno;
		std::cout<<"\nLevel:"<<level;
		render_board(a);
	}
	/******************************************************************/
	//FUNCTION TO RETURN THE LEVEL OF SUDOKU
	/******************************************************************/
	int r_level()
	{
		return level;
	}
	/******************************************************************/
	//FUNCTION TO RETURN THE SERIAL NO. OF SUDOKU
	/******************************************************************/
	int r_sno()
	{
		return sno;
	}
	/******************************************************************/
	//FUNCTION TO RETURN AN ELEMENT OF THE 9x9 ARRAY OF SUDOKU
	/******************************************************************/
	int r_a(int i,int j)
	{
		return a[i][j];
	}
};
/******************************************************************/
//FUNCTION TO ACCEPT THE VALUES OF DATA MEMBERS OF CLASS sudoku
/******************************************************************/
void sudoku::input(int sz)
{
	system("clear");
	sno=sz;
	std::cout<<"Serial No.:"<<sno<<std::endl;
	prompt_level(level);
	std::cout<<"Enter SU-DO-KU:";
	enterin(a);
}
/******************************************************************/
//FUNCTION TO INPUT RECORDS TO THE FILE "sudoku.dat"
/******************************************************************/
void writefile(std::fstream &f)
{
	sudoku su;
	char ch='Y';
	ensure_sudoku_file_layout();
	f.open(kSudokuFile,std::ios::binary|std::ios::in);
	f.seekg(0,std::ios::end);
	long sz=f.tellg()/sizeof(su);
	f.close();
	f.open(kSudokuFile,std::ios::binary|std::ios::app);
	while(ch=='Y')
	{
		su.input(sz+1);
		f.write((char*)(&su),sizeof(su));
		std::cout<<"\nMORE[Y,N]:";
		ch=toupper(getchar());
		std::cout<<ch;
	}
	f.close();
	return;
}
/******************************************************************/
//FUNCTION TO SEARCH RECORDS ACCORDING TO THE LEVEL FROM THE FILE
//"sudoku.dat"
/******************************************************************/
void searchlevel(std::fstream &f)
{
	system("clear");
	int l;
	sudoku su;
	std::cout<<"Enter ";
	prompt_level(l);
	ensure_sudoku_file_layout();
	f.open(kSudokuFile,std::ios::binary|std::ios::in);
	while(f.read((char*)(&su),sizeof(su)))
		if(su.r_level()==l)
		{
			system("clear");
			su.output();
			std::cout<<"\nNEXT";
			getchar();
		}
	f.close();
	return;
}
/******************************************************************/
//FUNCTION TO OUTPUT RECORDS FROM THE FILE "sudoku.dat"
/******************************************************************/
void readfile(std::fstream &f)
{
	sudoku su;
	ensure_sudoku_file_layout();
	f.open(kSudokuFile,std::ios::binary|std::ios::in);
	while(f.read((char*)(&su),sizeof(su)))
	{
		system("clear");
		su.output();
		std::cout<<"\nNEXT";
		getchar();
	}
	f.close();
	return;
}
/******************************************************************/
//FUNCTION TO EDIT RECORDS OF THE FILE "sudoku.dat"
/******************************************************************/
void editfile(std::fstream &f)
{
	system("clear");
	int s_no;
	sudoku su;
	ensure_sudoku_file_layout();
	f.open(kSudokuFile,std::ios::binary|std::ios::in|std::ios::out);
	f.seekg(0,std::ios::end);
	long sz=f.tellg()/sizeof(su);
	f.seekg(0);
	std::cout<<"Enter Serial no to Edit : ";
	std::cin>>s_no;
	if(s_no<1 || s_no>sz)
	{
		std::cout<<"No such record exists";
		getchar();
	}
	else
	{
		f.seekg((s_no-1)*sizeof(su));
		f.read((char*)(&su),sizeof(su));
		su.output();
		std::cout<<"\nPress Enter to Edit........";
		su.input(s_no);
		f.seekp((s_no-1)*sizeof(su));
		f.write((char*)(&su),sizeof(su));
	}
	f.close();
	return;
}

int usable_puzzle(sudoku &su,int required_level)
{
	if(su.r_level()<1 || su.r_level()>5)
		return 0;
	if(required_level!=0 && su.r_level()!=required_level)
		return 0;
	int clues=0;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			int val=su.r_a(i,j);
			if(val<0 || val>9)
				return 0;
			if(val!=0)
				clues++;
		}
	// Avoid trivially empty/corrupt boards.
	return (clues>=17 && clues<=80);
}

void ensure_sudoku_file_layout()
{
	std::fstream f(kSudokuFile,std::ios::in|std::ios::binary);
	if(!f)
		return;
	f.seekg(0,std::ios::end);
	long sz=f.tellg();
	f.close();
	if(sz<0 || (sz%sizeof(sudoku))==0)
		return;

	// Incompatible layout detected: recreate a clean modern data file.
	std::fstream fresh(kSudokuFile,std::ios::out|std::ios::binary|std::ios::trunc);
	fresh.close();
}
/******************************************************************/
//FUNCTION TO RANDOMLY SEARCH RECORDS FROM THE FILE "sudoku.dat"
/******************************************************************/
void searchfile(std::fstream &f,int level,int a[m][m],int w[m][m])
{
	sudoku su;
	std::vector<long> records;
	ensure_sudoku_file_layout();
	f.open(kSudokuFile,std::ios::in|std::ios::binary);
	if(!f)
	{
		wclear(win);
		box(win,0,0);
		move_xy(2,2);
		wprintw(win,"Unable to open sudoku.dat");
		move_xy(2,4);
		wprintw(win,"Press ENTER to continue...");
		wrefresh(win);
		getchar();
		return;
	}
	long rec_no=0;
	while(f.read((char*)(&su),sizeof(su)))
	{
		if(usable_puzzle(su,level))
			records.push_back(rec_no);
		rec_no++;
	}
	f.close();
	if(records.empty())
	{
		wclear(win);
		box(win,0,0);
		move_xy(2,2);
		wprintw(win,"No valid puzzle found for level %d.",level);
		move_xy(2,4);
		wprintw(win,"Use option 6 to create puzzles or option 4 to change level.");
		move_xy(2,6);
		wprintw(win,"Press ENTER to continue...");
		wrefresh(win);
		getchar();
		return;
	}
	f.open(kSudokuFile,std::ios::in|std::ios::binary);
	seed_rng_once();
	long pick=records[rand()%records.size()];
	f.seekg(sizeof(su)*pick);
	f.read((char*)(&su),sizeof(su));
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			a[i][j]=su.r_a(i,j);
			w[i][j]=su.r_a(i,j);
		}
	f.close();
	return;
}
//****************************************************************//
//CLASS NAME : player
//DETAILS : IT GROUPS ALL THE ELEMENTS RELATED TO A PLAYER
//	    & CONTROLS ALL THE FUNCTION RELATED TO THEM
//****************************************************************//
class player
{
	char name[54];
	long scr;
	public:
	void input(int a[m][m],int w[m][m],int level);
	/******************************************************************/
	//FUNCTION TO DISPLAY DATA MEMBERS OF CLASS player
	/******************************************************************/
	void output(int i,int j)
	{
		move_xy(2,(4+(j*2)));
		std::cout<<1+j+(i*v)<<'.';
		move_xy(6,(4+(j*2)));
		puts(name);
		move_xy(61,(4+(j*2)));
		std::cout<<scr;
	}
	/******************************************************************/
	//FUNCTION TO RETURN THE SCORE OF THE PLAYER
	/******************************************************************/
	long r_scr()
	{
		return scr;
	}
	const char* r_name() const
	{
		return name;
	}
};
/******************************************************************/
//FUNCTION TO ACCEPT THE VALUES OF THE DATA MEMBERS OF CLASS player
/******************************************************************/
void player::input(int a[m][m],int w[m][m],int level)
{
	wclear(win);
	box(win,0,0);
	move_xy(2,2);
	wprintw(win,"Enter Your name : ");
	wrefresh(win);
	echo();
	wgetnstr(win,name,sizeof(name)-1);
	noecho();
	if(name[0]=='\0')
		std::strcpy(name,"PLAYER");
	for(int i=0;i<strlen(name);i++)
		name[i]=toupper(name[i]);
	move_xy(2,4);
	wprintw(win,"%s",name);
	scr=score(a,w,level);
	move_xy(2,6);
	wprintw(win,"Your SCORE : %ld",scr);
	move_xy(2,8);
	wprintw(win,"Press any key to continue...");
	wrefresh(win);
	wgetch(win);
}
/******************************************************************/
//FUNCTION TO INPUT RECORDS TO THE FILE "player.dat"
/******************************************************************/
void append_player_score(std::fstream &f,int a[m][m],int w[m][m],int level)
{
	player pl;
	f.open("player.dat",std::ios::app|std::ios::binary);
	pl.input(a,w,level);
	f.write((char*)&pl,sizeof(pl));
	f.close();
	return;
}
/******************************************************************/
//FUNCTION TO DISPLAY A SET OF 22 RECORD FROM THE FILE "player.dat"
/******************************************************************/
void render_highscore_page(std::fstream &f,long start,int n=v)
{
	player pl;
	int rows=0,cols=0;
	getmaxyx(win,rows,cols);
	const int left=2;
	const int right=cols-3;
	const int title_y=1;
	const int info_y=2;
	const int header_y=4;
	const int first_row_y=6;
	const int rank_w=6;
	const int score_w=12;
	int name_w=(right-left+1)-rank_w-score_w-4;
	if(name_w<8)
		name_w=8;
	const int name_x=left+rank_w+1;
	int score_x=name_x+name_w+1;
	if(score_x>right-score_w+1)
		score_x=right-score_w+1;
	if(score_x<name_x+1)
		score_x=name_x+1;

	wclear(win);
	box(win,0,0);
	style_normal();
	move_xy(left,title_y);
	wprintw(win,"HIGHSCORES");
	move_xy(left,info_y);
	wprintw(win,"Entries %ld to %ld",start+1,start+n);
	move_xy(left,header_y);
	wprintw(win,"RANK");
	move_xy(name_x,header_y);
	wprintw(win,"NAME");
	move_xy(score_x,header_y);
	wprintw(win,"SCORE");
	mvwhline(win,header_y+1,left,'-',right-left+1);

	f.open("player.dat",std::ios::in|std::ios::binary);
	for(int j=0;j<n;j++)
	{
		int y=first_row_y+j;
		if(y>=rows-2)
			break;
		f.seekg((start+j)*sizeof(pl));
		if(!f.read((char*)&pl,sizeof(pl)))
			break;
		move_xy(left,y);
		wprintw(win,"%4ld",start+j+1);
		move_xy(name_x,y);
		wprintw(win,"%-*.*s",name_w,name_w,pl.r_name());
		move_xy(score_x,y);
		wprintw(win,"%10ld",pl.r_scr());
	}
	f.close();
	move_xy(left,rows-2);
	wclrtoeol(win);
	wprintw(win,"Press any key to continue...");
	wrefresh(win);
	wgetch(win);
}
/******************************************************************/
//FUNCTION TO RETURN THE NO. OF RECORDS STORED IN FILE "player.dat"
/******************************************************************/
long player_record_count(std::fstream &f)
{
	f.open("player.dat",std::ios::in|std::ios::binary);
	f.seekg(0,std::ios::end);
	long fs=f.tellg()/sizeof(player);
	f.close();
	return fs;
}
/******************************************************************/
//FUNCTION TO DISPLAY RECORDS FROM THE FILE "player.dat"
/******************************************************************/
void show_player_scores(std::fstream &f)
{
	long total=player_record_count(f);
	if(total<=0)
	{
		wclear(win);
		box(win,0,0);
		move_xy(2,2);
		wprintw(win,"No highscores yet.");
		move_xy(2,4);
		wprintw(win,"Press any key to continue...");
		wrefresh(win);
		wgetch(win);
		return;
	}
	int rows=0,cols=0;
	getmaxyx(win,rows,cols);
	int per_page=rows-8;
	if(per_page<1)
		per_page=1;
	for(long start=0;start<total;start+=per_page)
	{
		int page_count=static_cast<int>(total-start);
		if(page_count>per_page)
			page_count=per_page;
		render_highscore_page(f,start,page_count);
	}
	return;
}
/******************************************************************/
//FUNCTION TO SORT RECORDS IN DESCENDING ORDER OF SCORES TO THE FILE
//"player.dat"
/******************************************************************/
void sort_player_scores(std::fstream &f)
{
	player pl1,pl2;
	long pos1,pos2,fs;
	fs=player_record_count(f);
	f.open("player.dat",std::ios::in|std::ios::out|std::ios::binary);
	for(int i=0;i<fs-1;i++)
		for(int j=0;j<fs-i-1;j++)
		{
			pos1=j*sizeof(pl1);
			pos2=(j+1)*sizeof(pl2);
			f.seekg(pos1);
			f.read((char*)&pl1,sizeof(pl1));
			f.seekg(pos2);
			f.read((char*)&pl2,sizeof(pl2));
			if((pl1.r_scr())<(pl2.r_scr()))
			{
				f.seekp(pos1);
				f.write((char*)&pl2,sizeof(pl2));
				f.seekp(pos2);
				f.write((char*)&pl1,sizeof(pl1));
			}
		}
	f.close();
	return;
}
/******************************************************************/
//FUNCTION TO INPUT RECORDS TO THE FILE "continue.dat"
/******************************************************************/
void savegame(int a[m][m],int w[m][m],int level)
{
	std::fstream f;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
			if(!((w[i][j]>='1'&& w[i][j]<='9')||(w[i][j]>=1 && w[i][j]<=9)))
				w[i][j]=0;
	f.open("continue.dat",std::ios::out|std::ios::binary);
	f.write((char*)a,sizeof(int)*m*m);
	f.write((char*)w,sizeof(int)*m*m);
	f.write((char*)&level,sizeof(level));
	f.close();
}
/******************************************************************/
//FUNCTION TO OUTPUT RECORDS FROM THE FILE "continue.dat"
/******************************************************************/
void continuegame(int a[m][m],int w[m][m],int &level)
{
	std::fstream f;
	f.open("continue.dat",std::ios::in|std::ios::binary);
	f.read((char*)a,sizeof(int)*m*m);
	f.read((char*)w,sizeof(int)*m*m);
	f.read((char*)&level,sizeof(level));
	f.close();
}
/******************************************************************/
/******************************************************************/
//MAIN FUNCTION CALLING OTHER FUNCTIONS OF THE PROGRAM
/******************************************************************/
/******************************************************************/
int main()
{
	std::fstream f;
	player pl;
	int s[m][m],w[m][m],l=1,a[m][m];
	unsigned char u;
	const char* term=getenv("TERM");
	if(term==nullptr || strcmp(term,"dumb")==0)
		setenv("TERM","xterm-256color",0);
	setlocale(LC_ALL, "");
	
	initscr();
	cbreak();
	noecho();
	init_ui_colors();
	curs_set(1);
	int rows,cols;
	getmaxyx(stdscr,rows,cols);
	refresh();
	win = newwin(rows, cols, 0, 0);
	style_normal();
	box(win, 0, 0);
	//textcolor(WHITE);
	//intro();
	for(;;)
	{
		wclear(win);
		wrefresh(win);
		wprintw(win,"1) PLAY SUDOKU\n\n2) HIGHSCORES\n\n");
		wprintw(win,"3) CONTINUE\n\n4) SET LEVEL\n\n");
		wprintw(win,"5) INSTRUCTIONS\n\n6) Write File\n\n");
		wprintw(win,"7) Read File\n\n8) Search File");
		wprintw(win,"\n\n9) Edit File\n\n0) Exit");
		wprintw(win,"\n\n\n\nE n t e r   C h o i c e : ");
		u=getchar();
		wrefresh(win);
		while(!(u>='0'&& u<='9'))
			u=getchar();
		switch(u)
		{
			case '1' : searchfile(f,l,a,w);
					   play(a,w,l);break;
			case '2' : show_player_scores(f);break;
			case '3' : continuegame(a,w,l);
					   play(a,w,l);break;
			case '4' : std::cout<<"\nEnter ";prompt_level(l);break;
			case '5' : instructions();break;
			case '6' : write(f);break; 		       	//NEW FUNC
			case '7' : readfile(f);break;
			case '8' : searchlevel(f);break;
			case '9' : editfile(f);break;
			case '0' : exit(0);
		}
	}
	endwin();
	return 0;
}
/******************************************************************/
//FUNCTION TO GIVE INSTRUCTIONS TO THE USER
/******************************************************************/
void instructions()
{
	system("clear");
	move_xy(20,20);
	std::cout<<"I  N  S  T  R  U  C  T  I  O  N  S\n\n\n";
	std::cout<<"\nFill the Grid so that every row, column & 3x3 box ";
	std::cout<<"contains digits 1-9\n\nEach puzzle has 1 correct solution";
	getchar();
}
/******************************************************************/
//FUNCTION TO END THE GAME AND DISPLAY THE SOLUTION & SCORE
/******************************************************************/
void endgame(int a[m][m],int w[m][m],int level)
{
	std::fstream f;
	char c;
	int final_board[m][m],display_board[m][m],s[m][m];
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			display_board[i][j]=w[i][j];
			final_board[i][j]=w[i][j];
			if(!(w[i][j]>=1 && w[i][j]<=9))
			{
				c=w[i][j];
				final_board[i][j]=char_to_digit(c);
				if(final_board[i][j]>=1 && final_board[i][j]<=9)
					display_board[i][j]=c;
				else
					display_board[i][j]=0;
			}
			if(!(final_board[i][j]>=1 && final_board[i][j]<=9))
				final_board[i][j]=0;
		}
	wclear(win);
	box(win,0,0);
	move_xy(2,1);
	wclrtoeol(win);
	wprintw(win,"YOUR FINAL ANSWER");
	move_xy(2,2);
	wclrtoeol(win);
	wprintw(win,"Press any key to view solution...");
	render_board(display_board);
	move_xy(2,1);
	wprintw(win,"YOUR FINAL ANSWER");
	move_xy(2,2);
	wprintw(win,"Press any key to view solution...");
	wrefresh(win);
	wgetch(win);
	//delay(1000);
	do
	{
		solve(a,s);
	}while(verify(s)!=0 && sumer(s)!=0);
	wclear(win);
	box(win,0,0);
	move_xy(2,1);
	wclrtoeol(win);
	wprintw(win,"SOLUTION");
	move_xy(2,2);
	wclrtoeol(win);
	wprintw(win,"Press any key to continue...");
	render_board(s);
	move_xy(2,1);
	wprintw(win,"SOLUTION");
	move_xy(2,2);
	wprintw(win,"Press any key to continue...");
	wrefresh(win);
	wgetch(win);
	//delay(1000);
	append_player_score(f,a,final_board,level);
	sort_player_scores(f);
	show_player_scores(f);
}
/******************************************************************/
//FUNCTION TO ALLOW USER TO SET LEVEL FOR SUDOKU
/******************************************************************/
void prompt_level(int &level)
{
	std::cout<<"Level : ";
	std::cin>>level;
	while(level<1 || level>5)
	{
		std::cout<<"Level[1-5] : ";
		std::cin>>level;
	}
}
/******************************************************************/
//FUNCTION TO ALLOW USER TO PLAY THE GAME
/******************************************************************/
void play(int a[m][m],int w[m][m],int level)
{
	wclear(win);	
	box(win,0,0);	
	wrefresh(win);	
	char ch,c='A',e='!';
	int p=3,n,x=0,y=0;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
			if(w[i][j]==0)
			{
				w[i][j]=c;
				e=c;
				c++;
				if(c=='Z'+1)
					c='a';
				if(c=='z'+1)
					c='!';
			}
	render_board(w);
	move_xy(1,3);
	wclrtoeol(win);
	wprintw(win,"END the GAME [~]");
	move_xy(1,2);
	wclrtoeol(win);
	wprintw(win,"LEVEL : ");
	for(int u=0;u<level;u++)
		wprintw(win,"* ");
	move_xy(42,3);
	wclrtoeol(win);
	wprintw(win,"CONTINUE later [0]");
	move_xy(1,1);
	wclrtoeol(win);
	wprintw(win,"Enter CHOICE : ");
	wrefresh(win);
	for(;;)
	{
		ch=getchar();
		if(ch=='~' || ch=='0')
		{
			//textcolor(WHITE);
			switch(ch)
			{
				case '0' : savegame(a,w,level);break;
				case '~' : endgame(a,w,level);break;
			}
			break;
		}
		else if(ch>='1' && ch<='9')
		{
			c='0';
			int found=0;
			for(int i=0;i<m;i++)
			{
				for(int j=0;j<m;j++)
					if(w[i][j]==ch)
					{
						found=1;
						move_xy(((j+1)*2)+p,((i+1)*2)+p);
						c=getchar();
							if(c>='1' && c<='9')
							{
								w[i][j]=c;
								style_entry();
								waddch(win,c);
								style_normal();
								move_xy(16,1);
								wrefresh(win);
								break;
							}
					}
				if(c>='1' && c<='9')
					break;
			}
			if(found==0)
			{
				move_xy(16,1);
				wprintw(win,"No cell tagged %c",ch);
				wclrtoeol(win);
				wrefresh(win);
			}
			move_xy(16,1);
		}
		else if((ch>='a'&& ch<='z')||(ch>='A'&& ch<='Z')||(ch>='!'&& ch<=e))
		{
			int found=0;
			for(int i=0;i<m;i++)
			{
				for(int j=0;j<m;j++)
				{
					if(w[i][j]==ch)
					{
						x=i;
						y=j;
						found=1;
						break;
					}
				}
				if(found)
					break;
			}
			if(found==0)
			{
				move_xy(16,1);
				wprintw(win,"No cell tagged %c",ch);
				wclrtoeol(win);
				wrefresh(win);
				continue;
			}
			move_xy(((y+1)*2)+p,((x+1)*2)+p);
			wrefresh(win);
			c=getchar();
				while(!(c>='1' && c<='9'))
					c=getchar();
				w[x][y]=c;
				style_entry();
				waddch(win,c);
				style_normal();
				move_xy(16,1);
				wrefresh(win);
			}
	}
	//textcolor(WHITE);
}
/******************************************************************/
//FUNCTION TO CALCULATE SCORE OF A PLAYER
/******************************************************************/
long score(int a[m][m],int w[m][m],int level)
{
	long scor=0;
	int s[m][m],i,j;
	do
	{solve(a,s);
	}while(verify(s)!=0 && sumer(s)!=0);
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			if(a[i][j]==s[i][j])
				s[i][j]=0;
			if(a[i][j]==w[i][j])
				w[i][j]=0;
			if(w[i][j]!=0 && s[i][j]!=0 && s[i][j]==w[i][j])
				scor++;
		}
	scor*=(5*level);
	return scor;
}
/******************************************************************/
//FUNCTION USING A HIT-&-TRIAL METHOD TO SOLVE THE SUDOKU
/******************************************************************/
void solve(int a[m][m],int s[m][m])
{
	int i,j,y;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			s[i][j]=a[i][j];
		}
	int z[m*m][m],h=1,zc[m*m][m],sc[m][m];
	for(int i=0;i<m*m;i++)
		for(int j=0;j<m;j++)
		{
			z[i][j]=h++;
			if(j==(m-1))
				h=1;
		}
	logic(s,z);
	if(check(s)!=0)
	{
		copier(sc,s,zc,z);
		for(int i=0;i<m;i++)
			for(int j=0;j<m;j++)
				for(y=0;y<m;y++)
					if(zc[m*i+j][y]!=0)
					{
						sc[i][j]=zc[m*i+j][y];
						logic(sc,zc);
						if(check(sc)==0 && verify(sc)==0 && sumer(sc)==0)
							copier(s,sc,z,zc);
						else
							copier(sc,s,zc,z);
					}
	}
}
/******************************************************************/
//FUNCTION TO ALLOW USER TO INPUT A SUDOKU
/******************************************************************/
void enterin(int a[m][m])
{
	char c;
	int p=30;
	draw_sudoku_grid(p);
	int i,j;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			move_xy(((j+1)*2)+p,((i+1)*2)+p);
			c=getchar();
			a[i][j]=char_to_digit(c);
			if(a[i][j]>=1 && a[i][j]<=9)
				wprintw(win,"%d",a[i][j]);
			else
				waddch(win,' ');
		}
	wrefresh(win);
}
/******************************************************************/
//FUNCTION TO DISPLAY THE ELEMENTS OF SUDOKU IN THE BOX
/******************************************************************/
void render_board(int a[m][m])
{
	int p=3;
	char c;
	style_normal();
	draw_sudoku_grid(p);
	int i,j;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			move_xy(((j+1)*2)+p,((i+1)*2)+p);
			//delay(50);
			if(a[i][j]>9)
			{
				c=a[i][j];
				if(c>='1' && c<='9')
				{
					style_entry();
					waddch(win,c);
				}
				else
				{
					style_placeholder();
					waddch(win,c);
				}
			}
			else if(a[i][j]==0)
			{
				style_normal();
				waddch(win,' ');
			}
			else
			{
				style_normal();
				wprintw(win,"%d",a[i][j]);
			}
		}
	style_normal();
	wrefresh(win);
}
/******************************************************************/
//FUNCTION TO SOLVE A SUDOKU USING THREE LOGICS
/******************************************************************/
void logic(int a[m][m], int z[m*m][m])
{
	int i,j,y,k,gl,pl,g,p,e,l,n,r;
	int c[m*m],tz[m];
	for(int o=0;o<81;o++)
		for(int i=0;i<m;i++)
			for(int j=0;j<m;j++)
			{
				c[(m*i)+j]=0;  			//probability no.(c)=0 in beginning
				if(a[i][j]==0)
				{
					gl=(i/3)*3;
					pl=(j/3)*3;
					for(g=gl;g<(gl+3);g++)
						for(p=pl;p<(pl+3);p++)
							for(y=0;y<m;y++)
								if(a[g][p]==z[(m*i)+j][y])
									z[(m*i)+j][y]=0;
					for(k=0;k<m;k++)
						for(y=0;y<m;y++)
							if((a[i][k]==z[(m*i)+j][y])||(a[k][j]==z[(m*i)+j][y]))
								z[(m*i)+j][y]=0;
					for(y=0;y<m;y++)
						if(z[(m*i)+j][y]!=0)
							c[(m*i)+j]++;
					/*
					//Calculational render_board unit
					{
					std::cout<<"("<<i<<j<<")";
					for(y=0;y<m;y++)
					std::cout<<z[(m*i)+j][y];
					std::cout<<"<"<<c[(m*i)+j]<<">";
					}

					 */

				}
				if(a[i][j]==0)
				{
					if(c[(m*i)+j]==1)               //On Surity place the no. in the array
						for(y=0;y<m;y++)
							if(z[(m*i)+j][y]!=0)
								a[i][j]=z[(m*i)+j][y];
				}
				if(a[i][j]==0)
				{
					e=0;
					if(c[(m*i)+j]==2)    	//020050000 & 020050000  eliminate 2,5 in row
						for(k=0;k<m;k++)
							if(k!=i)
								if(c[(m*k)+j]==2)
								{
									for(y=0;y<m;y++)
										if(z[(m*i)+j][y]==z[(m*k)+j][y])
											e=e+1;
									if(e==9)
										for(int l=0;l<m;l++)
											if(l!=k && l!=i)
												for(y=0;y<m;y++)
													if(z[(m*i)+j][y]!=0 && z[(m*k)+j][y]!=0)
														z[(m*l)+j][y]=0;
								}
					e=0;
					if(c[(m*i)+j]==2) 	//020050000 & 020050000  eliminate 2,5 in //column
						for(k=0;k<m;k++)
							if(k!=j)
								if(c[(m*i)+k]==2)
								{
									for(y=0;y<m;y++)
										if(z[(m*i)+j][y]==z[(m*i)+k][y])
											e++;
									if(e==9)
										for(int l=0;l<m;l++)
											if(l!=k && l!=j)
												for(y=0;y<m;y++)
													if(z[(m*i)+j][y]!=0 && z[(m*i)+k][y]!=0)
														z[(m*i)+l][y]=0;
								}
					e=0;
					if(c[(m*i)+j]==2)		//020050000 & 020050000  eliminate 2,5 in box
						for(g=gl;g<(gl+3);g++)
							for(p=pl;p<(pl+3);p++)
								if(g!=i || p!=j)
									if(c[(m*g)+p]==2)
									{
										for(y=0;y<m;y++)
											if(z[(m*i)+j][y]==z[(m*g)+p][y])
												e++;
										if(e==9)
											for(int lo=gl;lo<(gl+3);lo++)
												for(int la=pl;la<(pl+3);la++)
													if((lo!=g || la!=p) && (lo!=i || la!=j))
														for(y=0;y<m;y++)
															if(z[(m*i)+j][y]!=0 && z[(m*g)+p][y]!=0)
																z[(m*lo)+la][y]=0;
									}
					e=0;
					for(y=0;y<m;y++)				//eliminate 8 from row
						tz[y]=z[(m*i)+j][y];
					for(k=0;k<m;k++)
						if(k!=j)
							if(a[i][k]==0)
								for(y=0;y<m;y++)
									if(tz[y]==z[(m*i)+k][y])
										tz[y]=0;
					for(y=0;y<m;y++)
						if(tz[y]!=0)
							e++;
					if(e==1)
						for(y=0;y<m;y++)
							z[(m*i)+j][y]=tz[y];
					e=0;
					for(y=0;y<m;y++)				//eliminate 8 from column
						tz[y]=z[(m*i)+j][y];
					for(k=0;k<m;k++)
						if(k!=i)
							if(a[k][j]==0)
								for(y=0;y<m;y++)
									if(tz[y]==z[(m*k)+j][y])
										tz[y]=0;
					for(y=0;y<m;y++)
						if(tz[y]!=0)
							e++;
					if(e==1)
						for(y=0;y<m;y++)
							z[(m*i)+j][y]=tz[y];
					e=0;
					for(y=0;y<m;y++)				//eliminate 8 from box
						tz[y]=z[(m*i)+j][y];
					for(g=gl;g<(gl+3);g++)
						for(p=pl;p<(pl+3);p++)
							if(p!=j || g!=i)
								if(a[g][p]==0)
									for(y=0;y<m;y++)
										if(tz[y]==z[(m*g)+p][y])
											tz[y]=0;
					for(y=0;y<m;y++)
						if(tz[y]!=0)
							e++;
					if(e==1)
						for(y=0;y<m;y++)
							z[(m*i)+j][y]=tz[y];
				}
			}
}
/******************************************************************/
//FUNCTION TO CHECK THAT ALL ELEMENTS OF SOLVED SUDOKU ARE FILLED
/******************************************************************/
int check(int s[m][m])
{
	int c=0;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
			if(s[i][j]==0)
				c++;
	return c;
}
/******************************************************************/
//FUNCTION TO COPY ELEMENTS OF 9x9 MATRIX & 81x9 TO OTHER MATRICES
/******************************************************************/
void copier(int sc[m][m],int s[m][m],int zc[m*m][m],int z[m*m][m])
{
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			sc[i][j]=s[i][j];
			for(int y=0;y<m;y++)
				zc[m*i+j][y]=z[m*i+j][y];
		}
}
/******************************************************************/
//FUNCTION TO VERIFY THE SOLVED SUDOKU BY ELIMINATING ELEMENTS
/******************************************************************/
int verify(int s[m][m])
{
	int j,y,i,h=1,c=0;
	int arr[m];
	for(y=0;y<m;y++)
		arr[y]=h++;
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<m;j++)
		{
			for(y=0;y<m;y++)
				if(s[i][j]==arr[y])
					arr[y]=0;
		}
		for(y=0;y<m;y++)
			if(arr[y]!=0)
				c++;
		h=1;
		for(y=0;y<m;y++)
			arr[y]=h++;
	}
	return c;
}
/******************************************************************/
//FUNCTION TO VERIFY THE SOLVED SUDOKU BY SUMMING ELELMENTS OF A ROW
/******************************************************************/
int sumer(int s[m][m])
{
	int i,j,c=0,o=0;
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<m;j++)
			o+=s[i][j];
		if(o!=(1+2+3+4+5+6+7+8+9))
			c++;
		o=0;
	}
	return c;
}
/******************************************************************/
//FUNCTION TO CREATE A BOX FOR DISPLAYING SUDOKU
/******************************************************************/
void draw_sudoku_grid(int p)
{
	unsigned char co1=201,co2=187,co3=200,co4=188;
	unsigned char cr1=206,cr2=216,cr3=215,cr4=197;
	unsigned char sr1=186,sr2=205,sr3=179,sr4=196;
	unsigned char ts1=199,ts2=209,ts3=207,ts4=182;
	unsigned char td1=204,td2=185,td3=203,td4=202;
	for(int i=1;i<=m*2+1;i++)
		for(int j=1;j<=m*2+1;j++)
			if(i%2!=0)
			{
				draw_grid_char(i+p,j+p,sr4);draw_grid_char(j+p,i+p,sr3);
				if((i-1)%3==0)
				{draw_grid_char(i+p,j+p,sr2);draw_grid_char(j+p,i+p,sr1);}
			}
	for(int i=1;i<=m*2+1;i++)
		for(int j=1;j<=m*2+1;j++)
			if(i%2!=0 && j%2!=0)
			{
				if(i<=j)
				{draw_grid_char(i+p,j+p,cr4);draw_grid_char(j+p,i+p,cr4);}
				if((i-1)%3==0)
				{
					draw_grid_char(i+p,j+p,cr2);draw_grid_char(j+p,i+p,cr3);
					if(i==1)
					{
						draw_grid_char(i+p,j+p,ts2);draw_grid_char(j+p,i+p,ts1);
						if((j-1)%3==0)
						{draw_grid_char(i+p,j+p,td3);draw_grid_char(j+p,i+p,td1);}
					}
					if(i==m*2+1)
					{
						draw_grid_char(i+p,j+p,ts3);draw_grid_char(j+p,i+p,ts4);
						if((j-1)%3==0)
						{draw_grid_char(i+p,j+p,td4);draw_grid_char(j+p,i+p,td2);}
					}
					if((j-1)%3==0)
					{
						if(i<=j)
						{draw_grid_char(i+p,j+p,cr1);draw_grid_char(j+p,i+p,cr1);}
					}
				}
			}
	for(int i=1;i<=m*2+1;i++)
		for(int j=1;j<=m*2+1;j++)
			if((i-1)%6==0 && (j-1)%6==0)
			{
				if(i<j && j!=m*2+1)
					if(i==1)
						draw_grid_char(i+p,j+p,td3);
					else
						draw_grid_char(i+p,j+p,cr1);
				if(i>j && i!=m*2+1)
					if(j==1)
						draw_grid_char(i+p,j+p,td1);
					else
						draw_grid_char(i+p,j+p,cr1);
			}
	draw_grid_char(1+p,1+p,co1);
	draw_grid_char(1+p,m*2+1+p,co2);
	draw_grid_char(m*2+1+p,1+p,co3);
	draw_grid_char(m*2+1+p,m*2+1+p,co4);
}
/******************************************************************/
//FUNCTION TO CREATE A CIRCLE AND DISPLAY THE INTRODUCTION
/******************************************************************/
void intro()
{
	system("clear");
	unsigned char c=196;
	int y,x1,x2,r;
	for(r=20;r<=23;r++)
		for(y=1;y<=50;y++)
			if(pow(r,2)>pow((y-25),2))
			{
				x1=40+pow((pow(r,2)-pow((y-25),2)),0.5);
				x2=40-pow((pow(r,2)-pow((y-25),2)),0.5);
				//delay(10);
				draw_grid_char(y,x1,c);
				//delay(10);
				draw_grid_char(y,x2,c);
			}
	for(r=20;r<=23;r++)
		for(y=1;y<=80;y++)
			if(pow(r,2)>pow((y-40),2))
			{
				x1=25+pow((pow(r,2)-pow((y-40),2)),0.5);
				x2=25-pow((pow(r,2)-pow((y-40),2)),0.5);
				//delay(10);
				draw_grid_char(x1,y,c);
				//delay(10);
				draw_grid_char(x2,y,c);
			}
	move_xy(33,23);std::cout<<"| S U D O K U |";
	//delay(1000);
	move_xy(28,20);std::cout<<"C + +   P R O J E C T   O N";
	move_xy(33,22);std::cout<<"---------------";
	move_xy(33,24);std::cout<<"---------------";
	//delay(1000);
	move_xy(32,31);std::cout<<"B Y   :   U M A N G";
	//delay(1000);
	move_xy(32,33);std::cout<<"Btech : 1st SEM";
	getchar();
}
/******************************************************************/
//FUNCTION TO CONVERT A CHARACTER TO AN INTEGER (0 TO 9)
/******************************************************************/
int char_to_digit(char c)
{
	int t;
	switch(c)
	{
		case '1': t=1; break;
		case '2': t=2; break;
		case '3': t=3; break;
		case '4': t=4; break;
		case '5': t=5; break;
		case '6': t=6; break;
		case '7': t=7; break;
		case '8': t=8; break;
		case '9': t=9; break;
		default : t=0;
	}
	return t;
}
/******************************************************************/
//FUNCTION TO PRINT A CHARACTER AT A SPECIFIC LOCATION ON SCREEN
/******************************************************************/
static const char* cp437_box_to_utf8(unsigned char c)
{
	switch(c)
	{
		case 179: return u8"│";
		case 180: return u8"┤";
		case 182: return u8"╢";
		case 185: return u8"╣";
		case 186: return u8"║";
		case 187: return u8"╗";
		case 188: return u8"╝";
		case 191: return u8"┐";
		case 192: return u8"└";
		case 193: return u8"┴";
		case 194: return u8"┬";
		case 195: return u8"├";
		case 196: return u8"─";
		case 197: return u8"┼";
		case 199: return u8"╟";
		case 200: return u8"╚";
		case 201: return u8"╔";
		case 202: return u8"╩";
		case 203: return u8"╦";
		case 204: return u8"╠";
		case 205: return u8"═";
		case 206: return u8"╬";
		case 207: return u8"╧";
		case 209: return u8"╤";
		case 215: return u8"╫";
		case 216: return u8"╪";
		case 217: return u8"┘";
		case 218: return u8"┌";
		default: return nullptr;
	}
}

void draw_grid_char(int i,int j,char c)
{
	unsigned char uc=static_cast<unsigned char>(c);
	style_normal();
	// Legacy drawing code passes coordinates as (row, col).
	if(const char* glyph=cp437_box_to_utf8(uc))
		mvwaddstr(win,i,j,glyph);
	else
		mvwaddch(win,i,j,uc);
	wrefresh(win);
}
/******************************************************************/
//FUNCTION TO CREATE TABLE FOR DISPLAYING SCORES AND PLAYER NAMES
/******************************************************************/
void table(int n)
{
	move_xy(28,2);
	std::cout<<"NAME\t\t\t\t\tSCORE";
	unsigned char co1=218,co2=191,co3=192,co4=217;
	unsigned char hr=196,vr=179,cr=197;
	unsigned char t1=194,t2=193,t3=195,t4=180;
	for(int i=3;i<=n*2+3;i++)
		for(int j=1;j<=80;j++)
		{
			if(i%2!=0)
				draw_grid_char(i,j,hr);
			if(j==1||j==80||j==60)
			{
				draw_grid_char(i,j,vr);
				if(j==1 && i%2!=0)
					draw_grid_char(i,j,t3);
				if(j==80 && i%2!=0)
					draw_grid_char(i,j,t4);
			}
			if(i%2!=0 && j==60)
			{
				draw_grid_char(i,j,cr);
				if(i==3)
					draw_grid_char(i,j,t1);
				if(i==n*2+3)
					draw_grid_char(i,j,t2);
			}
		}
	draw_grid_char(3,1,co1);
	draw_grid_char(3,80,co2);
	draw_grid_char(n*2+3,80,co4);
	draw_grid_char(n*2+3,1,co3);
	move_xy(65,49);
	std::cout<<"Continue........";
}
/**********************************************************************/
/***************************NEW FUNCTIONS******************************/
/**********************************************************************/
void create(int a[m][m])
{
	seed_rng_once();
	int i,j,s[m][m],u,c,pl,gl,p,g,y,k=0;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			a[i][j]=0;
			s[i][j]=0;
		}
	draw_sudoku_grid(30);
	while(check(a)!=0)
	{
		i=rand()%9+0;
		j=rand()%9+0;
		u=rand()%9+1;
		//if(a[i][j]==0)
		{
			c=0;
			gl=(i/3)*3;
			pl=(j/3)*3;
			for(g=gl;g<(gl+3);g++)
				for(p=pl;p<(pl+3);p++)
					if(a[g][p]==u)
						c++;
			for(y=0;y<m;y++)
				if(a[i][y]==u)
					c++;
			for(y=0;y<m;y++)
				if(a[y][j]==u)
					c++;
			if(c==0)
				a[i][j]=u;
		}
		k++;
		if(k%1000==0)
		{
			c=0;
			for(int i=0;i<m;i++)
				for(int j=0;j<m;j++)
					if(s[i][j]!=a[i][j])
						c++;
			if(c==0)
			{
				k=0;
				for(int i=0;i<m;i++)
					for(int j=0;j<m;j++)
						a[i][j]=0;
			}
			else
			{
				for(int i=0;i<m;i++)
					for(int j=0;j<m;j++)
						s[i][j]=a[i][j];
			}
		}
	}
	render_preview_board(a);
	do
	{
		i=rand()%9+0;
		j=rand()%9+0;
		if(a[i][j]!=0)
		{
			u=a[i][j];
			a[i][j]=0;
			render_preview_board(a);
			solve(a,s);
		}
	}
	while(verify(s)==0 && sumer(s)==0);
	a[i][j]=u;
	render_preview_board(a);
	std::cout<<"\nCOMPLETED!!";
}
void write(std::fstream &f)
{
	sudoku su;
	char ch='Y';
	ensure_sudoku_file_layout();
	f.open(kSudokuFile,std::ios::binary|std::ios::in);
	f.seekg(0,std::ios::end);
	long sz=f.tellg()/sizeof(su);
	f.close();
	f.open(kSudokuFile,std::ios::binary|std::ios::app);
	while(ch=='Y')
	{
		su.input2(sz+1);
		f.write((char*)(&su),sizeof(su));
		std::cout<<"\nMORE[Y,N]:";
		ch=toupper(getchar());
		std::cout<<ch;
	}
	f.close();
	return;
}
void render_preview_board(int a[m][m])
{
	int p=30;
	char c;
	int i,j;
	style_normal();
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			move_xy(((j+1)*2)+p,((i+1)*2)+p);
			////delay(50);
			if(a[i][j]>9)
			{
				c=a[i][j];
				if(c>='1' && c<='9')
				{
					style_entry();
					waddch(win,c);
				}
				else
				{
					style_placeholder();
					waddch(win,c);
				}
			}
			else if(a[i][j]==0)
			{
				style_normal();
				waddch(win,' ');
			}
			else
			{
				style_normal();
				wprintw(win,"%d",a[i][j]);
			}
		}
	style_normal();
	wrefresh(win);
}
void sudoku::input2(int sz)
{
	system("clear");
	sno=sz;
	std::cout<<"Serial No.:"<<sno<<std::endl;
	seed_rng_once();
	level=rand()%5+1;
	std::cout<<"Level:"<<level;
	std::cout<<"\nSU-DO-KU:";
	create(a);
}
