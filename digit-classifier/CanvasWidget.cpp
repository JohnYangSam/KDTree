/**********************************************************
 * File: CanvasWidget.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of the CanvasWidget object.  You should not
 * need to modify or use the contents of this file.
 */

#include "CanvasWidget.h"
#include "autounlock.h"
#include <QMouseEvent>
#include <QPainter>
#include <algorithm>
#include <iostream>

/***** Module Constants *****/

/* A constant specifying how many pixels on the canvas to allocate to
 * each pixel in the image.
 */
static const int kPixelsPerCell = 5;

/* Canvas constructor stores the semaphore, initializes the grid, and
 * forces the size to a reasonable value.
 */
CanvasWidget::CanvasWidget(QSemaphore& sem, QWidget* parent) : canvasLock(sem), QWidget(parent) {
  /* Configure window properties. */
  setMinimumSize(kPixelsPerCell * kImageDimension, kPixelsPerCell * kImageDimension);
  setMaximumSize(kPixelsPerCell * kImageDimension, kPixelsPerCell * kImageDimension);
  
  /* Set up the grid. */
  image.resize(size_t(kImageDimension), size_t(kImageDimension));
}

/* On invocation of reset, clear the canvas and redraw the display. */
void CanvasWidget::reset() {
  /* Lock the canvas so that nothing else can use it, then reset it. */
  {
    AutoUnlock au(canvasLock);
    fill(image.begin(), image.end(), false);
  }
  
  /* Issue a repaint when convenient. */
  update();
}

/* mouseMove events are only sent when the mouse button is down.  When that
 * happens, update the canvas in a 3x3 rectangle centered at the cursor.
 */
void CanvasWidget::mouseMoveEvent(QMouseEvent* event) {
  /* Transform from pixel space to canvas space by normalizing
   * the click location.
   */
  int x = event->x() / kPixelsPerCell;
  int y = event->y() / kPixelsPerCell;
  
  /* Ensure that we're the only ones with access to the canvas. */
  AutoUnlock au(canvasLock);
  
  /* Consider everything in a 3x3 box. */
  for (int xP = x - 1; xP <= x + 1; ++xP) {
    for (int yP = y - 1; yP <= y + 1; ++ yP) {
      /* If the point is in bounds, mark it. */
      if (xP >= 0 && yP >= 0 && xP < kImageDimension && yP < kImageDimension) {
        image[yP][xP] = true;
        
        /* This step tells the window to redraw the region around where we made
         * the change.
         */
        update(xP * kPixelsPerCell,        yP * kPixelsPerCell,
               (xP + 1) * kPixelsPerCell, (yP + 1) * kPixelsPerCell);
      }
    }
  }
}

/* On a click, do the same thing as when the mouse was dragged. */
void CanvasWidget::mousePressEvent(QMouseEvent* event) {
  mouseMoveEvent(event);
}

/* In response to a draw event, paint the canvas based on the image the user
 * has drawn.
 */
void CanvasWidget::paintEvent(QPaintEvent *) {
  /* Make sure we have exclusive access. */
  AutoUnlock au(canvasLock);
  
  /* Set up the painter to draw this object. */
  QPainter painter(this);
  
  /* Draw each point. */
  for (int x = 0; x < kImageDimension; ++x) {
    for (int y = 0; y < kImageDimension; ++y) {
      /* Setting the pen and brush color ensures that each pixel draws as a solid rectangle
       * of the appropriate color.
       */
      painter.setPen(image[y][x] ? QColor(0, 0, 0) : QColor(255, 255, 255));
      painter.setBrush(QBrush(image[y][x] ? QColor(0, 0, 0) : QColor(255, 255, 255), Qt::SolidPattern));
      painter.drawRect(QRect(x * kPixelsPerCell, y * kPixelsPerCell, (x + 1) * kPixelsPerCell, (y + 1) * kPixelsPerCell));
    }
  }
}

/* When retrieving the canvas, ensure that we lock the canvas to avoid race conditions. */
grid<bool> CanvasWidget::canvasImage() const {
  AutoUnlock au(canvasLock);
  grid<bool> result = image;
  return result;
}
