# Snake v1.0

> 实现:
* 游戏核心玩法以实现 包括 >> 移动 增量 刷新食物 地图渲染 画面渲染 碰撞检测
* 不同操作系统适配

> 不足:
* 对于Windows版本来说 会发生较为强烈的闪屏问题 原因有所猜测但未完全了解
* 没有菜单系统 包括 >> 设置 版本信息 游戏信息 积分 历史记录 开始游戏

> 文件解释:
```shell
.Snake/v1.0
├── main.c          // 主函数 所以实现
├── MakeRun.sh      // 编译脚本 可以自主选择Win与Linux的编译版本
├── README.md       // v1.0 的信息说明
├── SnakeGame.exe   // Windows v1.0 版本 
└── SnakeGame.out   // Linux   v1.0 版本
```