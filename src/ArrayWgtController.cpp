#include "../headers/ArrayWgtController.h"

QTextEdit *ArrayWgtController::logger;

ArrayWgtController::ArrayWgtController() : QWidget(nullptr) {
  // 初始化输入检查器
  validator = new CustomValidator;
  // 初始化根布局
  rootLayout = new QVBoxLayout;
  rootLayout->setSpacing(2);
  rootLayout->setContentsMargins(2, 2, 2, 2);

  // 初始化内容容器及布局
  contents = new QWidget;
  contentLayout = new QHBoxLayout;
  contentLayout->setSpacing(2);
  contentLayout->setContentsMargins(2, 2, 2, 2);

  // 初始化日志
  logger = new QTextEdit("logger is initialized.");
  logger->setReadOnly(true);
  logger->setMinimumWidth(120);
  logger->setMaximumWidth(350);

  // 初始化数组组件
  arrayWidget = new ArrayWidget(&ArrayWgtController::info, this);
  arrayWidget->setMinimumSize(500, 500);
  arrayWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  contentLayout->addWidget(arrayWidget);
  contentLayout->addWidget(logger);

  // 设置内容布局
  contents->setLayout(contentLayout);
  contents->adjustSize();
  // 初始化顶部组件
  topSelections = new QWidget;
  topLayout = new QHBoxLayout;
  topLayout->setSpacing(8);
  topLayout->setContentsMargins(2, 2, 2, 2);

  // 初始化大小设置
  sizeEdit = new QComboBox;
  sizeInfo = new QLabel("array length:");
  sizeEdit->addItems({"16", "32", "64", "128", "256", "512", "1024", "2048",
                      "4096", "8192", "16384", "32768", "65536", "131072",
                      "262144"});
  sizeEdit->setCurrentIndex(1);
  sizeEdit->setEditable(true);
  sizeEdit->setValidator(validator);
  topLayout->addWidget(sizeInfo);
  topLayout->addWidget(sizeEdit);

  // 初始化数组类型设置
  arrayTypeInfo = new QLabel("type:");
  arrayType = new QComboBox;
  arrayType->addItems({"unique", "random"});
  arrayType->setCurrentIndex(0);
  topLayout->addWidget(arrayTypeInfo);
  topLayout->addWidget(arrayType);

  // 初始化延时设置
  delayInfo = new QLabel("delay:");
  delayEdit = new QComboBox;
  delayEdit->setEditable(true);
  delayEdit->setValidator(validator);

  delayEdit->addItems(
      {"1", "5", "10", "20", "50", "66", "100", "133", "166", "200"});
  delayEdit->setCurrentIndex(1);

  delayUnit = new QLabel("ms");

  topLayout->addWidget(delayInfo);
  topLayout->addWidget(delayEdit);
  topLayout->addWidget(delayUnit);

  // 初始化排序选择设置
  sortMethodInfo = new QLabel("sort Method:");
  sortMethodEdit = new QComboBox;

  sortMethodEdit->addItems({"Bubble Sort"});
  sortMethodEdit->setCurrentIndex(0);

  topLayout->addWidget(sortMethodInfo);
  topLayout->addWidget(sortMethodEdit);

  // 初始化复选框
  enableLogInfo = new QLabel("enable log:");
  enableLog = new QCheckBox;
  enableLog->setChecked(true);

  topLayout->addWidget(enableLogInfo);
  topLayout->addWidget(enableLog);

  // 初始化功能键
  applyButton = new QPushButton("apply");
  connect(applyButton, &QPushButton::clicked, this,
          &ArrayWgtController::applySettings);
  topLayout->addWidget(applyButton);

  sortButton = new QPushButton("sort");
  connect(sortButton, &QPushButton::clicked, this,
          &ArrayWgtController::startSort);
  topLayout->addWidget(sortButton);

  shuffleButton = new QPushButton("shuffle");
  connect(shuffleButton, &QPushButton::clicked, this,
          &ArrayWgtController::shuffleArray);
  topLayout->addWidget(shuffleButton);
  // 添加弹簧
  topLayout->addStretch();
  // 设置顶部布局
  topSelections->setLayout(topLayout);

  // 将组件加入根布局
  rootLayout->addWidget(topSelections);
  rootLayout->addWidget(contents);

  // 设置根布局
  setLayout(rootLayout);

  // 移除焦点
  sizeInfo->setFocus();
  adjustSize();
}

