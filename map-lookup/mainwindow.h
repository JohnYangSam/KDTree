/**********************************************************
 * File: mainwindow.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Defines the main window for the Map Lookup program.  You
 * should not need to modify the contents of this file.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QThread>
#include <string>
#include <map>
#include "../KDTree.h"
using namespace std;

/* Forward-declare the class responsible for displaying the world map. */
class PictureDisplay;

class MainWindow : public QMainWindow {
  Q_OBJECT // More Qt hackery

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private:
  PictureDisplay* worldMapPic;   // The widget that draws the earth.
  KDTree<2, string> kd;          // The kd-tree that does 1-NN lookup.
  map<string, string> geoLookup; // Mapping from FIPS 10-4 codes to place names

  class LoadingThread;           // Thread that does loading off the main GUI loop.
  friend class LoadingThread;
  LoadingThread* loadThread;
                           
private slots:
  void onMapClick(const QPoint& where); // Triggered when the user clicks on the map.
  
  void handleLoadData(int);             // Triggered when the loading thread makes progress
  void handleDoneIndexing();
};

/* A thread class responsible for loading data from disk.  This is done off the main thread
 * to keep the GUI responsive.
 */
class MainWindow::LoadingThread: public QThread {
  Q_OBJECT
public:
  explicit LoadingThread(MainWindow* master);
  virtual void run();
  
signals:
  void onLoadData(int amount); // Emitted based on what the thread is doing.
  void onStartIndexing();
  void onDoneIndexing();
  
private:
  bool loadGeographicData(KDTree<2, string>& kd);
  bool loadGeoCodes(map<string, string>& geoLookup);
  
  MainWindow* const master;
};

/* This code is based on an example from
 *
 * http://www.vision.ee.ethz.ch/computing/sepp-irix/qt-2.3.1-mo/picture-picture-cpp.html
 *
 * However, it contains numerous fixes to make it work correctly in our context.
 */
class PictureDisplay : public QWidget {
  Q_OBJECT

public:
  explicit PictureDisplay(const string& filename);

protected:
  void paintEvent(QPaintEvent *);     // Draw the world
  void mousePressEvent(QMouseEvent*); // Report the click back to the main window
  
private:
  QImage pict;                        // The world map
  
signals:
  void clicked(const QPoint& where);  // Emitted on a click
};

#endif // MAINWINDOW_H
