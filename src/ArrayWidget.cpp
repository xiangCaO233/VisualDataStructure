#include "../headers/ArrayWidget.h"
#include <_stdlib.h>
#include <unistd.h>

// 构造数组组件
ArrayWidget::ArrayWidget(void (*logger)(QString &&), QWidget *parent, int size,
                         bool random)
    : QWidget(parent), arraySize(size), isRandom(random) {
  logInfo = logger;
  array = new int[size];
  generateData();
  if (!isRandom) {
    shuffle();
  }
  resetStatistics();
  // 计时器初始化
  timer = new QTimer;
  connect(timer, &QTimer::timeout, [this]() { processTime++; });
}

// 析构数组组件
ArrayWidget::~ArrayWidget() { delete array; }
// 生成数据
void ArrayWidget::generateData() {
  if (!isRandom) {
    for (int i = 0; i < arraySize; i++) {
      array[i] = i + 1;
    }
  } else {
    for (int i = 0; i < arraySize; i++) {
      array[i] = rand() % (2 * arraySize + 1) + 1;
    }
  }
  parentWidget()->repaint();
}

// 渲染组件
void ArrayWidget::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  QRectF wRect = rect().toRectF();
  // 无边框+黑色填充
  p.setPen(Qt::NoPen);
  p.setBrush(Qt::black);
  // 绘制背景
  p.drawRect(wRect);

  // 内容区域向内缩8px
  QRectF contentRect =
      wRect.adjusted(2 * spacing, 2 * spacing, -2 * spacing, -2 * spacing);
  qreal nWidth;
  if (arraySize <= 256) {
    // 数的宽度计算(间距1px)
    nWidth = contentRect.width() / (double)arraySize - spacing;
  } else {
    nWidth = contentRect.width() / (double)arraySize;
  }

  // 数的高度 nHeight =contentRect.height() * array[i]/
  // (isRandom?arraySize:arraySize*2)

  for (int i = 0; i < arraySize; i++) {
    qreal nHeight = contentRect.height() * array[i] /
                    (!isRandom ? arraySize : arraySize * 2);
    qreal drawX;
    if (arraySize <= 256) {
      drawX = (nWidth + spacing) * i + (double)spacing / 2.0 + 2 * spacing;
    } else {
      drawX = nWidth * i;
    }
    // 整体区域中 总高-数高-底部间距内缩+顶部间距内缩
    qreal drawY = wRect.height() - nHeight; //-8+8;

    // 设置填充色
    if (checkPair.first == i) {
      // 如果是被标记突出的下标,设置为红色
      p.setBrush(Qt::red);
    } else if (checkPair.second == i) {
      p.setBrush(Qt::green);
    } else {
      // 非标记,设置为白色
      p.setBrush(Qt::white);
    }

    QRectF nRect = QRectF(drawX, drawY, nWidth, nHeight);
    // 绘制此数
    p.drawRect(nRect);
  }

  // 绘制信息
  p.setPen(Qt::white);
  p.setBrush(Qt::NoBrush);
  p.setFont(QFont(font().family()));
  // 当前算法
  p.drawText(6, 12, "current algorithm:" + currentAg);
  // 次数信息
  p.drawText(6, 34, "comparisons:" + QString::number(compareTimes));
  p.drawText(6, 48, "swaps:" + QString::number(swapTimes));
  // 时间信息
  p.drawText(6, 70,
             "sort time:" + QString::number(sortTime / 1000 / 1000) + "ms");
  p.drawText(6, 84, "process time:" + QString::number(processTime) + "s");
  // 内存信息
  p.drawText(6, 106,
             "extra memory use:" + QString::number(memoryUse) + " byte\t" +
                 "max extra memory use:" + QString::number(maxMemoryUse) +
                 " byte");
  // 栈信息
  p.drawText(6, 128,
             "stack depth:" + QString::number(stackDepth) +
                 "\tmax stack depth:" + QString::number(maxStackDepth));
}

// 交换数据平均96ns
void ArrayWidget::swapData(int i, int j) {

  stackDeeper();
  memoryChange(4);
  int tempi = array[i];
  array[i] = array[j];
  array[j] = tempi;

  logInfo("<font color='gold'>" +
          QString("--swaped index--[%1,%2]--").arg(i).arg(j) + "</font>");

  // 重绘
  // 参考时间:三次包含内存计算及访问的赋值操作
  // 总计估计数组中的赋值平均时间[32ns]
  // 3*32ns~96ns
  // 开始记录
  swapTimes++;
  sortTime += 96;
  // 延迟重绘
  delay();
  memoryChange(-4);
  stackShallower();
  // 记录结束
}

// 洗牌
void ArrayWidget::shuffle() {
  stackDeeper();
  currentAg = "shuffling...";
  // 以1ms动画延时打乱数组
  int temp = animDelay;
  animDelay = 1;
  // Fisher-Yates 洗牌算法
  // 从数组的最后一个元素开始
  memoryChange(4);
  for (int i = arraySize - 1; i > 0; --i) {
    // 生成 [0, i] 范围内的随机索引
    memoryChange(4);
    int j = rand() % (i + 1);

    // 交换 arr[i] 和 arr[j]
    swapData(i, j);
    delay();
    memoryChange(-4);
  }
  memoryChange(-4);
  animDelay = temp;
  currentAg = "";
  parentWidget()->repaint();
  stackShallower();
}