ArrayWgtController::~ArrayWgtController() {}

// 应用设置
void ArrayWgtController::applySettings() {
  // 处理数组类型
  switch (arrayType->currentIndex()) {
  case 0: {
    if (arrayWidget->isRandom) {
      arrayWidget->setRegular();
      info("<font color='cyan'>array type set to unique</font>");
    } else {
      info("<font color='pink'>array type set no change</font>");
    }
    break;
  }
  case 1: {
    if (!arrayWidget->isRandom) {
      arrayWidget->setRandom();
      info("<font color='green'>array type set to random</font>");
    } else {
      info("<font color='pink'>array type no change</font>");
    }
    break;
  }
  }

  // 处理数组长度
  bool ok;
  int arraySize = sizeEdit->currentText().toInt(&ok);
  if (ok) {
    if (arraySize < 16) {
      info("<font color='red'>array length minimum is 16</font>");
      sizeEdit->setCurrentIndex(0);
      arraySize = 16;
    }
    if (arraySize > 262144) {
      info("<font color='red'>array length maxmum is 262144</font>");
      sizeEdit->setCurrentIndex(14);
      arraySize = 262144;
    }
  } else {
    info("<font color='red'>array length invalid</font>");
  }
  // info("widget size:" + QString::number(arrayWidget->arraySize));
  // info("setting size:" + QString::number(arraySize));
  if (arrayWidget->arraySize == arraySize) {
    info("<font color='yellow'>array length no change</font>");
  } else {
    arrayWidget->resizeArray(arraySize);
  }

  // 处理动画延迟
  int animeDelay = delayEdit->currentText().toInt(&ok);
  if (ok) {
    if (animeDelay < 1) {
      info("<font color='red'>delay minimum is 1</font>");
      delayEdit->setCurrentIndex(0);
      animeDelay = 1;
    }
    if (animeDelay > 200) {
      info("<font color='red'>delay maxmum is 262144</font>");
      delayEdit->setCurrentIndex(9);
      animeDelay = 200;
    }
  } else {
    info("<font color='red'>delay invalid</font>");
  }
  arrayWidget->setAnimeDelay(animeDelay);

  // 处理日志框
  bool layoutChange{false};
  if (enableLog->isChecked()) {
    // 打开日志
    if (!logger->isVisible()) {
      contentLayout->addWidget(logger);
      logger->setVisible(true);
      layoutChange = true;
    }
  } else {
    // 关闭日志
    if (logger->isVisible()) {
      logger->setVisible(false);
      contentLayout->removeWidget(logger);
      layoutChange = true;
    }
  }
  if (layoutChange) {
    contents->adjustSize();
    adjustSize();
  }
}
// 开始排序
void ArrayWgtController::startSort() {
  switch (sortMethodEdit->currentIndex()) {
  case 0: {
    arrayWidget->resetStatistics();
    arrayWidget->bubbleSort();
    info("</font color='pink'>current using: bubble sort</font>");
    break;
  }
  }
}

// 打乱数组
void ArrayWgtController::shuffleArray() { arrayWidget->shuffle(); }

void ArrayWgtController::info(QString &&s) {
  if (logger->isVisible())
    logger->append("<font color='purple'>[" +
                   QDateTime::currentDateTime().toString("HH:mm:ss.zzz") +
                   "]</font>:" + s);
}

#include "moc_ArrayWgtController.cpp"
