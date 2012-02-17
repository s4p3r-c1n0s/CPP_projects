/*this program do nothing much but give an overview of 
how the system intreprets a rubik's cube and 
how are the steps for each movement performed*/
#include<fstream.h>
#include<conio.h>
#include<dos.h>
#include<ctype.h>
const char v[6]={'I','B','G','Y','O','R'};  
//the 6 faces of the cube
const char dr[2]={'c','a'};                
 //clockwise or anticlockwise
///////////////////////////////////////////////////////////
struct typ3	// represents a corner piece of cube
{
char a[3];
char *pa[3];
}crn[8];	// 8 corner pieces
/////////////////////////////////////////////////////////////
struct typ2	// represents an edge piece of cube
{
char b[2];
char *pb[2];
}edg[12];	// 12 edge pieces
//////////////////////////////////////////////////
struct typ1	// represents the central piece or the node
{
char c;
char s[4];
}cnt[6];	// 6 central nodes
////////////////////////////////////////////////////////
void initialize()	//connects all pieces
{
char t;
char i,j,y;
for(i=0;i<4;i++)
{
crn[i].a[0]=v[0];
crn[i+4].a[0]=v[1];
crn[i+2].a[1]=v[2];
crn[(i+6)%8].a[1]=v[3];
crn[i*2].a[2]=v[4];
crn[(i*2)+1].a[2]=v[5];
}
for(i=0;i<12;i++)
{
y=i+2;
switch(i%6)
{
case 0:edg[i].b[0]=edg[y%12].b[1]=v[0];break;
case 1:edg[i].b[0]=edg[y%12].b[1]=v[1];break;
case 2:edg[i].b[0]=edg[y%12].b[1]=v[2];break;
case 3:edg[i].b[0]=edg[y%12].b[1]=v[3];break;
case 4:edg[i].b[0]=edg[y%12].b[1]=v[4];break;
case 5:edg[i].b[0]=edg[y%12].b[1]=v[5];break;
}
if(y>=8 && (y%2!=0))
{
t=edg[y%12].b[1];
edg[y%12].b[1]=edg[(y%12)-1].b[1];
edg[(y%12)-1].b[1]=t;
}
}
for(y=0;y<6;y++)
cnt[y].c=v[y];
for(y=0;y<6;y++)
for(i=0;i<12;i++)
for(j=0;j<3;j++)
{
if(j<2)
if(edg[i].b[j]==cnt[y].c)
edg[i].pb[j]=&cnt[y].c;
if(i<8)
if(crn[i].a[j]==cnt[y].c)
crn[i].pa[j]=&cnt[y].c;
}
for(i=0;i<6;i++)
{
if(i>=2)
{cnt[i].s[0]=v[0];cnt[i].s[2]=v[1];}
else
{cnt[i].s[0]=v[2];cnt[i].s[2]=v[3];}
}
cnt[0].s[1]=cnt[1].s[3]=cnt[2].s[3]=cnt[3].s[1]=v[5];
cnt[0].s[3]=cnt[1].s[1]=cnt[2].s[1]=cnt[3].s[3]=v[4];
cnt[4].s[1]=cnt[5].s[3]=v[3];
cnt[4].s[3]=cnt[5].s[1]=v[2];
}
//////////////////////////////////////////////////////////
void move()		// defines the move of the cube
{			// c/a as per user's choice
char p,t,d;
int i,j,y,z,l,k;
cout<<"PHASE [I,B,G,Y,O,R] : ";
p=toupper(getch());
cout<<p<<"\nDIRECTION [c/a] : ";
d=tolower(getch());
cout<<d<<endl;
for(y=0;y<6;y++)
if(cnt[y].c==p)
for(y=0;y<6;y++)
if(cnt[y].c==p)
for(i=0;i<12;i++)
for(j=0;j<3;j++)
{
if(j<2)
{
if(cnt[y].c==*edg[i].pb[j])
for(k=0;k<2;k++)
if(k!=j)
{
t=*edg[i].pb[k];
for(l=0;l<4;l++)
if(cnt[y].s[l]==t)
switch(d)
{
case 'c':
for(z=0;z<6;z++)
if(cnt[y].s[(l+1)%4]==cnt[z].c)
edg[i].pb[k]=&cnt[z].c;
break;
case 'a':
for(z=0;z<6;z++)
if(cnt[y].s[(l+3)%4]==cnt[z].c)
edg[i].pb[k]=&cnt[z].c;
break;
}
}
}
if(i<8)
{
if(cnt[y].c==*crn[i].pa[j])
for(k=0;k<3;k++)
if(k!=j)
{
t=*crn[i].pa[k];
for(l=0;l<4;l++)
if(cnt[y].s[l]==t)
switch(d)
{
case 'c':
for(z=0;z<6;z++)
if(cnt[y].s[(l+1)%4]==cnt[z].c)
crn[i].pa[k]=&cnt[z].c;
break;
case 'a':
for(z=0;z<6;z++)
if(cnt[y].s[(l+3)%4]==cnt[z].c)
crn[i].pa[k]=&cnt[z].c;
break;
}
}
}
}
}
////////////////////////////////////////////////////////
void form_cb(char cb[6][3][3])	//forms a cube to be  
{				//presented on screen
char t[8];
int y,i,j,k,l,o,s;
for(y=0;y<6;y++)
{
for(i=0;i<12;i++)
for(j=0;j<3;j++)
{
cb[y][1][1]=cnt[y].c;
if(j<2)
{
if(cb[y][1][1]==*edg[i].pb[j])
for(k=0;k<2;k++)
if(k!=j)
for(l=0;l<4;l++)
if(cnt[y].s[l]==*edg[i].pb[k])
t[l*2]=edg[i].b[j];
}
if(i<8)
{
if(cb[y][1][1]==*crn[i].pa[j])
{
s=0;o=0;
for(k=0;k<3;k++)
if(k!=j)
for(l=0;l<4;l++)
if(cnt[y].s[l]==*crn[i].pa[k])
{
s+=l;
if(l==0 || l==3)
o++;
}
if(o==2)
s+=4;
t[s]=crn[i].a[j];
}
}
}
i=0;j=1;o=0;
while(o<8)
{
cb[y][i][j]=t[o];
if(j==0)
i--;
else if(i==2)
j--;
else if(j==2)
i++;
else if(i==0)
j++;
o++;
}
}
}
////////////////////////////////////////////////////////
void deform_cb(char cb[6][3][3])	// reverse of form
{
int m1=0,m2=0,m3=0,n1=0,n2=2,n3=1;
int y,y1,y2,i,j,i1=0,i2=0,j1=0,j2=0,l;
char ar[2][4][2];
for(y=0;y<6;y++)
cb[y][1][1]=cnt[y].c;
for(y=0;y<2;y++)
for(l=0;l<4;l++)
{
ar[y][l][0]=cnt[y].s[l];
ar[y][l][1]=cnt[y].s[(l+1)%4];
}
i=0;j=2;y=0;
while(i1<8)
{
crn[i1].a[j1%3]=cb[y][i][j];
crn[i1].pa[j1%3]=&cnt[y].c;
for(y1=0;y1<6;y1++)
if(ar[y][i1%4][0]==cnt[y1].c)
{
j1++;
crn[i1].a[j1%3]=cb[y1][m1][n1];
crn[i1].pa[j1%3]=&cnt[y1].c;
}
for(y2=0;y2<6;y2++)
if(ar[y][i1%4][1]==cnt[y2].c)
{
j1++;
crn[i1].a[j1%3]=cb[y2][m2][n2];
crn[i1].pa[j1%3]=&cnt[y2].c;
}
if(j==0) i-=2;
else if(i==2) j-=2;
else if(j==2) i+=2;
i1++; j1++;
if(i1%4==0)
{y++; m1=2; m2=2; i=0; j=2; n1=2; n2=0;}
}
y=0; i=0; j=1; m1=1; m2=1; n1=0; n2=2;
while(i2<12)
{
if(i2<8)
{
edg[i2].b[j2%2]=cb[y][i][j];
edg[i2].pb[j2%2]=&cnt[y].c;
for(y1=0;y1<6;y1++)
if(ar[y][i2%4][0]==cnt[y1].c)
{
j2++;
edg[i2].b[j2%2]=cb[y1][m3][n3];
edg[i2].pb[j2%2]=&cnt[y1].c;
}
i=i+j; j=i-j; i=i-j;
if(i2%2==0)
{
if(j==2) j=0;
else if(j==0) j=2;
}
i2++; j2++;
if(i2%4==0)
{y++; m3=2;}
}
else
{
for(y1=0;y1<6;y1++)
if(ar[0][i2%4][0]==cnt[y1].c)
{
edg[i2].b[j2%2]=cb[y1][m1][n1];
edg[i2].pb[j2%2]=&cnt[y1].c;
}
for(y2=0;y2<6;y2++)
if(ar[0][i2%4][1]==cnt[y2].c)
{
j2++;
edg[i2].b[j2%2]=cb[y2][m2][n2];
edg[i2].pb[j2%2]=&cnt[y2].c;
}
i2++; j2++;
}
}
}
////////////////////////////////////////////////////////
void prnt(char a)
{
char c=219;
switch(a)
{
case 'I':textcolor(WHITE);break;
case 'B':textcolor(BLUE);break;
case 'G':textcolor(GREEN);break;
case 'Y':textcolor(YELLLOW);break;
case 'O':textcolor(BROWN);break;
case 'R':textcolor(RED);break;
}
cprintf("%c",c);
}
////////////////////////////////////////////////////////
void display_cube() // exact display format
{
char cb[6][3][3];
form_cb(cb);
deform_cb(cb);
form_cb(cb);
int i,j=0,y=0,o;
while(y<6)
{
for(i=0;i<3;i++)
for(o=0;o<2;o++)
{
while(y%2==0 || j!=3)
{
if(j==3)
{
cout<<"\t";
j=0;
y++;
}
prnt(cb[y][i][j]);
prnt(cb[y][i][j]);
j++;
}
y--;
j=0;
cout<<"\n";
}
cout<<"\n";
y+=2;
}
textcolor(WHITE);
}
//////////////////////////////////////////////////////////
void main()
{
clrscr();
cout<<"\n";
int i,y,j;
char a;
initialize();
do
{
display_cube();
move();
cout<<"\n";
display_cube();
cout<<"MOVE [Y/N] : ";
a=toupper(getch());
clrscr();
cout<<"\n";
}while(a!='N');
}
