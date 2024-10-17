#include "snake.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 

/**
 * @brief 对 Window 和 Linux 库的 选择适配
*/
#ifdef _WIN32
    #include <conio.h>      // 包含 _kbhit() 和 _getch()
    #include <windows.h>    // 包含 Sleep() 函数
#else
    #include <termios.h>    // linux 终端设置
    #include <unistd.h>     // linux 内核系统调用
    #include <fcntl.h>      // linux 文件描述符 操作接口
    // linux 终端原始状态存储变量
    typedef struct termios terminal;      // 终端状态结构体 改名
    static terminal terminal_orig_status;  // 保存原始终端设置
#endif

static int direction_stack = 0;
static int map_horizontal = 0;
static int map_vertical = 0;

static snakeBody *tail = NULL;

/**
 * @author      Exrpid
 * @brief       贪吃蛇的头
 * @memberof    int x 横坐标 int y 纵坐标 int length 长度 int direction 方向
*/
struct SnakeHead{
    int x;
    int y;
    int length;
    char direction;
};

/**
 * @author      Exrpid
 * @brief       贪吃蛇的身体
 * @memberof    int x 横坐标 int y 纵坐标 int direction 方向
*/
struct SnakeBody{
    int x;
    int y;
    char direction;
    // 采用单链表结构
    struct SnakeBody *next;
};

/**
 * @author      Exrpid
 * @brief       贪吃蛇的食物
 * @memberof    int x 横坐标 int y 纵坐标 bool flag 存在标志
*/
struct SnakeFood{
    int x;
    int y;
    bool flag;
};

/**
 * @author      Exrpid
 * @brief       初始化地图大小
 * @param       lenght_x 长度
 * @param       width_y  宽度
*/
void MapSizeInit(int lenght_x, int width_y){
    map_horizontal = lenght_x;
    map_vertical = width_y;
}

/**
 * @author      Exrpid
 * @brief       动态生成地图池
*/
char **MapSpaceCreate(){
    char **map_box = (char**)malloc(map_horizontal * sizeof(char*));
    if(!map_box){
        printf("地图空间分配失败\n");
        return NULL;
    }
    for(int i = 0; i < map_horizontal; i++)
        // 多出来的两格 装 '\n' (map_vertical) '\0' (map_vertical + 1)
        map_box[i] = (char*)malloc((map_vertical + 2) * sizeof(char));
    if(!map_box){
        printf("地图空间分配失败\n");
        return NULL;
    }
    return map_box;
}

/**
 * @author      Exrpid
 * @brief       动态生成地图缓冲区
*/
// FrontBuffer BackBUffer
char *MapBufferSpaceCreate(){
    char *buff = (char*)malloc((map_horizontal*(map_vertical+2)+1)*sizeof(char));
    return buff;
}

/**
 * @author      Exrpid
 * @brief       初始化地图 生成空地和墙壁
*/
void MapInit(char **map){
    // 填充空地
    for(int i = 1; i < map_vertical - 1; i++){
        for(int j = 1; j < map_horizontal - 1; j++){
            map[i][j] = ' ';
        }
        map[i][map_horizontal] = '\n'; // 添加换行符
        map[i][map_horizontal + 1] = '\0';    // 添加字符串结束符
    }
    // 填充墙壁
    for(int i = 0; i < map_vertical; i++){
        for(int j = 0; j < map_horizontal; j += (i == 0 || i == map_vertical - 1) ? 1 : map_horizontal - 1){
            map[i][j] = 'W';
        }
        map[i][map_horizontal] = '\n'; // 添加换行符
        map[i][map_horizontal + 1] = '\0';    // 添加字符串结束符
    }
}

