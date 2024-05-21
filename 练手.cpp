#include<stdio.h>
#include<conio.h>
#include<time.h>
#include<windows.h>　
#define width 20
#define length 40
//代表四个方位的数值
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
//当前蛇头方向（初始值）
int head = RIGHT;
void canvas();
void draw(int x,int y,char);
void draw(int x, int y, int a);
void draw(int x, int y,const char a[]);
//head数值代表头指向的方位，1,2,3,4，分别代表上下左右
void move(struct coordinate * snake_length,int head,int array_length);
void face();
//获取范围为a-b之间的随机数
int random(int a, int b);
//失败条件判断
bool lose(struct coordinate snake_length[],int array_length);
//判断是否得分
bool isscore(struct coordinate *ball, struct coordinate *snake_length);
struct coordinate {
	int x;
	int y;
};

DWORD WINAPI InputThread(LPVOID lpParam) {
	while (1) {
		if (_kbhit())
			face();
	}
}
int main() {
	int speed = 100;
	canvas();
	draw(length + 15-5, width / 2, "SCORE:0");
	HANDLE hInputThread;
	DWORD inputThreadId;
	// 创建玩家输入监控线程，这段代码是为了转向更加流畅的优化，可去
	hInputThread = CreateThread(NULL, 0, InputThread, NULL, 0, &inputThreadId);
	if (hInputThread == NULL) {
		printf("Error creating input thread\n");
		return 1;
	}
	srand((unsigned int)time(NULL));
	int snake = 3;
	int score = 0;
	struct coordinate* snake_length = (struct coordinate*)malloc(snake*sizeof(struct coordinate*));
	struct coordinate register ball;	
	//球位置初始化
	ball.x = random(2, length-2);
	ball.y = random(2, width-2);
	draw(ball.x, ball.y, '*');
	//初始的蛇身位置
	snake_length[0].x = random(3, length-2);
	snake_length[0].y = random(3, width-2);
	draw(snake_length[0].x, snake_length[0].y, '&');
	snake_length[1].x = snake_length[0].x - 1;
	snake_length[1].y = snake_length[0].y;
	draw(snake_length[1].x, snake_length[1].y, '&');
	snake_length[2].x = snake_length[0].x - 2;
	snake_length[2].y = snake_length[0].y;
	draw(snake_length[2].x, snake_length[2].y, '&');
	//循环，随时监控游戏各项数据
	while (1) {
		//去掉尾部的部分
		draw(snake_length[snake-1].x, snake_length[snake-1].y, ' ');
		//更新位置
		move(snake_length, head, snake);
		//头部向前移动
		draw(snake_length[0].x, snake_length[0].y, '&');
		//假如得分
		if (isscore(&ball,snake_length)) {
			draw(ball.x, ball.y, '\b');
			score += 1;//分数+1
			snake += 1;//蛇身长+1
			//扩大数组空间
			snake_length = (struct coordinate*)realloc(snake_length, snake * sizeof(struct coordinate*));
			//重置球位置
			ball.x = random(2, length-2);
			ball.y = random(2, width-2);
			//增加蛇身长度的位置
			switch (head) {
			case UP:
				(*(snake_length + snake - 1)).y = (*(snake_length + snake - 2)).y - 1;
				break;
			case DOWN:
				(*(snake_length + snake-1)).y = (*(snake_length + snake - 2)).y + 1;
				break;
			case RIGHT:
				(*(snake_length + snake-1)).x = (*(snake_length + snake - 2)).x - 1;
				break;
			case LEFT:
				(*(snake_length + snake-1)).x = (*(snake_length + snake - 2)).x + 1;
				break;
			}
			while (1) {
				int flag = 0;
				for (int t = 0; t < snake; t++) {
					if ((snake_length + snake)->x == ball.x && (snake_length + snake)->y == ball.y) {
						flag = 1;
						break;
					}
				}
				if (flag == 0) {
					break;
				}
				else {
					ball.x = random(2, length - 2);
					ball.y = random(2, width - 2);
				}
			}
			draw(ball.x, ball.y, *"*");//再画一个球
		}
		draw(length + 16, width / 2, score);
		//Sleep(50);
		if (lose(snake_length, snake)) {
			draw(length / 2, width / 2, 'G');
			draw(length / 2 + 1, width / 2, 'G');
			CloseHandle(hInputThread);
			free(snake_length);
			break;
		}
		//根据蛇头方向改变时间流速（因为上下和左右的长度不一样）
		switch (head) {
		case UP:
			speed = 75;
			break;
		case DOWN:
			speed = 75;
			break;
		case RIGHT:
			speed = 50;
			break;
		case LEFT:
			speed = 50;
			break;
		}
		Sleep(speed);
	}
}
//定义画布
void canvas() {
	for (int i = 0; i < width; i++) {
		if (i == 0 || i == width - 1) {
			for (int j = 0; j < length - 1; j++) {
				printf("#");
			}
			printf("#\n");
		}
		else {
			printf("#");
			for (int j = 0; j < length-2; j++) {
				printf(" ");
			}
			printf("#\n");
		}
	}
}
//画图函数
//两个思路：一、每200ms画一张图（可能造成光标闪烁）
//二、利用WINDOWS内置光标移动方法移动光标到x,y位置
//本次编程使用第二条
void draw(int x, int y,char a) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	printf("%c",a);
}
void draw(int x, int y, int a) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	printf("%d", a);
}
void draw(int x, int y, const char a[]) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	printf("%s", a);
}
//蛇头每秒向着蛇头的方向前进一格
//传入蛇身地址，保留数据改动
void move(struct coordinate * snake_length,int head,int array_length) {
	int flagx, flagy;
	flagx = snake_length[0].x;
	flagy = snake_length[0].y;
	//蛇移动逻辑
	if (head == UP) {
		snake_length[0].y = snake_length[0].y - 1;
	}
	else if (head == DOWN) {
		snake_length[0].y = snake_length[0].y + 1;
	}
	else if (head == RIGHT) {
		snake_length[0].x = snake_length[0].x + 1;
	}
	else if (head == LEFT) {
		snake_length[0].x = snake_length[0].x - 1;
	}
	for (int i = array_length - 1; i > 0; i--) {
		//边界检查逻辑
		if (snake_length[array_length - 1 - i].x > length-2) {
			snake_length[array_length - 1 - i].x = 1;
		}
		else if (snake_length[array_length - 1 - i].x < 1) {
			snake_length[array_length - 1 - i].x = length - 2;
		}
		if (snake_length[array_length - 1 - i].y > width-2) {
			snake_length[array_length - 1 - i].y = 1;
		}
		else if (snake_length[array_length - 1 - i].y < 1) {
			snake_length[array_length - 1 - i].y = width - 2 ;
		}
		if (i != 1) {
			snake_length[i].x = snake_length[i - 1].x;
			snake_length[i].y = snake_length[i - 1].y;
		}
		else {
			snake_length[i].x = flagx;
			snake_length[i].y = flagy;
		}
	}
}
//改变蛇头的方向的函数
void face() {
	switch (_getch()) {
	case 72:
		head = UP;
		break;
	case 80:
		head = DOWN;
		break;
	case 75:
		head = LEFT;
		break;
	case 77:
		head = RIGHT;
		break;
	default:
		break;
	}
}
bool isscore(struct coordinate* ball, struct coordinate* snake_length) {
	if (ball->x == snake_length->x && ball->y == snake_length->y) {
		return true;
	}
	else {
		return false;
	}
}
int random(int a, int b) {
	return a + rand() % (b - a + 1);
}
bool lose(struct coordinate snake_length[], int array_length) {
	for (int i = 1; i < array_length; i++) {
		if (snake_length[0].x == snake_length[i].x && snake_length[0].y == snake_length[i].y) {
			return true;
		}
	}
	return false;
}