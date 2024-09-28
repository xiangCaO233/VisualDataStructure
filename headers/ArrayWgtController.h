#ifndef ARRAYWGTCONTROLLER_H
#define ARRAYWGTCONTROLLER_H

#include "ArrayWidget.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QTextEdit>
#include <QValidator>
#include <QWidget>

// 输入检查器
class CustomValidator : public QValidator {
public:
  CustomValidator(QObject *parent = nullptr) : QValidator(parent) {}

  State validate(QString &input, int &pos) const override {
    // 正则表达式检查
    QRegularExpression regExp("[0-9]"); // 1到6位数字
    if (!regExp.match(input).hasMatch()) {
      return Invalid;
    }
    return Acceptable;
  }
};

class ArrayWgtController : public QWidget {
  Q_OBJECT
  // 数组组件
  ArrayWidget *arrayWidget;
  // 输入检查
  CustomValidator *validator;
  // 根布局
  QVBoxLayout *rootLayout;

  // 内容布局及组件
  QWidget *contents;
  QHBoxLayout *contentLayout;

  // 顶部栏
  QWidget *topSelections;
  QHBoxLayout *topLayout;
  // 设置数组大小
  QLabel *sizeInfo;
  QComboBox *sizeEdit;
  // 设置数组类型
  QLabel *arrayTypeInfo;
  QComboBox *arrayType;
  // 确认
  QPushButton *applyButton;

  // 设置动画延迟
  QLabel *delayInfo;
  QComboBox *delayEdit;
  QLabel *delayUnit;

  // 排序方式选择
  QLabel *sortMethodInfo;
  QComboBox *sortMethodEdit;

  // 开始排序
  QPushButton *sortButton;
  QPushButton *skipButton;
  // 打乱数组
  QPushButton *shuffleButton;

  // 启用日志
  QLabel *enableLogInfo;
  QCheckBox *enableLog;

  // 日志框
  static QTextEdit *logger;
public slots:
  void applySettings();
  void startSort();
  void shuffleArray();

public:
  ArrayWgtController();
  ~ArrayWgtController();
  static void info(QString &&s);
};

#endif // ARRAYWGTCONTROLLER_H
