/**********************************************************
 * File: autounlock.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * A class that automatically acquires and releases a semaphore.
 * This ensures that acquire() is never called without a matching
 * call to release().  This is a specific instance of a technique
 * called Resource Acquisition Is Initialization (RAII) and is
 * a staple of modern C++ programming.
 */

#ifndef AUTOUNLOCK_H
#define AUTOUNLOCK_H

#include <QSemaphore>

/* Helper class to automatically acquire and release a semaphore. */
class AutoUnlock {
public:
  /* Constructor acquires a semaphore. */
  explicit AutoUnlock(QSemaphore& s);
  
  /* Destructor releases it. */
  ~AutoUnlock();
  
private:
  QSemaphore& sem;
  
  AutoUnlock(const AutoUnlock&); // No copying
  AutoUnlock& operator= (const AutoUnlock&); // No copying
};

#endif // AUTOUNLOCK_H