void ArrayWidget::setRandom() {
  isRandom = true;
  delete array;
  array = new int[arraySize];

  generateData();
}

void ArrayWidget::setRegular() {
  isRandom = false;
  delete array;
  array = new int[arraySize];

  generateData();
}

void ArrayWidget::resizeArray(int newSize) {
  arraySize = newSize;
  delete array;
  array = new int[arraySize];

  generateData();
}

void ArrayWidget::setAnimeDelay(int milliseconds) { animDelay = milliseconds; }

// 冒泡排序
void ArrayWidget::bubbleSort() {
  // 设置当前算法标题
  currentAg = "<Bubble Sort>";
  // 栈深度++
  stackDeeper();
  // 申请4字节
  memoryChange(+4);
  for (int i = 0; i < arraySize; i++) {
    // 不可避免的比较(循环内比较)
    inevitableComparison();
    // 申请4字节内存
    memoryChange(+4);
    for (int j = 0; j < arraySize - i - 1; j++) {
      // 不可避免的比较(循环内比较)
      inevitableComparison();
      if (compare(j, j + 1) == 1) {
        swapData(j, j + 1);
      }
    }
    // 释放了4字节
    memoryChange(-4);
  }
  // 释放了4字节
  memoryChange(-4);
  // 栈深度--
  stackShallower();
}
// 插入排序
void ArrayWidget::insertionSort() {
  currentAg = "Insertion Sort";
  logInfo("<font color='red'>fuck you</font>");
}

// 选择排序
void ArrayWidget::selectSort() {
  currentAg = "Select Sort";
  logInfo("<font color='red'>fuck you</font>");
}

// 归并排序
void ArrayWidget::mergeSort(int from, int to) {
  currentAg = "Merge Sort";
  logInfo("<font color='red'>fuck you</font>");
}

// 桶排序
void ArrayWidget::bucketSort() {
  currentAg = "Bucket Sort";
  logInfo("<font color='red'>fuck you</font>");
}

// 计数排序
void ArrayWidget::countingSort() {
  currentAg = "Counting Sort";
  logInfo("<font color='red'>fuck you</font>");
}

// 基数排序
void ArrayWidget::radixSort() {
  currentAg = "Radix Sort";
  logInfo("<font color='red'>fuck you</font>");
}

// 快速排序
void ArrayWidget::quickSort(int from, int to) {
  currentAg = "Quick Sort";
  logInfo("<font color='red'>fuck you</font>");
  int pivot = array[from];
  while (from < to) {
    // while () {
    // }
  }
}

// 希尔排序
void ArrayWidget::shellSort() {
  currentAg = "Shell Sort";
  logInfo("<font color='red'>fuck you</font>");
}

// 堆排序(大顶堆)
void ArrayWidget::maxHeapSort(int from, int to) {
  currentAg = "Heap Sort(max)";
  logInfo("<font color='red'>fuck you</font>");
}

// 堆排序(小顶堆)
void ArrayWidget::minHeapSort(int from, int to) {
  currentAg = "Heap Sort(min)";
  logInfo("<font color='red'>fuck you</font>");
}

// 梳排序
void ArrayWidget::combSort() {
  currentAg = "Comb Sort";
  logInfo("<font color='red'>fuck you</font>");
}

void ArrayWidget::delay() {
  // 判断是否跳过延迟
  if (skipFlag)
    return;

  QEventLoop loop;
  QTimer::singleShot(animDelay, &loop, &QEventLoop::quit);
  loop.exec(); // 进入局部事件循环，等待定时器超时
  parentWidget()->repaint();
}

int ArrayWidget::compare(int i, int j) {
  // 标记比较下标并延迟重绘
  checkPair.first = i;
  checkPair.second = j;
  delay();
  inevitableComparison();
  return array[i] > array[j] ? 1 : (array[i] == array[j] ? 0 : -1);
}

void ArrayWidget::inevitableComparison() {
  // 开始记录
  compareTimes++;
  sortTime += 29.5;
  // 记录结束
}

void ArrayWidget::stackDeeper() {
  stackDepth++;
  if (stackDepth > maxStackDepth)
    maxStackDepth = stackDepth;
  parentWidget()->repaint();
}

void ArrayWidget::stackShallower() {
  if (stackDepth > 0) {
    stackDepth--;
    parentWidget()->repaint();
  }
}

void ArrayWidget::memoryChange(int dMemory) {
  if (dMemory < 0) {
    if (memoryUse > 0) {
      memoryUse += dMemory;
    }
  } else {
    memoryUse += dMemory;
  }
  if (maxMemoryUse < memoryUse)
    maxMemoryUse = memoryUse;
  parentWidget()->repaint();
}

void ArrayWidget::resetStatistics() {
  compareTimes = 0;
  sortTime = 0.0;
  swapTimes = 0;
  processTime = 0;
  memoryUse = 0.0;
  maxMemoryUse = 0.0;
  stackDepth = 0;
  maxStackDepth = 0;
  repaint();
}

void ArrayWidget::startTimer() { timer->start(1000); }

#include "moc_ArrayWidget.cpp"
