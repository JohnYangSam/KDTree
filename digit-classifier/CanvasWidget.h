/**********************************************************
 * File: CanvasWidget.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * A class representing a window that can be drawn on like a
 * canvas.  This widget will be used by the main application
 * to get input from the user before invoking the classifier.
 * You do not need to modify the contents of this file.
 */

#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QSemaphore>
#include "grid.h"

/* The actual canvas class.  As with most Qt-related classes,
 * it contains its fair share of nonstandard C++ extensions.
 */
class CanvasWidget: public QWidget {
  Q_OBJECT // Indicate to the MOC that this is a Qt object
public:
  /* Constructor takes in a reference to the canvas-protection
   * semaphore and stores it for later use.
   */
  CanvasWidget(QSemaphore& sem, QWidget* parent = 0);
  
  /* Returns a copy of what's currently on the canvas.  This function
   * uses the semaphore passed in to the constructor to ensure that
   * the access is synchronized.
   */
  grid<bool> canvasImage() const;
  
  /* Constant defining the size of the image on one side. */
  static const int kImageDimension = 28;
  
protected:
  /* Respond to mouse events so that when the client clicks and drags
   * across the canvas an image will take form.
   */
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  
  /* Respond to a redraw by displaying the current canvas image. */
  virtual void paintEvent(QPaintEvent*);
                                       
public slots:
  /* Clears the canvas. */
  void reset();
  
private:
  grid<bool> image;
  QSemaphore& canvasLock;
};

#endif // CANVASWIDGET_H
