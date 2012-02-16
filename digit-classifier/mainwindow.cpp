/**********************************************************
 * File: mainwindow.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of the main window class for the digit
 * classifier program.  You should not need to modify this
 * file in your solution.
 */

#include "mainwindow.h"
#include "autounlock.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCoreApplication>
#include <QStatusBar>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <cmath>

/***** Module Constants and Functions *****/

/* Utility function to convert from ints to strings. */
static string IntegerToString(int val) {
  stringstream converter;
  converter << val;
  return converter.str();
}

/* Given an image, shifts it over so that it hugs the top and left edges
 * of the image.
 */
static void TranslateToOrigin(grid<bool>& image) {
  size_t lowX = numeric_limits<size_t>::max(), lowY = numeric_limits<size_t>::max();
  
  /* Find the lowest x and y value. */
  for (size_t x = 0; x < image.numCols(); ++x) {
    for (size_t y = 0; y < image.numRows(); ++y) {
      if (image[y][x]) {
        lowX = min(x, lowX);
        lowY = min(y, lowY);
      }
    }
  }
  
  /* Rebuild the grid. */
  grid<bool> result(image.numRows(), image.numCols());
  for (size_t x = 0; x < image.numCols(); ++x)
    for (size_t y = 0; y < image.numRows(); ++y)
      if (image[y][x])
        result[y - lowY][x - lowX] = true;
  
  image = result;
}

/* Given an image hugging the top-left corner, shifts it over so that it's
 * centered.
 */
static void CenterImage(grid<bool>& image) {
  /* Find the max x and y coordinates. */
  size_t highX = 0, highY = 0;
  
  /* Find the maximum x and y value. */
  for (size_t x = 0; x < image.numCols(); ++x) {
    for (size_t y = 0; y < image.numRows(); ++y) {
      if (image[y][x]) {
        highX = max(x, highX);
        highY = max(y, highY);
      }
    }
  }
  
  /* We now have the dimensions.  If either are zero, we're done. */
  if (!highX || !highY) return;
  
  /* Figure out how much more wiggle room we have and shift it over by
   * that amount.
   */
  size_t dx = (image.numCols() - highX) / 2;
  size_t dy = (image.numRows() - highY) / 2;
  
  /* Rebuild the grid. */
  grid<bool> result(image.numRows(), image.numCols());
  for (size_t x = 0; x < image.numCols(); ++x)
    for (size_t y = 0; y < image.numRows(); ++y)
      if (image[y][x])
        result[y + dy][x + dx] = true;
  
  image = result;
}

/* Given an image whose upper corner is the origin, rescales it to fit in a 20 x 20 box. */
static void RescaleImage(grid<bool>& image) {
  /* Find the max x and y coordinates. */
  size_t highX = 0, highY = 0;
  
  /* Find the maximum x and y value. */
  for (size_t x = 0; x < image.numCols(); ++x) {
    for (size_t y = 0; y < image.numRows(); ++y) {
      if (image[y][x]) {
        highX = max(x, highX);
        highY = max(y, highY);
      }
    }
  }
  
  /* We now have the dimensions.  If either are zero, we're done. */
  if (!highX || !highY) return;
  
  /* Compute the new width and height based on the extents. */
  size_t width, height;
  if (highX > highY) {
    width = 20;
    height = size_t(ceil((20.0 * highY) / highX));
  } else {
    height = 20;
    width = size_t(ceil((20.0 * highX) / highY));
  }

  /* Use a nearest-neighbor scaling algorithm.  This works as follows.  First,
   * we add a data point for each pixel that's normalized to [0, 1] based on
   * its x, y position.  Next, we iterate over the normalized size and set each
   * pixel to its nearest-neighbor value.
   */
  KDTree<2, bool> points;
  for (size_t x = 0; x < highX; ++x) {
    for (size_t y = 0; y < highY; ++y) {
      Point<2> pt;
      pt[0] = double(x) / highX;
      pt[1] = double(y) / highY;
      points[pt] = image[y][x];
    }
  }

  /* Use nearest-neighbor lookup to interpolate the points. */
  grid<bool> result(image.numRows(), image.numCols());
  for (size_t x = 0; x < width; ++x) {
    for (size_t y = 0; y < height; ++y) {
      Point<2> pt;
      pt[0] = double(x) / width;
      pt[1] = double(y) / width;
      result[y][x] = points.kNNValue(pt, 1);
    }
  }
  
  image = result;
}

/* Helper function used as an algorithms callback.  It converts a boolean
 * true/false into a real-valued 1/-1
 */
static double BoolToValue(bool b) {
  return b ? 1 : -1;
}

/******* MainWindow Implementation *******/

