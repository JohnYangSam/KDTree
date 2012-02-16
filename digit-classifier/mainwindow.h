/**********************************************************
 * File: mainwindow.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Defines the main window class for the digit classifier
 * program.  The main window spawns off two threads - one
 * which reads data from disk, and one which processes
 * digit drawings when they become available.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QPushButton>
#include <QThread>
#include <QSemaphore>
#include <queue>
#include "CanvasWidget.h"
#include "../KDTree.h"

/* Constant: kImageDimension
 * Value: The size of one side of an image.
 */
const size_t kImageDimension = 28;

/* Constant: kImageSize
 * Value: The number of pixels in an image.
 */
const size_t kImageSize = kImageDimension * kImageDimension;

class MainWindow : public QMainWindow {
    Q_OBJECT // More QT hackery

public:
  /* Constructor sets up the main window. */
  MainWindow(QWidget* parent = 0);
  
  /* Destructor waits for the worker to terminate, then quits. */
  ~MainWindow();
  
private:
  CanvasWidget* canvas;       // The canvas where the user draws.
  QPushButton* start, *reset; // Buttons for user input.
  
  QSemaphore canvasSemaphore; // Synchronization objects
  QSemaphore queueSemaphore;
  QSemaphore queueReady;
  
  class WorkerThread;  // A thread that processes digits when they're ready.
  class LoadingThread; // A thread that loads data from disk.
  friend class WorkerThread;
  friend class LoadingThread;
  
  WorkerThread*  worker; // The instance of the worker thread.
  LoadingThread* loader; // The instance of the loading thread.
  
  KDTree<kImageSize, unsigned char> lookup; // KDTree used for classification.
  
  queue< Point<kImageSize> > analysisQueue;    // List of images to classify.

public slots:
  void onStart();               // User wants to classify a digit
  
  void onDataLoaded(int);       // Status messages from loading thread.
  void onDoneIndexing();
  
  void onStartProcessing();     // Status messages from the worker thread.
  void onProcessingResult(int);
};

/* This thread is responsible for loading data from disk to keep the GUI
 * responsive.
 */
class MainWindow::LoadingThread: public QThread {
  Q_OBJECT

public:
  explicit LoadingThread(MainWindow* master);
  virtual void run();

private:
  bool loadDataSet(KDTree<kImageSize, unsigned char>& kd);
    
  MainWindow* const master;
  
signals:
  void onDataLoaded(int amountLoaded); // Emitted in response to data loads.
  void onDoneIndexing();
};

/* This thread is responsible for actually doing the classification.  Since
 * classification is expensive, this is done outside of the main thread.
 */
class MainWindow::WorkerThread: public QThread {
  Q_OBJECT

public:
  explicit WorkerThread(MainWindow* master);
  virtual void run();

private:
  MainWindow* const master;

signals:
  void onStartProcessing();
  void onProcessingResult(int);
};

#endif // MAINWINDOW_H