/**
 * @author      Exrpid
 * @brief       初始化 蛇对象 食物对象 画面第一帧
 * @param       head 蛇头
 * @param       body 蛇身
 * @param       food 食物
 * @param       map  地图
 * @param       front_buffer 初始帧
 * @param       back_buffer  初始帧
*/
void SnakeInit(snakeHead *head, snakeBody *body, snakeFood *food, char **map, char *map_buffer){
    // place 为四位整数 前两位为 x 后两位为 y
    head->x = map_horizontal / 2;
    head->y = map_vertical  / 2;
    head->direction = RIGHT;
    head->length = 3;
    map[head->y][head->x] = 'H';
    printf("----- 蛇头初始化 OK -----\n");
    body->x = head->x - 1;
    body->y = head->y;
    body->direction = head->direction;
    body->next = NULL;
    tail = body;
    map[body->y][body->x] = 'B';
    for(int i = 0; i < 2; i++){
        tail->next = SnakeAddNewNode(tail);
        tail = tail->next;
        map[tail->y][tail->x] = 'B';
    }
    printf("----- 蛇身初始化 OK -----\n");
    do{
    food->x = rand() % (map_horizontal - 1) + 1;
    food->y = rand() % (map_vertical - 1) + 1;
    }while(map[food->y][food->x] != ' ');
    food->flag = true;
    map[food->y][food->x] = 'F';
    printf("----- 食物初始化 OK -----\n");
    for(int i = 0; i < map_vertical; i++){
        strcat(map_buffer,map[i]);
    }
    printf("----- 初始帧绘制 OK -----\n");
}

/**
 * @author      Exrpid
 * @brief       为蛇添加新的节点
 * @param       node 尾巴节点
 * @return      新生成的尾部节点
*/
snakeBody *SnakeAddNewNode(snakeBody *node){
    snakeBody *body = (snakeBody*)malloc(sizeof(snakeBody));
    switch(node->direction){
        case UP: body->x = node->x; body->y = node->y + 1; break;
        case DOWN: body->x = node->x; body->y = node->y - 1; break;
        case LEFT: body->x = node->x + 1; body->y = node->y; break;
        case RIGHT: body->x = node->x - 1; body->y = node->y; break;
        default:break;
    }
    body->direction = node->direction;
    body->next = NULL;
    return body;
}

/**
 * @author      Exrpid
 * @brief       终端模式和设置初始化
*/
void TerminalInit(){
#ifdef _WIN32
    system("cls");
    printf("Windows Terminal Init OK\n");
#else
    // 新终端状态存储变量
    terminal terminal_new_status;
    // 存储原始终端状态 以便于 “ 复位 ”
    tcgetattr(0, &terminal_orig_status);
    tcgetattr(0, &terminal_new_status);
    terminal_new_status.c_lflag &= ~ICANON; // 取消终端 “ 规范 ” 模式
    terminal_new_status.c_lflag &= ~ECHO;   // 关闭终端 “ 回显 ” 模式
    tcsetattr(0, TCSANOW, &terminal_new_status); //使能 终端新状态
    printf("Linux Terminal Init OK\n");
    system("clear");
#endif
}

/**
 * @author      Exrpid
 * @brief       终端状态复位
*/
void TerminalBack(){
#ifdef _WIN32
    system("cls");
    printf("Windows Terminal Status Back OK\n");
#else
    tcsetattr(0, TCSANOW, &terminal_orig_status);
    // system("clear");
    printf("Linux Terminal Status Back OK\n");
#endif
}

/**
 * @author      Exrpid
 * @brief       蛇 位移 与 碰撞检测
 * @param       head    蛇头
 * @param       body    蛇身
 * @param       food    食物
 * @param       map     地图
 * @return      true 非死亡碰撞 ; false 死亡碰撞
*/
bool SnakeMoveAndCollisionCheck(snakeHead *head, snakeBody *body, snakeFood *food, char** map){
    // Head move
    map[head->y][head->x] = ' ';
    switch (head->direction)
    {
        case UP   : head->y -= 1;  break;
        case DOWN : head->y += 1; break;
        case LEFT : head->x -= 1; break;
        case RIGHT: head->x += 1; break;
        default:break; 
    }
    map[head->y][head->x] = 'H';
    // Body move
    snakeBody *ibody = body;
    while (ibody != NULL) {
        map[ibody->y][ibody->x] = ' ';
        switch (ibody->direction) {
            case UP   : ibody->y -= 1;  break;
            case DOWN : ibody->y += 1;  break;
            case LEFT : ibody->x -= 1; break;
            case RIGHT: ibody->x += 1; break;
            default: break;
        }
        map[ibody->y][ibody->x] = 'B';
        ibody = ibody->next;
    }
    // Wall Collision Check
    if(head->x == 0 || head->x == map_horizontal - 1 || head->y == 0 || head->y == map_vertical - 1)
        return false;
    // Food Collision Check
    if(head->x == food->x && head->y == food->y){
        food->flag = false;
        return true;
    }
    // Body Collision Check
    ibody = body;
    for (int i = 0; i < head->length; i++)
    {
        if(head->x == ibody->x && head->y == ibody->y)
            return false;
        if(ibody->next != NULL)
            ibody = ibody->next;
    }
    return true;
}

