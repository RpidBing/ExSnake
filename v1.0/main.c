#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#define MAP_X 32 // 水平 16
#define MAP_Y 18 // 垂直 9

#define UP    'w'
#define DOWN  's'
#define LEFT  'a'
#define RIGHT 'd'

#ifdef _WIN32
#else
static struct termios orig_termios;  // 保存原始终端设置
#endif
static char map_buffer[MAP_Y][MAP_X];     // 创建一个缓冲区
static int control_count;            // 操作计数器

// 蛇头
typedef struct SnakeHead
{
    int x;
    int y;
    int length;
    char direction;
} SnakeHead;

// 蛇身
typedef struct SnakeBody
{
    int x;
    int y;
    char direction;
} SnakeBody;

// 食物
typedef struct SnakeFood
{
    int x;
    int y;
    int exist;
} SnakeFood;

void snake_setup(SnakeHead *head, SnakeBody body[(MAP_X - 2) * (MAP_Y - 2)], SnakeFood *food)
{
    head->x = 4;
    head->y = 6;
    head->length = 3;
    head->direction = RIGHT;
    printf("----- 蛇头初始化 OK  -----\n");
    for(int i = 0; i < 3; i++){
        body[i].x = head->x - i - 1;
        body[i].y = head->y;
        body[i].direction = head->direction;
    }
    printf("----- 蛇身初始化 OK  -----\n");
    food->x = 1 + rand() % (MAP_X - 2);
    food->y = 1 + rand() % (MAP_Y - 2);
    if(food->y == 6)food->y--;
    food->exist = 1;
    printf("----- 食物初始化 OK  -----\n");
    return;
}

void snake_move(SnakeHead *head, SnakeBody body[(MAP_X - 2) * (MAP_Y - 2)]){
    switch(head->direction){
        case UP   : if(head->direction != DOWN)  head->y -= 1; break;
        case DOWN : if(head->direction != UP)    head->y += 1; break;
        case LEFT : if(head->direction != RIGHT) head->x -= 1; break;
        case RIGHT: if(head->direction != LEFT)  head->x += 1; break;
        default:break; 
    }
    for(int i = 0; i < head->length;i++){
        switch(body[i].direction){
        case UP   : if(body[i].direction != DOWN)  body[i].y -= 1; break;
        case DOWN : if(body[i].direction != UP)    body[i].y += 1; break;
        case LEFT : if(body[i].direction != RIGHT) body[i].x -= 1; break;
        case RIGHT: if(body[i].direction != LEFT)  body[i].x += 1; break;
        default:break;
        }
    }
    return;
}

void game_render(SnakeHead head, char map[MAP_Y][MAP_X], SnakeBody body[(MAP_X - 2)*(MAP_Y - 2)], SnakeFood food){
    for(int i = 1; i < MAP_Y - 1; i++)
        for (int j = 1; j < MAP_X - 1; j++)
            map[i][j] = ' ';
    
    for (int i = 0; i < head.length; i++){
        map[body[i].y][body[i].x] = 'B';
    }

    map[head.y][head.x] = 'H';

    if(food.exist == 0){
        map[food.y][food.x] = ' ';   
    }else{
        map[food.y][food.x] = 'F';
    }
    
    printf("body x:%d y:%d len:%d\n",body[0].x,body[0].y,head.length);
    printf("head x:%d y:%d\n",head.x,head.y);

    // 将地图内容复制到缓冲区
    memcpy(map_buffer, map, sizeof(map_buffer));
}

void game_show(char map[MAP_Y][MAP_X]) {

    // 打印缓冲区内容到终端
    for (int i = 0; i < MAP_Y; i++) {
        for (int j = 0; j < MAP_X; j++) {
            printf("%c", map_buffer[i][j]);
        }
        printf("\n");
    }
}

// 重要函数实现 ！！！！
// 设置终端为非阻塞模式
void set_conio_terminal_mode() {
#ifdef _WIN32
    // Windows 不需要这些设置
#else
    struct termios new_settings;
    tcgetattr(0, &orig_termios);
    tcgetattr(0, &new_settings);
    new_settings.c_lflag &= ~ICANON; // 取消规范模式
    new_settings.c_lflag &= ~ECHO;   // 不回显
    tcsetattr(0, TCSANOW, &new_settings);
#endif
}

void reset_terminal_mode() {
#ifdef _WIN32
    // Windows 不需要这些设置
#else
    tcsetattr(0, TCSANOW, &orig_termios); // 恢复终端设置
#endif
}

int kbhit() {
#ifdef _WIN32
    return _kbhit(); // 使用 conio.h 提供的 _kbhit()
#else
    struct termios oldt, newt;
    int oldf;
    
    // Linux 的实现
    tcgetattr(0, &oldt);
    newt = oldt; 
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newt);
    oldf = fcntl(0, F_GETFL, 0);
    fcntl(0, F_SETFL, oldf | O_NONBLOCK);
    
    int ch = getchar();
    tcsetattr(0, TCSANOW, &oldt);
    fcntl(0, F_SETFL, oldf);
    
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1; // 有输入
    }
    return 0; // 没有输入
#endif
}

void snake_change_direction(SnakeHead *head, char direction){
    if((head->direction == UP) && direction == DOWN)return;
    if((head->direction == DOWN) && direction == UP)return;
    if((head->direction == LEFT) && direction == RIGHT)return;
    if((head->direction == RIGHT) && direction == LEFT)return;
    control_count = head->length;
    head->direction = direction;
    return;
}