/* Constructor initializes the main window, adds all of the proper components,
 * wires them into the appropriate callbacks, then fires off the two threads.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          canvasSemaphore(1),  // Binary semaphore guarding canvas
                                          queueSemaphore(1),   // Binary semaphore guarding queue
                                          queueReady(0)        // Counting semaphore for worker thread
{    
  worker = NULL;
  loader = NULL;
  
  setWindowTitle("Digit Classifier");
  
  /* Create the canvas. */
  canvas = new CanvasWidget(canvasSemaphore);
  
  /* ... and the buttons. */
  start = new QPushButton("Start");
  reset = new QPushButton("Clear");
  
  /* Wire ourselves into button notifications. */
  connect(reset, SIGNAL(clicked()), canvas, SLOT(reset()));
  connect(start, SIGNAL(clicked()), this, SLOT(onStart()));
  
  /* Lay everything out.  We'll begin by creating a row of some spaces
   * and the canvas widget which goes up top.
   */
  QHBoxLayout* canvasLayout = new QHBoxLayout();
  canvasLayout->addSpacing(100);
  canvasLayout->addWidget(canvas);
  canvasLayout->addSpacing(100);
  QWidget* canvasContainer = new QWidget;
  canvasContainer->setLayout(canvasLayout);
  
  /* We'll now create the main layout, which consists of the above widget
   * and the two buttons.
   */
  QVBoxLayout* layout = new QVBoxLayout;
  layout->addWidget(canvasContainer);
  layout->addWidget(start);
  layout->addWidget(reset);
  
  /* Create a widget with this layout and install it as the central widget. */
  QWidget* w = new QWidget;
  w->setLayout(layout);
  setCentralWidget(w);
  
  /* Create the loading thread and wire it in. */
  loader = new LoadingThread(this);
  connect(loader, SIGNAL(onDataLoaded(int)), this, SLOT(onDataLoaded(int)));
  connect(loader, SIGNAL(onStartIndexing()), this, SLOT(onStartIndexing()));
  connect(loader, SIGNAL(onDoneIndexing()), this, SLOT(onDoneIndexing()));
  
  /* Create the worker thread and wire it in. */
  worker = new WorkerThread(this);
  connect(worker, SIGNAL(onStartProcessing()), this, SLOT(onStartProcessing()));
  connect(worker, SIGNAL(onProcessingResult(int)), this, SLOT(onProcessingResult(int)));
  
  /* Fire the threads. */
  loader->start(QThread::InheritPriority);
  worker->start(QThread::InheritPriority);
}

/* Destructor needs to gracefully tell the worker thread to terminate before
 * it can finish running.
 */
MainWindow::~MainWindow() {
  /* Stop the worker by activating it with no data. */
  {
    AutoUnlock au(queueSemaphore);
    while (!analysisQueue.empty())
      analysisQueue.pop();
  }
  queueReady.release(1);
  
  /* Wait for the worker to terminate. */
  worker->wait();
}

/* When told to start, get the image from the canvas, process it so that
 * it's centered and scaled, then add it to the processing queue so that
 * the worker thread can go to town on it.
 */
void MainWindow::onStart() {
  /* Extract contents from the canvas. */
  grid<bool> image = canvas->canvasImage();
  
  /* Perform preprocessing on image to scale and center it. */
  TranslateToOrigin(image);
  RescaleImage(image);
  CenterImage(image);
  
  /* Convert from grid to vector. */
  Point<kImageSize> dataPoint;
  transform(image.begin(), image.end(), dataPoint.begin(), BoolToValue);
  
  /* Add to processing queue. */
  {
    AutoUnlock au(queueSemaphore);
    analysisQueue.push(dataPoint);
  }
  
  /* Wake up the processing thread. */
  queueReady.release(1);
}

/* These next three functions respond to notifications from the loading thread about
 * loading progress and report status messages appropriately.
 */
void MainWindow::onDataLoaded(int amount) {
  statusBar()->showMessage(("Loaded " + IntegerToString(amount) + " data points...").c_str());
}
void MainWindow::onDoneIndexing() {
  statusBar()->showMessage("Ready");
}

/* These next two functions respond to notifications from the worker thread and
 * display status messages appropriately.
 */
void MainWindow::onStartProcessing() {
  statusBar()->showMessage("Processing");
}
void MainWindow::onProcessingResult(int result) {
  statusBar()->showMessage(("Computer thinks this digit is a " + IntegerToString(result)).c_str());
}

/************************** LoadingThread Implementation ***************************/

