/*
时间参考：
时钟频率为400Mhz的x86指令集处理器
一个时钟周期为1s/4E9 ~ 2.5E-10[2.5ns]
寄存器到寄存器->1 个时钟周期[2.5ns]
立即数到寄存器->1 个时钟周期[2.5ns]

若代码涉及频繁的内存访问(使用指针访问内存),
结合cpu缓存考虑
假设缓存命中率为 90%,
设主存访问需要 50 个时钟周期,
缓存访问只需3~5 取4个周期
得出内存与寄存器交互的平均周期为(0.9*50 + 0.1*4)~5.4周期[13.5ns]

//数组中的赋值
        array[0] = array[1] 的x86架构汇编指令:
mov eax, DWORD PTR [array + 4]  ; 将 array[1] 的值加载到寄存器 eax 中
mov DWORD PTR [array], eax      ; 将寄存器 eax 的值存储到 array[0] 中

可拆分为4步
计算 array[1] 的内存地址;1时钟周期[2.5ns]
将 array[1] 的值从内存加载到寄存器;内存交互[13.5ns]
计算 array[0] 的内存地址;1时钟周期[3.5ns]
将寄存器中的值存储到 array[0] 的内存地址;内存交互[13.5ns]
总计估计数组中的赋值平均时间[32ns]

       int temp = array[0] 的x86架构汇编指令:
mov eax, DWORD PTR [array]      ; 将 array[0] 的值(内存)加载到寄存器 eax 中
mov DWORD PTR [temp], eax       ; 将寄存器 eax 的值存储到 temp(内存) 中
同上约[32ns]
变量的生命周期较短，
且它只在一个局部作用域内使用，
编译器通常会将其直接放在寄存器中，

引用变量或指针变量访问时需要使用到内存
int &ref = a;
因为寄存器没有地址

寄存器优先：编译器通常会将临时变量分配到寄存器中，因为寄存器访问最快。
内存使用：当寄存器不足，或者变量需要地址引用时（如使用指针、引用时），
临时变量会存储在内存（栈或堆）中。
缓存：缓存是硬件层面的优化，编译器不直接控制，
但在内存中访问的变量可能被 CPU 自动缓存，提高访问速度。
优化级别：编译器的优化级别会极大影响临时变量的存储策略，
在高优化下，许多临时变量可能被优化掉。
不指定构建类型，CMake 默认使用 Debug 模式，相当于没有优化（-O0）

无优化（-O0）：编译器会尽量为每个变量分配内存，尽可能保留所有变量的声明，
即使它们可以存储在寄存器中。
高优化级别（-O2, -O3）：编译器会对临时变量进行寄存器分配，
甚至对不必要的临时变量进行消除（elimination）。
例如，如果临时变量只是某个值的中间步骤，编译器可能会将其优化掉，而直接使用寄存器计算结果。
而不会占用内存

 赋值语句:int i = 5;
mov eax, 5   ; 将常量 5 赋值给寄存器 eax（对应 i）[2.5ns]
 比较语句:if(i <= j)...
cmp eax, ebx  ; 比较寄存器 eax（i）和寄存器 ebx（j）[2.5ns]

默认模式下（-O0），CMake 和 Clang
编译时的策略是内存优先，尽量将所有变量保留在内存中以便调试。
即相当于
mov eax, DWORD PTR [array + 4]  ; 将 array[1] 的值加载到寄存器 eax 中
一次内存计算+访问+赋值=2+5.4时钟周期~[18.5ns]
而比较则为两次内存计算及访问+比较指令=5.4*2+1周期～[29.5ns]
*/

#ifndef ARRAYWIDGET_H
#define ARRAYWIDGET_H

#include <QEventLoop>
#include <QPainter>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <utility>

class ArrayWgtController;

class ArrayWidget : public QWidget {
  Q_OBJECT
  // 数组本体
  int *array;

  // 数组大小
  int arraySize;

  // 动画延迟(ms)
  int animDelay{3};

  // 理论排序耗时
  double sortTime = 0;

  // 内存使用
  double memoryUse{0};

  // 最大内存占用
  double maxMemoryUse{0};

  // 栈深度
  int stackDepth{0};

  // 最大栈深度
  int maxStackDepth{0};

  // 程序耗时
  int processTime{0};

  // 是否为随机数组
  bool isRandom;

  // 检查标记(对比)
  std::pair<int, int> checkPair{0, 0};

  // 比较次数
  int compareTimes = 0;

  // 交换次数
  int swapTimes = 0;

  // 柱体间距
  int spacing = 2;

  // 当前算法名
  QString currentAg;

  // 日志函数指针(使用logInfo(string);来在日志框打印日志)
  void (*logInfo)(QString &&);

  // 生成数据
  void generateData();

  // 交换数据
  void swapData(int i, int j);

  // 延时方法，不阻塞主线程
  void delay();

  // 冒泡排序
  void bubbleSort();

  // 插入排序
  void insertionSort();

  // 选择排序
  void selectSort();

  // 快速排序
  void quickSort(int from, int to);

  // 进行一次标记比较
  // (array[i]大于array[j] 返回1，相等返回0，小于返回-1)
  // (用于标红和计算理论排序时间,不要作弊奥)
  int compare(int i, int j);

  // 不可避免的比较，如循环的内部判断，也需要算在算法时间内
  void inevitableComparison();

  friend ArrayWgtController;

public:
  ArrayWidget(void (*logger)(QString &&), QWidget *parent = nullptr,
              int size = 32, bool random = false);
  ~ArrayWidget();
  // 重新洗牌
  void shuffle();

  // 随机数组
  void setRandom();

  // 常规间距1数组
  void setRegular();

  // 设置数组长度
  void resizeArray(int newSize);

  // 设置动画延时
  void setAnimeDelay(int milliseconds);

  // 重置统计信息
  void resetStatistics();

protected:
  // 绘制数组
  void paintEvent(QPaintEvent *event) override;
};

#endif // ARRAYWIDGET_H