void snake_body_change_direction(SnakeHead head, SnakeBody body[(MAP_X - 2) * (MAP_Y - 2)]){
    for(int i = head.length - 1; i > 0;i--){
        body[i].direction = body[i - 1].direction;
    }
    body[0].direction = head.direction;
}

void snake_collision(SnakeHead *head, SnakeFood *food, SnakeBody body[(MAP_X - 2) * (MAP_Y - 2)]){
    if(head->x == 0 || head->x == MAP_X - 1){
        reset_terminal_mode();  // 恢复终端为原始模式
        exit(0);
    }
    if(head->y == 0 || head->y == MAP_Y - 1){
        reset_terminal_mode();  // 恢复终端为原始模式
        exit(0);
    }
    if(food->x == head->x && food->y == head->y){
        food->exist = 0;
    }
    for(int i = 0; i < head->length; i++){
        if(head->x == body[i].x && head->y == body[i].y){
            reset_terminal_mode();  // 恢复终端为原始模式
            exit(0);  
        }
    }
}

void game_wall_loading(char map[MAP_Y][MAP_X]){
    for(int i = 0; i < MAP_Y; i++)
        for(int j = 0; j < MAP_X; j++)
            if(i < 1 || i > MAP_Y - 2 || j < 1 || j > MAP_X - 2){
                map[i][j] = 'W';
            }else{
                map[i][j] = ' ';
            }
    printf("----- 地图墙壁填充 OK -----\n");
}

void game_balance_delay(SnakeHead head, int time_us) {
#ifdef _WIN32
    Sleep(time_us / 100);  // 转换为毫秒
#else
    if (head.direction == UP || head.direction == DOWN) usleep((time_us * 16));
    if (head.direction == LEFT || head.direction == RIGHT) usleep((time_us * 9));
#endif
}

void snake_body_add(SnakeBody body[(MAP_X - 2) * (MAP_Y - 2)],SnakeHead *head){
    // 确保当前蛇的长度不会超过数组限制
    if (head->length >= (MAP_X - 2) * (MAP_Y - 2)) {
        printf("蛇已经填满整个地图，无法再增长！\n");
        reset_terminal_mode();
        exit(0);  // 如果蛇太长，直接结束游戏
    }
    int tail = head->length;
    head->length++;
    switch (body[tail - 1].direction)
    {
        case UP   : body[tail].direction = UP; body[tail].x = body[tail - 1].x; body[tail].y = body[tail - 1].y + 1; break;
        case DOWN : body[tail].direction = DOWN; body[tail].x = body[tail - 1].x; body[tail].y = body[tail - 1].y - 1; break;
        case LEFT : body[tail].direction = LEFT; body[tail].x = body[tail - 1].x + 1; body[tail].y = body[tail - 1].y; break;
        case RIGHT: body[tail].direction = RIGHT; body[tail].x = body[tail - 1].x - 1; body[tail].y = body[tail - 1].y; break;
        default:break;
    }
}

void clear_screen() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0}; // 光标位置
    SetConsoleCursorPosition(hConsole, coord); // 将光标移到左上角
    // 清空终端的每一行
    for (int i = 0; i < MAP_Y; i++) {
        for (int j = 0; j < MAP_X; j++) {
            printf(" ");
        }
        printf("\n");
    }
    SetConsoleCursorPosition(hConsole, coord); // 将光标移回左上角
#else
    system("clear");
#endif
}

void game_new_food_add(SnakeFood *food, char map[MAP_Y][MAP_X]){
    do
    {
        food->x = 1 + rand() % (MAP_X - 2);
        food->y = 1 + rand() % (MAP_Y - 2);
    } while (map[food->y][food->x] != ' ');
    food->exist = 1;
}

void game_set_delay(){
    #ifdef _WIN32
        Sleep(1);
    #else
        usleep(100000);
    #endif
}

int getch() {
#ifdef _WIN32
    return _getch();
#else
    return getchar();
#endif
}

int main()
{
    // 初始化随机数种子，只需调用一次
    srand(time(0));

    //难度选择
    int level = 1;

    // 主函数变量声明
    SnakeHead head;
    SnakeBody body[(MAP_X - 2) * (MAP_Y - 2)];
    SnakeFood food;
    char map[MAP_Y][MAP_X];
    char controls;
    int time = 1;

    // 初始化函数引用
    snake_setup(&head, body, &food);
    game_set_delay();
    game_wall_loading(map);
    game_set_delay();

    clear_screen();
    game_render(head, map, body, food);
    time = 20000 * level;
    set_conio_terminal_mode();
    // game_show(map);
    while (1){
        clear_screen();
        if(food.exist == 0)
            game_new_food_add(&food, map);
        if(control_count!=0)
            snake_body_change_direction(head,body);
        if(kbhit()){
            controls = getch();
            if (controls == 'q') {
                break;
            } else if (controls == 'w' || controls == 'a' || controls == 's' || controls == 'd') {
                snake_change_direction(&head, controls);
            }
        }
        snake_move(&head, body);
        snake_collision(&head, &food, body);
        if(food.exist == 0){
            snake_body_add(body,&head);
        }
        game_render(head, map, body, food);
        game_show(map);
        game_balance_delay(head, time);
    }
    reset_terminal_mode();  // 恢复终端为原始模式
    return 0;
}