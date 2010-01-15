/*
 * Copyright (C) 2010, Gostai S.A.S.
 *
 * This software is provided "as is" without warranty of any kind,
 * either expressed or implied, including but not limited to the
 * implied warranties of fitness for a particular purpose.
 *
 * See the LICENSE file for more information.
 */

#ifndef LIBPORT_SYNCHRONIZER_HH
# define LIBPORT_SYNCHRONIZER_HH

# include <libport/export.hh>
# include <libport/condition.hh>
# include <libport/semaphore.hh>

namespace libport
{
  /** Synchronization point implementation.
   *
   * This class can be used to have multiple threads execute a critical code
   * section only when a master thread is at a specific point (check()).
   *
   * Usage:
   *   - The master thread must call check() when it is ready to let other
   *     threads execute their critical section. All currently blocked threads
   *     will execute their critical section before the master thread returns
   *     from check().
   *   - The other threads must enclose their critical section between
   *     construction and destruction of a Synchronizer::SynchroPoint.
   *     Its constructor will block until the master thread reaches check().
   *     The master thread will then be blocked until destructor is called.
   */
  class LIBPORT_API Synchronizer
  {
  public:
    Synchronizer();
    ~Synchronizer();

    void check();

    class LIBPORT_API SynchroPoint
    {
    public:
      SynchroPoint(Synchronizer& src);
      ~SynchroPoint();
    private:
      Synchronizer& sync_;
    };

  private:
    friend class SynchroPoint;
    libport::Condition cond_;
    libport::Semaphore done_;
    /* We must use two counters, since when we are in check(), once we
     * signal, new waiting threads might insert themselve before check
     * finishes.  So incoming waiting threads increase waiting_count,
     * and check() copies waiting into signaled, and reset waiting to
     * 0 (safe since we own the lock).  Unblocked threads decrement
     * signaled_count (safe, lock is owned), and signal the main
     * thread when it reaches 0.  This approch is better than once
     * sem++/sem-- per blocked thread (less syscalls).
     */
    size_t waiting_count_;
    size_t signaled_count_;
  };

}
#include "synchronizer.hxx"

#endif
