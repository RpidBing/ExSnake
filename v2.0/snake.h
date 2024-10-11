#ifndef SNAKE_H
#define SNAEK_H

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
void SnakeInit(snakeHead *head, snakeBody *body, snakeFood *food,int place);
snakeBody *Snake_add_new_node(snakeBody *node);
#endif
