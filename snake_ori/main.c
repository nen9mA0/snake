#include "stdio.h"
#include "time.h"
#include "conio.h"
#include "windows.h"

#define len_init 4
#define MAP_LEN 100

int background(HANDLE hout);
int start(HANDLE hout);
int paintmap(HANDLE hout);
int resnake(void);
int snakeinit(HANDLE hout);
int point(HANDLE hout);
int over(HANDLE hout);
int repaint(HANDLE hout);
int redir(int *x,int *y,int dir);

int mapx,mapy;
int map[MAP_LEN][MAP_LEN];			//0 space	1 block		2 snake		-2 point 	-1 bound
int dir[MAP_LEN * MAP_LEN];			//head	tail	0 left  1 right  2 up  3 down
const int mov[4][2]={{-1,0},{1,0},{0,1},{0,-1}};
int len,tim_num;
COORD snake[2];			//head	tail

int main(void)
{
	int speed,flag=1,ret;
	unsigned long count=0;
	char input;
	HANDLE hout,hin;
	MSG msg;

	mapx=45;
	mapy=30;
	hin=GetStdHandle(STD_INPUT_HANDLE);
	hout=GetStdHandle(STD_OUTPUT_HANDLE);

restart:
	SetConsoleTextAttribute(hout,FOREGROUND_GREEN);
	system("cls");
	memset(map,0,sizeof(map));
	speed=start(hout);
	background(hout);
	paintmap(hout);
	snakeinit(hout);

	point(hout);
	Sleep(1000);
	tim_num=SetTimer(NULL,0,120-speed*speed,NULL);

	while(1)
	{
		if(GetMessage(&msg,NULL,0,0)>0)
		{
			switch(msg.message)
			{
				case WM_TIMER:
					ret=repaint(hout);
					if(1==ret)
						goto restart;
					break;
			}
		}
		if(kbhit())
		{
			input=getch();
			fflush(stdin);
			switch(input)
			{
				case 'a':
					if(dir[0]!=1)
						dir[0]=0;
					break;
				case 'd':
					if(dir[0]!=0)
						dir[0]=1;
					break;
				case 'w':
					if(dir[0]!=2)
						dir[0]=3;
					break;
				case 's':
					if(dir[0]!=3)
						dir[0]=2;
					break;
			}
		}
	}

	return 0;
}


int background(HANDLE hout)
{
	COORD pos;
	FILE *fp;
	int i,j,n;
	char ch;

	memset(map,-1,sizeof(map));

	fp=fopen("map.txt","r");

	i=1;
	j=1;
	while((ch=fgetc(fp))!=EOF)
	{
		if('\n'==ch)
		{
			j++;
			i=1;
		}
		else
		{
			map[j][i]=ch-'0';
			i++;
		}
	}
	
	mapx=i;
	mapy=j+1;

	system("cls");
	pos.X=0;
	pos.Y=0;
	SetConsoleCursorPosition(hout,pos);

	return 0;
}

int snakeinit(HANDLE hout)
{
	COORD pos;
	int x,y,i;
	int flag;
	srand((unsigned int)time(NULL));

	do 
	{
		flag = 1;
		x=rand()%(mapx/2)+mapx/4;
		y=rand()%(mapy/4)*2+mapy/4;
		for(i=-1;i<len_init;i++)
		{
			if(map[y][x+i]==1)
				flag = 0;
		}
	}while(flag==0);

	pos.X=x*2;
	pos.Y=y;

	len=len_init;

	for(i=0;i<len_init;i++)
		dir[i]=0;

	SetConsoleCursorPosition(hout,pos);
	SetConsoleTextAttribute(hout,FOREGROUND_BLUE);

	for(i=0;i<len_init;i++)
		printf("¡ö");

	for(i=x;i<x+4;i++)
		map[y][i]=2;

	snake[0].X=x;
	snake[0].Y=y;
	snake[1].X=x+len_init-1;
	snake[1].Y=y;

	return 0;
}

