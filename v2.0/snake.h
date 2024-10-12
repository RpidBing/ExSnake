#ifndef SNAKE_H
#define SNAEK_H

#include <stdbool.h> 

/* 宏定义声明 */
#define UP    'w'
#define DOWN  's'
#define LEFT  'a'
#define RIGHT 'd'

/* 结构体声明 */
typedef struct SnakeHead snakeHead;
typedef struct SnakeBody snakeBody;
typedef struct SnakeFood snakeFood;

/* 全局变量 */
// extern snakeBody *tail_flag;

/* 函数声明 */
// 交换两数的值
void ThreeCups(int *a, int *b);
// 地图大小初始化
void MapSizeInit(int lenght_x, int width_y);
// 地图控制分配初始化
char **MapSpaceCreate();
// 地图缓冲区空间分配
char *MapBufferSpaceCreate();
// 地图初始化 填充 空地和墙壁
void MapInit(char **map);
// “ 蛇 ” 初始化 蛇头 蛇身 初始画面帧
void SnakeInit(snakeHead *head, snakeBody *body, snakeFood *food, char **map, char *front_buffer, char *back_buffer);
// 新增蛇节点
snakeBody *SnakeAddNewNode(snakeBody *node);
// 终端状态初始化
void TerminalInit();
// 终端状态复位
void TerminalBack();
// 蛇 位移 与 碰撞检测 
bool SnakeCollisionCheckAndMove(snakeHead *head, snakeBody *body, snakeFood *food, char **map);
// 地图帧渲染
void MapFramesWirte(char **map, char *front_buffer, char *back_buffer);
// 地图画面输出
void MapDrawOutPut(char **map, char *front_buffer, char *back_buffer);

#endif