/* Loads all of the image examples from disk. */
bool MainWindow::LoadingThread::loadDataSet(KDTree<kImageSize, unsigned char>& kd) try {
  /* These next lines tell the stream to generate exceptions on failure.
   * This greatly simplifies processing.
   */
  ifstream images; // Image file
  images.exceptions(ios::failbit | ios::badbit);
  images.open("../../training-images", ios::binary);
  
  ifstream labels; // Labels for those images
  labels.exceptions(ios::failbit | ios::badbit);
  labels.open("../../training-labels", ios::binary);
  
  /* Each file begins with a "magic number" identifying the file.
   * These numbers are stored as raw bytes in low-endian form.
   * These next few lines read the data from the file and confirm
   * that the values are what we'd expect them to be.
   */
  char buffer[4];
  images.read(buffer, 4);
  
  /* memcmp is a function which compares raw memory and returns an integer
   * based on their lexicographical ordering.  In particular, it should
   * return zero if the values match.
   */
  if (memcmp(buffer, "\x00\x00\x08\x03", 4) != 0)
    return false;
  
  /* Now read how many images there are.  This also requires some memory hackery
   * because the integer is stored in low-endian format and we need to reconstruct
   * it.
   */
  images.read(buffer, 4);
  size_t numImages = ((unsigned char)buffer[0] << 24) +
                     ((unsigned char)buffer[1] << 16) +
                     ((unsigned char)buffer[2] <<  8) +
                      (unsigned char)buffer[3];

  /* Read the image dimensions.  We really don't care about these, so we just
   * skip them.
   */
  images.ignore(streamsize(8));
  
  /* Now repeat this process for the labels file. */
  labels.read(buffer, 4);
  if (memcmp(buffer, "\x00\x00\x08\x01", 4) != 0)
    return false;
  
  labels.read(buffer, 4);
  size_t numLabels = ((unsigned char)buffer[0] << 24) +
                     ((unsigned char)buffer[1] << 16) +
                     ((unsigned char)buffer[2] <<  8) +
                      (unsigned char)buffer[3];

  /* Confirm that we have the same number of images as labels. */
  if (numLabels != numImages) return false;
  
  
  /* Read all images. */
  for (size_t i = 0; i < numLabels; ++i) {
    /* Images are stored as byte arrays ranging from 0 - 255.  We'll
     * read the whole image in and then convert it to binary white/black
     * to match our input format.
     */
    char buffer[kImageSize];
    images.read(buffer, kImageSize);
    
    /* Convert from 0 - 255 to -1 - +1 */
    Point<kImageSize> pt;
    for (size_t j = 0; j < kImageSize; ++j)
      pt[j] = (buffer[j] > 0 ? 1.0 : -1.0);
    
    /* Read label */
    char byte;
    labels.get(byte);
    
    kd.insert(pt, (unsigned char)byte);
    
    /* Keep the GUI informed about what's going on. */
    if (i % 1000 == 0)
      emit onDataLoaded(i);
  }
  return true;
} catch (const exception&) {
  /* On error, signal failure. */
  return false;
}


/* Constructor just stores a pointer back to the window that created us */
MainWindow::LoadingThread::LoadingThread(MainWindow* master) : master(master) {
  // Handled in initializer list
}

/* Main thread routine loads data and builds it into a KD tree. */
void MainWindow::LoadingThread::run() {
  /* Load in the color data from the file. */
  if (!loadDataSet(master->lookup)) {
    QMessageBox::critical(0, tr("An error occurred loading color data.  This program will now exit"), tr("Color Lookup"));
    QCoreApplication::quit();
    return;
  }
  
  /* Success! */
  emit onDoneIndexing();
}

/************** WorkerThread Implementation *************/

/* Constructor stores a pointer back to the window that created us. */
MainWindow::WorkerThread::WorkerThread(MainWindow* master) : master(master) {
  // Handled in initializer list
}

/* Thread routine continuously waits for data to be ready, then processes it. */
void MainWindow::WorkerThread::run() {
  while (true) {
    /* Wait for the queue to be ready. */
    master->queueReady.acquire();
    
    /* Grab the next thing out of the queue. */
    Point<kImageSize> dataPoint;
    {
      AutoUnlock au(master->queueSemaphore);
      
      /* If no data, we should terminate. */
      if (master->analysisQueue.empty())
        return;
      
      /* Otherwise go grab the next entry. */
      dataPoint = master->analysisQueue.front();
      master->analysisQueue.pop();
    }
    
    /* If we can't process (i.e. not loaded), don't process. */
    if (!master->loader->isFinished()) continue;
    
    emit onStartProcessing();
    
    /* Otherwise, ask the KD tree what this is... */
    emit onProcessingResult(master->lookup.kNNValue(dataPoint, 4));
  }
}
