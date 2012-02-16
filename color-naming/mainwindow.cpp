/**********************************************************
 * File: mainwindow.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of the main window class for the Color
 * Naming program.  Comments have been added to the appropriate
 * sections.  You should not need to modify this file in your
 * submission.
 */

#include "mainwindow.h"
#include <QStatusBar>
#include <QCoreApplication>
#include <QMessageBox>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

/***** Module Helper Function *****/

/* Utility function to convert an integer to a string. */
static string IntegerToString(int val) {
  stringstream converter;
  converter << val;
  return converter.str();
}

/***** LoadingThread Implementation *****/

/* Loads the color data from disk into the out parameter.  This function
 * returns a boolean indicating whether it succeeded.
 */
bool MainWindow::LoadingThread::loadDataSet(KDTree<3, string>& kd) {
  /* Open the file; fail if we can't. */
  ifstream input("../../colors.txt", ios::binary);
  if (!input) return false;
  
  /* Read how many entries there are and preallocate space for them. */
  size_t count;
  if (!(input >> count)) return false;
  
  if(!input.ignore(1)) return false; // Skip the newline character.
  
  /* Keep reading data out of the file and parsing it to color data. */
  size_t read = 0;
  while (true) {
    /* Read RGB */
    char colorBuffer[3];
    input.read(colorBuffer, streamsize(3));
    
    /* Read name size. */
    char bufferSize = input.get();
    size_t toRead = (unsigned char)(bufferSize);
    
    /* Read name from file.  Note that because characters range from
     * 0 - 255, we can't overrun this buffer.
     */
    char nameBuffer[256];
    input.read(nameBuffer, streamsize(toRead));
    
    /* Transform from a char array to a point. */
    Point<3> pt;
    pt[0] = (unsigned char)(colorBuffer[0]);
    pt[1] = (unsigned char)(colorBuffer[1]);
    pt[2] = (unsigned char)(colorBuffer[2]);
    
    /* Check for stream integrity. */
    if (!input) break;
    
    /* Add to the data set. */
    kd.insert(pt, string(nameBuffer, nameBuffer + toRead));
    
    /* Keep the GUI informed of what's going on. */
    if (++read % 10000 == 0)
      emit onDataLoaded(read);
  }
  
  /* Ensure we read enough. */
  return read == count;
}

/* LoadingThread constructor just stores a pointer back to the main
 * window it works for.
 */
MainWindow::LoadingThread::LoadingThread(MainWindow* master) : master(master) {
  // Handled in initializer list
}

/* On execution, the thread loads the data from disk, builds a KD tree out of it,
 * then stores the result back in the master object.
 */
void MainWindow::LoadingThread::run() {
  /* Load in the color data from the file. */
  if (!loadDataSet(master->lookup)) {
    QMessageBox::critical(0, tr("An error occurred loading color data.  This program will now exit"), tr("Color Lookup"));
    QCoreApplication::quit();
    return;
  }
  emit onDoneIndexing();
}

/***** MainWindow Implementation *****/

/* Constructor configures the main dialog and fires off the loading thread. */
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle(QString("Color Lookup"));
  
  /* Create a color chooser and set it as the main widget. */
  colorChooser = new QColorDialog();
  colorChooser->setOptions(QColorDialog::NoButtons | QColorDialog::DontUseNativeDialog);
  setCentralWidget(colorChooser);
  
  /* Wire up color change notification. */
  connect(colorChooser, SIGNAL(currentColorChanged(QColor)), this, SLOT(handleColorChange(QColor)));
  
  /* Initialize status bar. */
  statusBar()->setSizeGripEnabled(false);
  statusBar()->showMessage(tr("Test!"));
  
  /* Fire off the loading thread! */
  loadingThread = new LoadingThread(this);
  
  /* Connect the thread to us. */
  connect(loadingThread, SIGNAL(onDataLoaded(int)), this, SLOT(handleDataLoaded(int)));
  connect(loadingThread, SIGNAL(onDoneIndexing()), this, SLOT(handleDoneIndexing()));
  
  /* Run the loading thread. */
  loadingThread->start(QThread::InheritPriority);
}

MainWindow::~MainWindow() {
  /* Leak everything.  This seems like a bad idea, but the program always
   * terminates after this window closes and so memory will be reclaimed
   * automatically.
   */
}

/* If the color changes, recompute the color name. */
void MainWindow::handleColorChange(const QColor& c) {
  /* Ensure that data is loaded so we don't use incorrect data. */
  if (!loadingThread->isFinished())
    return;
  
  /* Convert the color from a QColor to a std::vector. */
  Point<3> colorVector;
  colorVector[0] = c.red();
  colorVector[1] = c.green();
  colorVector[2] = c.blue();
  
  /* Use the kd-tree to get the color name, then display it. */
  statusBar()->showMessage(QString(lookup.kNNValue(colorVector, 3).c_str()));
}

/* When the loading thread says that data is available, display a progress indicator. */
void MainWindow::handleDataLoaded(int amountLoaded) {
  stringstream message;
  message << "Loaded " << amountLoaded << " points...";
  statusBar()->showMessage(QString(message.str().c_str()));
}

/* Update the status bar in response to data from the loading thread. */
void MainWindow::handleDoneIndexing() {
  statusBar()->showMessage(QString("Ready"));
}
