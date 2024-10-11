#include "snake.h"

#include <stdio.h>
#include <stdbool.h> 

static snakeBody *tail;
static int map_horizontal;
static int map_vertical;

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
        printf("地图空间分配失败");
        return NULL;
    }
    for(int i = 0; i < map_horizontal; i++)
        // 多出来的两格 装 '\n' (map_vertical) '\0' (map_vertical + 1)
        map_box[i] = (char*)malloc((map_vertical + 2) * sizeof(char));
    if(!map_box){
        printf("地图空间分配失败");
        return NULL;
    }
    return map_box;
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
    }
    // 填充墙壁
    for(int i = 0; i < map_vertical; i++){
        for(int j = 0; j < map_horizontal; j += (i == 0 || i == map_vertical - 1) ? 1 : map_horizontal - 1){
            map[i][j] = 'W';
        }
    }

}

/**
 * @author      Exrpid
 * @brief       初始化 蛇对象 食物对象 画面第一帧
 * @param       head 蛇头
 * @param       body 蛇身
 * @param       food 食物
 * @param       place 位置
*/
void SnakeInit(snakeHead *head, snakeBody *body, snakeFood *food,int place){
    // place 为四位整数 前两位为 x 后两位为 y
    head->x = (place / 100) / 2;
    head->y = (place % 100) / 2;
    head->direction = RIGHT;
    printf("----- 蛇头初始化 OK -----");
    body->x = head->x - 1;
    body->y = head->y;
    body->direction = head->direction;
    body->next = NULL;
    tail = body;
    for(int i = 0; i < 2; i++){
        tail->next = Snake_add_new_node(tail);
        tail = tail->next;    
    }
    printf("----- 蛇身初始化 OK -----");
    food->x = rand() % (place / 100) + 1;
    food->y = rand() % (place / 100) + 1;
    food->flag = true;
}

/**
 * @author      Exrpid
 * @brief       为蛇添加新的节点
 * @param       node 尾巴节点
 * @return      新生成的尾部节点
*/
snakeBody *Snake_add_new_node(snakeBody *node){
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


