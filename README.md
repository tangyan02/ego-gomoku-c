## 更新记录

### 5月7日 v8.0
- 支持自我对战调整参数
- 修复VCT BUG
- 使评分允许更细化
- 在piskvork协议下对战中，支持用上次的计算结果，帮助下一回合计算

## ===2021年分隔=== 


### 8月26日 v7.0
- 重构各个模块，重写评估函数

## ===2018年分隔===

### 10月11日 v5.1.0
- 去除了复合棋形的判断
- 用位运算优化了分析的效率
- 算杀模块中去除区部搜索，来保持稳定性

### 10月10日 v5.0.0
- C语言重制版
- 修复复合棋形判断的BUG
- 优化了AB剪枝的效率

### 9月26日 v4.6.0
- 支持GomokuCup的Piskvork协议

### 9月25日 v4.5.0
- 加入了迭代加深，来限制单步落子时间

### 9月22日 v4.0.0
- 加入对单层最多节点数的配置

### 9月21日 v3.9.0
- 对包名进行了优化
- 简化了搜索代码
- 加入了支持黑棋随机开局

### 9月17日 v3.8.0
- 修复了连击优化后可能出现的BUG

### 9月17日 v3.7.0
- 对连击性能进行了优化

### 9月14日 v3.6.0
- 对连击策略进行了优化
- 引擎由Komuku更名为EGO

### 8月11日 v3.4.0
- 为连击添加了缓存
- 修复了边界评分的问题
- 加入迭代连击搜索，必胜的情形会走最捷径的方式，不再会调戏玩家
- 在必败的情形，不再消极的下棋

### 8月5日 v3.0.0
- 重构了部分代码
- 优化了评估函数，大局观加强
- 优化了连击算法，能够打出更高端的操作

### 7月1日 v2.0.1
- 修复了计算时有可能数组越界的问题
- 修复了能赢的情况有可能下出迷之下法的情况

### 6月25日 v2.0.0
- 加入了连击体系，战斗力大幅提升

### 6月18日 v1.3.0
- 算法优化，减少高难度下的运算时间

### 6月4日 v1.2.0
- 算法优化升级，战斗力LEVEL UP
