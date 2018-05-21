#include "stdio.h"
#include "time.h"
#include "conio.h"
#include "windows.h"
#include "winnt.h"
#include "MD5.h"

#define len_init 4
#define MAP_X 70
#define MAP_Y 35

int background(HANDLE hout);
int start(HANDLE hout);
int paintmap(HANDLE hout);
int resnake(void);
int snakeinit(HANDLE hout);
int point(HANDLE hout);
int over(HANDLE hout);
int repaint(HANDLE hout);
int redir(int *x,int *y,int dir);
int checksum(int argc,char *argv[],HANDLE hout);

int mapx,mapy;

static int point_index=0;
char filename[100];
int map[MAP_Y][MAP_X]; 	
uint8_t checkresult[16];
char mymap[] ="1111111111111111111111111111111111111111111111111\n\
1000000000000000000000000000000000000000000000001\n\
1000000000000000000000000000000000000000000000001\n\
1000000000011000011000000000000000111111000000001\n\
1000000000001100110000000000000000110001100000001\n\
1000000000000111100000000000000000110000110000001\n\
1000000000000011000000000000000000110000110000001\n\
1000000000001101100000000000000000110000110000001\n\
1000000000011000110000000000000000110001100000001\n\
1000000000110000011000000000000000111110000000001\n\
1000000000000000000000000000000000000000000000001\n\
1000000000000000000000000000000000000000000000001\n\
1000000000000000000000000000000000000000000000001\n\
1000000000000000000000000000000000000000000000001\n\
1000000111100000000001111111000000000001111000001\n\
1000001111110000000001111111000000000011111100001\n\
1000011000000000000001100000000000000110000000001\n\
1000001100000000000001100000000000001100000000001\n\
1000000110000000000001111110000000001100000000001\n\
1000000011000000000001111110000000001100000000001\n\
1000000001100000000001100000000000001100000000001\n\
1000000000110000000001100000000000001100000000001\n\
1000000000011000000001100000000000000110000000001\n\
1000001111110000000001111111000000000011111100001\n\
1000000111100000000001111111000000000001111000001\n\
1000000000000000000000000000000000000000000000001\n\
100000000000000000000000000000000000000000000000133333333333333\n\	
100000000000000000000000000000000000000000000000000000000000003\n\
111111111111111111111111111111111111111111111111133333333333333";			//0 space	1 block		2 snake		-2 point 	-1 bound

char pic[]="\n\n\t  ______     ____    ___    ___       ___  __  _______\n\
\t / _____\\    |   \\   | |   |   \\      | | / /  \\  ____\\ \n\
\t \\ \\_____    | |\\ \\  | |   | |\\ \\     | |/ /    \\ \\_____\n\
\t  \\_____ \\   | | \\ \\ | |   | |_\\ \\    |   /      \\  ____\\ \n\
\t   _____\\ \\  | |  \\ \\| |   | ____ \\   | |\\ \\      \\ \\_____\n\
\t   \\______/  |_|   \\___|   |_|   \\_\\  |_| \\_\\      \\______\\ ";
uint8_t hash[16] = {0x8d,0xcd,0x2c,0xbd,0x9c,0x8a,0xd1,0x8c,0xb6,0xcc,0x74,0xd1,0x1c,0x98,0x92,0x8c};
int dir[MAP_X * MAP_Y];			//head	tail	0 left  1 right  2 up  3 down
int mypoint[60];
int pointpos[]={16,8, 22, 19, 32, 2, 18 ,7, 22, 28 ,84, 13, 90, 7, 40, 15, 28, 22, 42, 13,82, 17, 68, 25, 22 ,3, 14, 7, 28, 21, 40, 3, 52, 13, 72, 19, 92, 19, 26, 18,24, 25, 18, 12,  58, 18, 74, 24, 76, 16, 32, 12, 50, 5, 4, 17, 74,14,110, 28};
int myflag[]={17, 4, 8, 52, 58, 102, 113, 53, 13, 66, 66, 109, 2, 36, 6, 48, 115, 126, 69, 55, 20, 5, 111, 18, 117, 13, 82, 112, 61, 179};
const int mov[4][2]={{-1,0},{1,0},{0,1},{0,-1}};
char result[31];
int len,tim_num;
COORD snake[2];			//head	tail

