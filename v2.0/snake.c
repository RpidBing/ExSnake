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

static bool buffer_flag;
static snakeBody *tail = NULL;
static int map_horizontal = 0;
static int map_vertical = 0;

/**
 * @author      Exrpid
 * @brief       贪吃蛇的头
 * @memberof    int x 横坐标 int y 纵坐标 int length 长度 int direction 方向
*/
struct SnakeHead{
    int x;
    int y;
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
 * @brief       交换两数的值
 * @param       a 变量 1
 * @param       b 变量 2
*/
void ThreeCups(int *a, int *b){
    int cup = *a;
    *a = *b;
    *b = cup;
}

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
    buffer_flag = true;
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
void SnakeInit(snakeHead *head, snakeBody *body, snakeFood *food, char **map, char *front_buffer, char *back_buffer){
    // place 为四位整数 前两位为 x 后两位为 y
    head->x = map_horizontal / 2;
    head->y = map_vertical  / 2;
    head->direction = RIGHT;
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
    food->x = rand() % (map_horizontal - 1) + 1;
    food->y = rand() % (map_vertical - 1) + 1;
    food->flag = true;
    map[food->y][food->x] = 'F';
    printf("----- 食物初始化 OK -----\n");
    for(int i = 0; i < map_vertical; i++){
        strcat(front_buffer,map[i]);
        strcat(back_buffer,map[i]);
    }
    buffer_flag = true;
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
 * @param       buffer  渲染缓冲区
*/
bool SnakeCollisionCheckAndMove(snakeHead *head, snakeBody *body, snakeFood *food, char **map){
    // 碰撞检测
    // 判蛇头预位移位置 是否有墙壁或食物
    int future_x, future_y;
    switch(head->direction){
        case UP: future_x = head->x; future_y = head->y - 1; break; 
        case DOWN: future_x = head->x; future_y = head->y + 1; break; 
        case LEFT: future_x = head->x - 1; future_y = head->y; break; 
        case RIGHT: future_x = head->x + 1; future_y = head->y; break; 
        default:break;
    }
    // 蛇头 位移 规则
    if(map[future_y][future_x] == 'W' || map[future_y][future_x] == 'B'){
        // 游戏失败
        return false;
    }else if(map[future_y][future_x] == 'F'){
        food->flag = false;
        map[head->y][head->x] = ' ';
        ThreeCups(&(head->x), &future_x);
        ThreeCups(&(head->y), &future_y);
        map[head->y][head->x] = 'H';
    }else{
        map[head->y][head->x] = ' ';
        ThreeCups(&(head->x), &future_x);
        ThreeCups(&(head->y), &future_y);
        map[head->y][head->x] = 'H';
    }
    // 蛇尾 前移
    int before_tail_x, before_tail_y;
    switch(tail->direction){
        case UP: before_tail_x = head->x; before_tail_y = head->y - 1; break; 
        case DOWN: before_tail_x = head->x; before_tail_y = head->y + 1; break; 
        case LEFT: before_tail_x = head->x - 1; before_tail_y = head->y; break; 
        case RIGHT: before_tail_x = head->x + 1; before_tail_y = head->y; break; 
        default:break;
    }
    snakeBody *future_tail;
    future_tail = body;        
    for(int i = 0; i < (map_horizontal - 1) * (map_vertical - 1); i++){
        if(!future_tail->next)
            future_tail = future_tail->next;
    }
    future_tail->next = NULL;
    map[tail->y][tail->x] = ' ';
    tail->next = body;
    body = tail;
    tail = future_tail;
    body->direction = head->direction;
    body->x = future_x;
    body->y = future_y;
    map[body->y][body->x] = 'B';
    if(!food->flag){
        // 增尾 操作 <在吃果实之后>
        tail->next = SnakeAddNewNode(tail);
        tail = tail->next;
        map[tail->y][tail->x] = 'B';
        // 增加果实 <在果实被吃后>
        food->x = rand() % (map_horizontal - 1) + 1;
        food->y = rand() % (map_vertical - 1) + 1;
        food->flag = true;
        map[food->y][food->x] = 'F';
    }
    return true;
}

/**
 * @author      Exrpid
 * @brief       地图帧渲染
 * @param       map             地图
 * @param       front_buffer    初缓冲
 * @param       back_buffer     次缓冲
*/
void MapFrameWirte(char **map, char *front_buffer, char *back_buffer){
    if(buffer_flag){
        for (int i = 0; i < map_vertical; i++)
            strcat(back_buffer,map[i]);
        return;
    }
    for (int i = 0; i < map_vertical; i++)
        strcat(front_buffer,map[i]);
    buffer_flag = !buffer_flag;
    return;
}

/**
 * @author      Exrpid
 * @brief       地图帧渲染
 * @param       front_buffer    初缓冲
 * @param       back_buffer     次缓冲
*/
void MapDrawOutPut(char *front_buffer, char *back_buffer){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    if(buffer_flag){
        printf("%s",front_buffer);
        return;
    }
    printf("%s",back_buffer);
    return;
}



// // 用于测试
// int main(){
//     srand(time(0));
//     MapSizeInit(16,9);
//     char **map = MapSpaceCreate();
//     char *map_front_buff = MapBufferSpaceCreate();
//     char *map_back_buff = MapBufferSpaceCreate();
//     MapInit(map);
//     snakeHead head;
//     snakeBody body;
//     snakeFood food;
//     SnakeInit(&head, &body, &food, map, map_front_buff, map_back_buff);
//     TerminalInit();
//     printf("%s",map_front_buff);
//     usleep(100000);
//     SnakeCollisionCheckAndMove(&head, &body, &food, map);
//     printf("%s",map_front_buff);
//     TerminalBack();
//     getchar();
//     return 0; 
// }
