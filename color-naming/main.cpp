/**********************************************************
 * File: main.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Main entry point for the Color Naming program.  This just
 * sets up the Qt framework and creates the initial window.
 */
#include <QtGui/QApplication>
#include <QColorDialog>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
