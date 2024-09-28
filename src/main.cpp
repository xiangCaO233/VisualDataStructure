#include "../headers/ArrayWgtController.h"
#include <QApplication>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  srand(time(nullptr));
  cout << "nmsl" << endl;
  // auto w = new ArrayWidget(128, false);
  auto w = new ArrayWgtController;
  w->show();
  // w->bubbleSort();
  return app.exec();
}