/**
 * @author      Exrpid
 * @brief       蛇食物刷新
*/
void SnakeFoodAndBodyCreate(snakeHead *head, snakeFood *food, char **map){
    // Food flag check
    if(food->flag)
        return;
    // NewBody
    tail->next = SnakeAddNewNode(tail);
    tail = tail->next;
    map[tail->y][tail->x] = 'B';
    head->length++;
    // NewFood
    do{
    food->x = rand() % (map_horizontal - 1) + 1;
    food->y = rand() % (map_vertical - 1) + 1;
    }while(map[food->y][food->x] != ' ');
    food->flag = true;
    map[food->y][food->x] = 'F';
}

/**
 * @author      Exrpid
 * @brief       输入检测
 * @return      输入状态
*/
int InPutCheck() {
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
// #ifdef _WIN32
//     return _kbhit(); // 使用 conio.h 提供的 _kbhit()
// #else
//     int oldf = fcntl(0, F_GETFL, 0);
//     fcntl(0, F_SETFL, oldf | O_NONBLOCK);
    
//     int ch = getchar();
//     fcntl(0, F_SETFL, oldf);
    
//     if(ch != EOF) {
//         ungetc(ch, stdin);
//         return 1; // 有输入
//     }
//     return 0; // 没有输入
// #endif
}

/**
 * @author      Exrpid
 * @brief       按键输入读取
 * @return      输入字符
*/
int KeyInPut() {
#ifdef _WIN32
    return _getch();
#else
    return getchar();
#endif
}

/**
 * @author      Exrpid
 * @brief       头部方向改变
*/
void SnakeHeadDirectionChange(snakeHead *head, char in){
    if((head->direction == UP) && in == DOWN)return;
    if((head->direction == DOWN) && in == UP)return;
    if((head->direction == LEFT) && in == RIGHT)return;
    if((head->direction == RIGHT) && in == LEFT)return;
    direction_stack = head->length;
    head->direction = in;
    return;
}

/**
 * @author      Exrpid
 * @brief       身体方向改变
*/
void SnakeBodyDirectionChange(snakeHead *head, snakeBody *body){
    if(direction_stack == 0)
        return;
    snakeBody *ibody = body;
    char next_direction = ' ';
    char before_direction = head->direction;
    for(int i = 0; i < head->length; i++){
        next_direction = ibody->direction;
        ibody->direction = before_direction;
        before_direction = next_direction;
        if(ibody->next != NULL)
            ibody = ibody->next;
    }
    direction_stack--;
}

/**
 * @author      Exrpid
 * @brief       画面绘制
*/
void MapScreenDraw(char **map, char *map_buff){
#ifdef _WIN32
    system("cls");   // Windows   
#else
    system("clear"); // Linux
#endif
    memset(map_buff, 0, (map_horizontal * (map_vertical + 2) + 1) * sizeof(char)); // 清空 buffer
    for(int i = 0; i < map_vertical; i++){
        strcat(map_buff, map[i]);
    }
    printf("%s",map_buff);
}

// 用于测试
int main(){
    srand(time(0));
    MapSizeInit(16,9);
    char **map = MapSpaceCreate();
    char *map_buff = MapBufferSpaceCreate();
    MapInit(map);
    snakeHead head;
    snakeBody body;
    snakeFood food;
    SnakeInit(&head, &body, &food, map, map_buff);
    TerminalInit();
    printf("%s",map_buff);
    while(1){
        SnakeBodyDirectionChange(&head, &body);
        if(InPutCheck()){
            char controls = KeyInPut();
            SnakeHeadDirectionChange(&head, controls);
        }
        if(SnakeMoveAndCollisionCheck(&head, &body, &food, map)){
            printf("\n计算完成\n");
        }else{
            TerminalBack();
            break;
        }
        SnakeFoodAndBodyCreate(&head, &food, map);
        MapScreenDraw(map, map_buff);
        usleep(100000);
    }
    return 0; 
}
