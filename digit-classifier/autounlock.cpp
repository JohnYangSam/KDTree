/**********************************************************
 * File: autounlock.cpp
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of the AutoUnlock object.  Not much here is
 * that interesting; the benefit of this class is the guarantee
 * that these functions are called automatically.
 */

#include "autounlock.h"

/* Acquire the semaphore. */
AutoUnlock::AutoUnlock(QSemaphore& s) : sem(s) {
  sem.acquire(1);
}

/* Release the semaphore. */
AutoUnlock::~AutoUnlock() {
    sem.release(1);
}