int main(int argc,char *argv[])
{
	int speed,flag=1,ret;
	unsigned long count=0;
	char input;
	HANDLE hout,hin;
	MSG msg;

	mapx=50;
	mapy=30;
	hin=GetStdHandle(STD_INPUT_HANDLE);
	hout=GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hout,FOREGROUND_GREEN);
	system("cls");
	checksum(argc,argv,hout);
	memset(map,0,sizeof(map));
	speed=start(hout);
	background(hout);
	paintmap(hout);
	snakeinit(hout);

	point_index = 0;
	point(hout);
	Sleep(1000);
	tim_num=SetTimer(NULL,0,75-speed,NULL);

	while(1)
	{
		if(GetMessage(&msg,NULL,0,0)>0)
		{
			switch(msg.message)
			{
				case WM_TIMER:
					ret=repaint(hout);
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
	i=1;
	j=1;
	n = 0;
	while(mymap[n]!='\0')
	{
		ch = mymap[n];
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
		n++;
	}
	
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
			mypoint[point_index] = pos.X;
			mypoint[point_index+1] = pos.Y;
			point_index += 2;
			if(60 == point_index)
				over(hout);
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
	COORD pos_flag;
	char input;
	int i,j=0;
	int win=0;
	int cache,sum=0;

	pos.X=mapx;
	pos.Y=mapy/2;
	KillTimer(NULL,tim_num);
	Sleep(1000);

	system("cls");

	pos_flag.X = 1;
	pos_flag.Y = 1;
	SetConsoleCursorPosition(hout,pos_flag);
	if(point_index == 60)
	{
		win = 1;

		for(i=0;i<point_index;i++)
			sum+=mypoint[i];
		sum = sum % 128;
		for(i=0;i<point_index;i+=2)
		{
			cache = mypoint[i]+mypoint[i+1];
			printf("%c",cache ^ myflag[i/2] ^ sum);	
		}
	}

	SetConsoleCursorPosition(hout,pos);
	SetConsoleTextAttribute(hout,FOREGROUND_GREEN);
	if(win)
		printf("Congratulation!!!");
	else
		printf("You Failed\n");
	
	fflush(stdin);
	input=getch();
	exit(0);
}

int point(HANDLE hout)
{
	COORD pos;
	COORD score;

	score.X = 1;
	score.Y = 0;
	SetConsoleTextAttribute(hout,FOREGROUND_GREEN);
	SetConsoleCursorPosition(hout,score);
	printf("SCORE: %d",point_index/2);

	pos.X = pointpos[point_index];
	pos.Y = pointpos[point_index+1];

	SetConsoleTextAttribute(hout,FOREGROUND_RED);
	SetConsoleCursorPosition(hout,pos);
	printf("¡ï");
	map[pos.Y][pos.X/2]=-2;

	return 0;
}

int start(HANDLE hout)
{
	COORD pos;
	int speed=0;
	int off=0;
	int i;
	
	pos.Y=10;
	printf("%s",pic);
	printf("\n");

speed_input:
	pos.X=30;
	SetConsoleCursorPosition(hout,pos);
	printf("Input Speed(0-4)\n");
	pos.Y++;
	SetConsoleCursorPosition(hout,pos);
	scanf("%d",&speed);
	if(speed<=4 && speed>=0)
		return speed;
	else
	{
		SetConsoleCursorPosition(hout,pos);
		printf("\t\t\t\t");
		pos.Y--;
		goto speed_input;
	}
}

int checksum(int argc,char *argv[],HANDLE hout)
{
	HANDLE hfile;
	IMAGE_DOS_HEADER *dosheader;
	IMAGE_NT_HEADERS32 *peheader;
	IMAGE_SECTION_HEADER **psection;
	IMAGE_SECTION_HEADER *pcache;
	char *database;
	DWORD retsize;
	int tablenum;
	LARGE_INTEGER filesize;
	unsigned int optheaderoff;
	unsigned int headeroff;
	unsigned int tableoff;
	unsigned int dataoff;
	int maskl,maskr;
	unsigned char *p=NULL;
	int size,i;

	GetModuleFileName(NULL,filename,100);
	if( (hfile = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL) ) == NULL)
	{
		printf("OPEN FILE FAIL\n");
		exit(-1);
	}
	size = GetFileSize(hfile,NULL);
	p = (unsigned char*)malloc(size+10);
	if( !ReadFile(hfile,p,size,&retsize,NULL) )
		printf("Read File Error\n");

	dosheader = (IMAGE_DOS_HEADER*)p;
	headeroff = dosheader->e_lfanew;
	peheader = (IMAGE_NT_HEADERS32*)&p[headeroff];
	tablenum = peheader->FileHeader.NumberOfSections;
	optheaderoff = headeroff + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER);
	tableoff = optheaderoff + peheader->FileHeader.SizeOfOptionalHeader;

	psection = (IMAGE_SECTION_HEADER**)malloc(tablenum * sizeof(IMAGE_SECTION_HEADER*));

	for(i=0;i<tablenum;i++)
	{
		psection[i] = (IMAGE_SECTION_HEADER*)( &p[tableoff] + i*sizeof(IMAGE_SECTION_HEADER) );
		if( !strcmp( (char*)psection[i],".text") )
			dataoff = i;
	}

	maskl = psection[dataoff]->PointerToRawData;
	maskr = maskl +  psection[dataoff]->SizeOfRawData;
	
	md5((uint8_t*)&p[maskl],maskr-maskl,checkresult);
	for (i = 0; i < 16; i++)
        	if(checkresult[i]!=hash[i])
		{
			printf("Checksum Error\n");
			for(i=0;i<16;i++)
				printf("%2.2x",checkresult[i]);
			exit(-1);
		}
	
	CloseHandle(hfile);
	return 0;
}
