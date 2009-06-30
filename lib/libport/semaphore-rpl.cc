#include <libport/semaphore.h>
#include <libport/assert.hh>
#include <libport/cerrno>

// Provide replacement for semaphore.h functions.  This is different
// from semaphore.cc which implements the libport::Semaphore class.

namespace libport
{

# if defined WIN32 || defined LIBPORT_WIN32

  sem_t*
  sem_open(const char* /* name */,
           int /* oflag */,
           unsigned int /*mode_t*/ /* mode */,
           unsigned int /* value */)
  {
    return 0; // Use sem_init instead.
  }

  int
  sem_init(sem_t* sem, int, unsigned value)
  {
    *sem = CreateSemaphore(0, value, MAXLONG, 0);
    if (!sem)
      errabort("CreateSemaphore");
    return 0;
  }

  int
  sem_post(sem_t* sem)
  {
    if (!ReleaseSemaphore(*sem, 1, 0))
      errabort("ReleaseSemaphore");
    return 0;
  }

  int
  sem_wait(sem_t* sem)
  {
    if (WaitForSingleObject(*sem, INFINITE) == WAIT_FAILED)
      errabort("WaitForSingleObject");
    return 0;
  }

  int
  sem_timedwait(sem_t* sem, const struct timespec *abs_timeout)
  {
    switch (WaitForSingleObject(*sem,
                                abs_timeout->tv_sec * 1000
                                + abs_timeout->tv_nsec / 1000000))
    {
    case WAIT_FAILED:
      errabort("WaitForSingleObject");
    case WAIT_TIMEOUT:
      errno = ETIMEDOUT;
      return -1;
    default:
      return 0;
    }
  }

  int
  sem_destroy(sem_t* sem)
  {
    return CloseHandle(*sem) ? 0 : -1;
  }

  int
  sem_getvalue(sem_t* /* sem */, int* v)
  {
    *v = 1; //TODO: implement
    // Maybe look at: http://www.codeguru.com/Cpp/W-P/win32/article.php/c1423
    return 0;
  }
#endif /* !WIN32 */

} // namespace libport