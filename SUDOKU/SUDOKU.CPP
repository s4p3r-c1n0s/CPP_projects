//INCLUDED HEADER FILES
/******************************************************************/
#include<fstream>
#include<iostream>
#include<curses.h>
#include<cmath>
#include<cstdio>
#include<cstdlib>
#include<cstring>
/******************************************************************/
const int m=9,v=22;
/******************************************************************/
long score(int a[m][m],int w[m][m],int level);
void solve(int a[m][m],int s[m][m]);
void logic(int a[m][m], int z[m*m][m]);
void copier(int sc[m][m],int s[m][m],int zc[m*m][m],int z[m*m][m]);
int verify(int s[m][m]);
int sumer(int s[m][m]);
int check(int s[m][m]);
void display(int a[m][m]);
void enterin(int a[m][m]);
void box(int p);
int convert(char c);
void intro();
void play(int a[m][m],int w[m][m],int level);
void endgame(int a[m][m],int w[m][m],int level);
void prt(int i,int j,char c);
void table(int n);
void set_level(int &level);
void instructions();
void write(std::fstream &f);			//NEW FUNC
void create(int a[m][m]);   			//NEW FUNC
void display2(int a[m][m]);      		//NEW FUNC
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
		display(a);
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
	clrscr();
	sno=sz;
	std::cout<<"Serial No.:"<<sno<<std::endl;
	set_level(level);
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
	f.open("sudoku.dat",std::ios::binary|std::ios::in);
	f.seekg(0,std::ios::end);
	long sz=f.tellg()/sizeof(su);
	f.close();
	f.open("sudoku.dat",std::ios::binary|std::ios::app);
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
	clrscr();
	int l;
	sudoku su;
	std::cout<<"Enter ";
	set_level(l);
	f.open("sudoku.dat",std::ios::binary|std::ios::in);
	while(f.read((char*)(&su),sizeof(su)))
		if(su.r_level()==l)
		{
			clrscr();
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
	f.open("sudoku.dat",std::ios::binary|std::ios::in);
	while(f.read((char*)(&su),sizeof(su)))
	{
		clrscr();
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
	clrscr();
	int s_no;
	sudoku su;
	f.open("sudoku.dat",std::ios::binary|std::ios::in|std::ios::out);
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
/******************************************************************/
//FUNCTION TO RANDOMLY SEARCH RECORDS FROM THE FILE "sudoku.dat"
/******************************************************************/
void searchfile(std::fstream &f,int level,int a[m][m],int w[m][m])
{
	sudoku su;
	int x[20],pos=0;
	f.open("sudoku.dat",std::ios::in|std::ios::binary);
	while(f.read((char*)(&su),sizeof(su)))
		if(su.r_level()==level)
		{
			x[pos]=su.r_sno();
			pos++;
		}
	f.close();
	f.open("sudoku.dat",std::ios::in|std::ios::binary);
	random();
	f.seekg(sizeof(su)*(x[rand()%pos]-1));
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
		//gotoxy(2,(4+(j*2)));
		std::cout<<1+j+(i*v)<<'.';
		//gotoxy(6,(4+(j*2)));
		puts(name);
		//gotoxy(61,(4+(j*2)));
		std::cout<<scr;
	}
	/******************************************************************/
	//FUNCTION TO RETURN THE SCORE OF THE PLAYER
	/******************************************************************/
	long r_scr()
	{
		return scr;
	}
};
/******************************************************************/
//FUNCTION TO ACCEPT THE VALUES OF THE DATA MEMBERS OF CLASS player
/******************************************************************/
void player::input(int a[m][m],int w[m][m],int level)
{
	clrscr();
	std::cout<<"Enter Your name : ";
	gets(name);
	for(int i=0;i<strlen(name);i++)
		name[i]=toupper(name[i]);
	//gotoxy(19,1);
	puts(name);
	scr=score(a,w,level);
	std::cout<<"Your SCORE : "<<scr;
	getchar();
}
/******************************************************************/
//FUNCTION TO INPUT RECORDS TO THE FILE "player.dat"
/******************************************************************/
void writeplayer(std::fstream &f,int a[m][m],int w[m][m],int level)
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
void show(std::fstream &f,int i,int n=v)
{
	clrscr();
	player pl;
	f.open("player.dat",std::ios::in|std::ios::binary);
	table(n);
	for(int j=0;j<n;j++)
	{
		f.seekg((j+(i*v))*sizeof(pl));
		f.read((char*)&pl,sizeof(pl));
		pl.output(i,j);
		//delay(200);
	}
	f.close();
	getchar();
}
/******************************************************************/
//FUNCTION TO RETURN THE NO. OF RECORDS STORED IN FILE "player.dat"
/******************************************************************/
long filesize(std::fstream &f)
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
void readplayer(std::fstream &f)
{
	int i;
	for(i=0;i<filesize(f)/v;i++)
		show(f,i);
	if(filesize(f)%v!=0)
		show(f,i,filesize(f)%v);
	return;
}
/******************************************************************/
//FUNCTION TO SORT RECORDS IN DESCENDING ORDER OF SCORES TO THE FILE
//"player.dat"
/******************************************************************/
void sortplayer(std::fstream &f)
{
	player pl1,pl2;
	long pos1,pos2,fs;
	fs=filesize(f);
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
	f.write((char*)&a,sizeof(a));
	f.write((char*)&w,sizeof(w));
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
	f.read((char*)&a,sizeof(a));
	f.read((char*)&w,sizeof(w));
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
	//textcolor(WHITE);
	intro();
	for(;;)
	{
		clrscr();
		std::cout<<"1) PLAY SUDOKU\n\n2) HIGHSCORES\n\n";
		std::cout<<"3) CONTINUE\n\n4) SET LEVEL\n\n";
		std::cout<<"5) INSTRUCTIONS\n\n6) Write File\n\n";
		std::cout<<"7) Read File\n\n8) Search File";
		std::cout<<"\n\n9) Edit File\n\n0) Exit";
		std::cout<<"\n\n\n\nE n t e r   C h o i c e : ";
		u=getchar();
		while(!(u>='0'&& u<='9'))
			u=getchar();
		switch(u)
		{
			case '1' : searchfile(f,l,a,w);
					   play(a,w,l);break;
			case '2' : readplayer(f);break;
			case '3' : continuegame(a,w,l);
					   play(a,w,l);break;
			case '4' : std::cout<<"\nEnter ";set_level(l);break;
			case '5' : instructions();break;
			case '6' : write(f);break; 		       	//NEW FUNC
			case '7' : readfile(f);break;
			case '8' : searchlevel(f);break;
			case '9' : editfile(f);break;
			case '0' : exit(0);
		}
	}
	return 0;
}
/******************************************************************/
//FUNCTION TO GIVE INSTRUCTIONS TO THE USER
/******************************************************************/
void instructions()
{
	clrscr();
	//gotoxy(20,20);
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
	int s[m][m];
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			if(!(w[i][j]>=1 && w[i][j]<=9))
			{
				c=w[i][j];
				w[i][j]=convert(c);
			}
			if(!(w[i][j]>=1 && w[i][j]<=9))
				w[i][j]=0;
		}
	clrscr();
	//gotoxy(25,20);
	std::cout<<"Y O U R   F I N A L   A N S W E R";
	display(w);
	//delay(1000);
	do
	{
		solve(a,s);
	}while(verify(s)!=0 && sumer(s)!=0);
	clrscr();
	//gotoxy(27,20);
	std::cout<<"S   O   L   U   T   I   O   N";
	display(s);
	getchar();
	//delay(1000);
	writeplayer(f,a,w,level);
	sortplayer(f);
	readplayer(f);
}
/******************************************************************/
//FUNCTION TO ALLOW USER TO SET LEVEL FOR SUDOKU
/******************************************************************/
void set_level(int &level)
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
	clrscr();
	char ch,c='A',e;
	int p=30,n,x,y;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
			if(w[i][j]==0)
			{
				w[i][j]=c;
				c++;
				if(c=='Z'+1)
					c='a';
				if(c=='z'+1)
					c='!';
				e=c;
			}
	display(w);
	//gotoxy(1,50);std::cout<<"END the GAME [~]";
	//gotoxy(1,4);std::cout<<"LEVEL : ";
	for(int u=0;u<level;u++)std::cout<<"* ";
	//gotoxy(60,50);std::cout<<"CONTINUE later [0]";
	//gotoxy(1,1);std::cout<<"Enter CHOICE : ";
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
			for(int i=0;i<m;i++)
			{
				for(int j=0;j<m;j++)
					if(w[i][j]==ch)
					{
						//gotoxy(((j+1)*2)+p,((i+1)*2)+p);
						c=getchar();
						if(c>='1' && c<='9')
						{
							w[i][j]=c;
							//textcolor(RED);
							printf("%c",c);
							//gotoxy(16,1);
							break;
						}
					}
				if(c>='1' && c<='9')
					break;
			}
			//gotoxy(16,1);
		}
		else if((ch>='a'&& ch<='z')||(ch>='A'&& ch<='Z')||(ch>='!'&& ch<=e))
		{
			for(int i=0;i<m;i++)
				for(int j=0;j<m;j++)
					if(w[i][j]==ch)
					{x=i;y=j;}
			//gotoxy(((y+1)*2)+p,((x+1)*2)+p);
			c=getchar();
			while(!(c>='1' && c<='9'))
				c=getchar();
			w[x][y]=c;
			//textcolor(RED);
			printf("%c",c);
			//gotoxy(16,1);
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
	box(p);
	int i,j;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			//gotoxy(((j+1)*2)+p,((i+1)*2)+p);
			c=getchar();
			a[i][j]=convert(c);
			if(a[i][j]>=1 && a[i][j]<=9)
				std::cout<<a[i][j];
			else
				std::cout<<' ';
		}
}
/******************************************************************/
//FUNCTION TO DISPLAY THE ELEMENTS OF SUDOKU IN THE BOX
/******************************************************************/
void display(int a[m][m])
{
	int p=3;
	char c;
	box(p);
	int i,j;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			//gotoxy(((j+1)*2)+p,((i+1)*2)+p);
			//delay(50);
			if(a[i][j]>9)
			{
				c=a[i][j];
				if(c>='1' && c<='9')
				{
					//textcolor(RED);
					printf("%c",c);
				}
				else
				{
					//textcolor(BLUE);
					printf("%c",c);
				}
			}
			else if(a[i][j]==0)
				std::cout<<' ';
			else
			{
				//textcolor(WHITE);
				printf("%d",a[i][j]);
			}
		}
	//textcolor(WHITE);
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
					//Calculational display unit
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
void box(int p)
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
				prt(i+p,j+p,sr4);prt(j+p,i+p,sr3);
				if((i-1)%3==0)
				{prt(i+p,j+p,sr2);prt(j+p,i+p,sr1);}
			}
	for(int i=1;i<=m*2+1;i++)
		for(int j=1;j<=m*2+1;j++)
			if(i%2!=0 && j%2!=0)
			{
				if(i<=j)
				{prt(i+p,j+p,cr4);prt(j+p,i+p,cr4);}
				if((i-1)%3==0)
				{
					prt(i+p,j+p,cr2);prt(j+p,i+p,cr3);
					if(i==1)
					{
						prt(i+p,j+p,ts2);prt(j+p,i+p,ts1);
						if((j-1)%3==0)
						{prt(i+p,j+p,td3);prt(j+p,i+p,td1);}
					}
					if(i==m*2+1)
					{
						prt(i+p,j+p,ts3);prt(j+p,i+p,ts4);
						if((j-1)%3==0)
						{prt(i+p,j+p,td4);prt(j+p,i+p,td2);}
					}
					if((j-1)%3==0)
					{
						if(i<=j)
						{prt(i+p,j+p,cr1);prt(j+p,i+p,cr1);}
					}
				}
			}
	for(int i=1;i<=m*2+1;i++)
		for(int j=1;j<=m*2+1;j++)
			if((i-1)%6==0 && (j-1)%6==0)
			{
				if(i<j && j!=m*2+1)
					if(i==1)
						prt(i+p,j+p,td3);
					else
						prt(i+p,j+p,cr1);
				if(i>j && i!=m*2+1)
					if(j==1)
						prt(i+p,j+p,td1);
					else
						prt(i+p,j+p,cr1);
			}
	prt(1+p,1+p,co1);
	prt(1+p,m*2+1+p,co2);
	prt(m*2+1+p,1+p,co3);
	prt(m*2+1+p,m*2+1+p,co4);
}
/******************************************************************/
//FUNCTION TO CREATE A CIRCLE AND DISPLAY THE INTRODUCTION
/******************************************************************/
void intro()
{
	clrscr();
	unsigned char c=196;
	int y,x1,x2,r;
	for(r=20;r<=23;r++)
		for(y=1;y<=50;y++)
			if(pow(r,2)>pow((y-25),2))
			{
				x1=40+pow((pow(r,2)-pow((y-25),2)),0.5);
				x2=40-pow((pow(r,2)-pow((y-25),2)),0.5);
				//delay(10);
				prt(y,x1,c);
				//delay(10);
				prt(y,x2,c);
			}
	for(r=20;r<=23;r++)
		for(y=1;y<=80;y++)
			if(pow(r,2)>pow((y-40),2))
			{
				x1=25+pow((pow(r,2)-pow((y-40),2)),0.5);
				x2=25-pow((pow(r,2)-pow((y-40),2)),0.5);
				//delay(10);
				prt(x1,y,c);
				//delay(10);
				prt(x2,y,c);
			}
	//gotoxy(33,23);std::cout<<"| S U D O K U |";
	//delay(1000);
	//gotoxy(28,20);std::cout<<"C + +   P R O J E C T   O N";
	//gotoxy(33,22);std::cout<<"---------------";
	//gotoxy(33,24);std::cout<<"---------------";
	//delay(1000);
	//gotoxy(32,31);std::cout<<"B Y   :   U M A N G";
	//delay(1000);
	//gotoxy(32,33);std::cout<<"Btech : 1st SEM";
	getchar();
}
/******************************************************************/
//FUNCTION TO CONVERT A CHARACTER TO AN INTEGER (0 TO 9)
/******************************************************************/
int convert(char c)
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
void prt(int i,int j,char c)
{
	//gotoxy(j,i);std::cout<<c;
}
/******************************************************************/
//FUNCTION TO CREATE TABLE FOR DISPLAYING SCORES AND PLAYER NAMES
/******************************************************************/
void table(int n)
{
	//gotoxy(28,2);
	std::cout<<"NAME\t\t\t\t\tSCORE";
	unsigned char co1=218,co2=191,co3=192,co4=217;
	unsigned char hr=196,vr=179,cr=197;
	unsigned char t1=194,t2=193,t3=195,t4=180;
	for(int i=3;i<=n*2+3;i++)
		for(int j=1;j<=80;j++)
		{
			if(i%2!=0)
				prt(i,j,hr);
			if(j==1||j==80||j==60)
			{
				prt(i,j,vr);
				if(j==1 && i%2!=0)
					prt(i,j,t3);
				if(j==80 && i%2!=0)
					prt(i,j,t4);
			}
			if(i%2!=0 && j==60)
			{
				prt(i,j,cr);
				if(i==3)
					prt(i,j,t1);
				if(i==n*2+3)
					prt(i,j,t2);
			}
		}
	prt(3,1,co1);
	prt(3,80,co2);
	prt(n*2+3,80,co4);
	prt(n*2+3,1,co3);
	//gotoxy(65,49);
	std::cout<<"Continue........";
}
/**********************************************************************/
/***************************NEW FUNCTIONS******************************/
/**********************************************************************/
void create(int a[m][m])
{
	random();
	int i,j,s[m][m],u,c,pl,gl,p,g,y,k=0;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			a[i][j]=0;
			s[i][j]=0;
		}
	box(30);
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
	display2(a);
	do
	{
		i=rand()%9+0;
		j=rand()%9+0;
		if(a[i][j]!=0)
		{
			u=a[i][j];
			a[i][j]=0;
			display2(a);
			solve(a,s);
		}
	}
	while(verify(s)==0 && sumer(s)==0);
	a[i][j]=u;
	display2(a);
	std::cout<<"\nCOMPLETED!!";
}
void write(std::fstream &f)
{
	sudoku su;
	char ch='Y';
	f.open("sudoku.dat",std::ios::binary|std::ios::in);
	f.seekg(0,std::ios::end);
	long sz=f.tellg()/sizeof(su);
	f.close();
	f.open("sudoku.dat",std::ios::binary|std::ios::app);
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
void display2(int a[m][m])
{
	int p=30;
	char c;
	int i,j;
	for(int i=0;i<m;i++)
		for(int j=0;j<m;j++)
		{
			//gotoxy(((j+1)*2)+p,((i+1)*2)+p);
			////delay(50);
			if(a[i][j]>9)
			{
				c=a[i][j];
				if(c>='1' && c<='9')
				{
					//textcolor(RED);
					printf("%c",c);
				}
				else
				{
					//textcolor(BLUE);
					printf("%c",c);
				}
			}
			else if(a[i][j]==0)
				std::cout<<' ';
			else
			{
				//textcolor(WHITE);
				printf("%d",a[i][j]);
			}
		}
	//textcolor(WHITE);
}
void sudoku::input2(int sz)
{
	clrscr();
	sno=sz;
	std::cout<<"Serial No.:"<<sno<<std::endl;
	random();
	level=rand()%5+1;
	std::cout<<"Level:"<<level;
	std::cout<<"\nSU-DO-KU:";
	create(a);
}