int paintmap(HANDLE hout)
{
	COORD pos;
	int i,j;
	
	pos.X=0;
	pos.Y=0;
	SetConsoleCursorPosition(hout,pos);

	SetConsoleTextAttribute(hout,FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN);

	for(j=0;j<mapy;j++)
		for(i=0;i<mapx;i++)
			if(map[j][i]==1)
			{
 				pos.X=i*2;
				pos.Y=j;
				SetConsoleCursorPosition(hout,pos);
				printf("¡ö");
			}
	printf("\n");

	return 0;
}

int redir(int *x,int *y,int dir)
{
	int i,j,k;
	i = *x;
	j = *y;
	switch(dir)
	{
		case 0:
			while(map[j][i+1]!=-1)
				i++;
			break;
		case 1:
			while(map[j][i-1]!=-1)
				i--;
			break;
		case 2:
			while(map[j-1][i]!=-1)
				j--;
			break;
		case 3:
			while(map[j+1][i]!=-1)
				j++;
			break;
	}
	*x = i;
	*y = j;
	return 0;
}

int repaint(HANDLE hout)
{
	int newhx,newhy,newtx,newty;				//new head	new tail
	int i,j,ret=0;
	COORD pos;

	newhx=snake[0].X+mov[ dir[0] ][0];
	newhy=snake[0].Y+mov[ dir[0] ][1];

	if(map[newhy][newhx]>0)
		ret=over(hout);
	else
	{
		if(-1 == map[newhy][newhx])
			redir(&newhx,&newhy,dir[0]);

		newtx=snake[1].X+mov[ dir[len-1] ][0];
		newty=snake[1].Y+mov[ dir[len-1] ][1];

		if(-1 == map[newty][newtx])
			redir(&newtx,&newty,dir[len-1]);

		pos.X=newhx*2;
		pos.Y=newhy;
		SetConsoleTextAttribute(hout,FOREGROUND_BLUE);
		SetConsoleCursorPosition(hout,pos);
		printf("¡ö");

		snake[0].X=newhx;
		snake[0].Y=newhy;
		if(-2!=map[newhy][newhx])
		{
			pos.X=snake[1].X*2;
			pos.Y=snake[1].Y;
			SetConsoleCursorPosition(hout,pos);
			printf(" ");
			map[snake[1].Y][snake[1].X]=0;
			snake[1].X=newtx;
			snake[1].Y=newty;
		}
		else
		{
			len++;
			point(hout);
		}

		map[newhy][newhx]=2;
		
		for(i=len-1;i>0;i--)
			dir[i]=dir[i-1];
	}

	return ret;
}

int over(HANDLE hout)
{
	COORD pos;
	char input;

	pos.X=mapx;
	pos.Y=mapy/2;
	KillTimer(NULL,tim_num);
	Sleep(1000);

	system("cls");
	SetConsoleCursorPosition(hout,pos);
	printf("You Failed");
	pos.Y++;
	SetConsoleCursorPosition(hout,pos);
	printf("r(Restart)\n");

	fflush(stdin);
	input=getch();
	if('r'!=input)
		exit(0);
	else
		return 1;
}

int point(HANDLE hout)
{
	COORD pos;
	do
	{
		pos.X=(rand()%(mapx-2)+1)*2;
		pos.Y=rand()%(mapy-2)+1;
	}while(0 != map[pos.Y][pos.X/2]);


	SetConsoleTextAttribute(hout,FOREGROUND_RED);
	SetConsoleCursorPosition(hout,pos);
	printf("¡ï");
	map[pos.Y][pos.X/2]=-2;

	return 0;
}

int start(HANDLE hout)
{
	COORD pos;
	FILE *fp;
	char str[100];
	int speed=0;
	
	pos.X=20;
	pos.Y=2;

	fp=fopen("paint.txt","r");
	while(NULL!=fgets(str,100,fp))
	{
		SetConsoleCursorPosition(hout,pos);
		pos.Y++;
		printf("%s",str);
	}
	printf("\n");
	fclose(fp);

speed_input:
	pos.X=40;
	SetConsoleCursorPosition(hout,pos);
	printf("Input Speed(0-4)\n");
	pos.Y++;
	SetConsoleCursorPosition(hout,pos);
	scanf("%d",&speed);
	if(speed<=7 && speed>=0)
		return speed;
	else
	{
		SetConsoleCursorPosition(hout,pos);
		printf("\t\t\t\t");
		pos.Y--;
		goto speed_input;
	}
}
