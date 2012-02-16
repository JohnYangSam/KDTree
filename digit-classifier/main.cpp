/**********************************************************
 * File: main.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Main entry point for the digit classification program.
 * You should not need to modify this file in your solution.
 */

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  int result;
  QApplication a(argc, argv);
  {
    MainWindow w;
    w.show();
    result = a.exec();
  }
  
  return result;
}
