/*
 * Synchronization primitives.
 * See synch.h for specifications of the functions.
 */

#include <types.h>
#include <lib.h>
#include <synch.h>
#include <thread.h>
#include <curthread.h>
#include <machine/spl.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *namearg, int initial_count)
{
	struct semaphore *sem;

	assert(initial_count >= 0);

	sem = kmalloc(sizeof(struct semaphore));
	if (sem == NULL) {
		return NULL;
	}

	sem->name = kstrdup(namearg);
	if (sem->name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->count = initial_count;
	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	spl = splhigh();
	assert(thread_hassleepers(sem)==0);
	splx(spl);

	/*
	 * Note: while someone could theoretically start sleeping on
	 * the semaphore after the above test but before we free it,
	 * if they're going to do that, they can just as easily wait
	 * a bit and start sleeping on the semaphore after it's been
	 * freed. Consequently, there's not a whole lot of point in 
	 * including the kfrees in the splhigh block, so we don't.
	 */

	kfree(sem->name);
	kfree(sem);
}

void 
P(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	assert(in_interrupt==0);

	spl = splhigh();
	while (sem->count==0) {
		thread_sleep(sem);
	}
	assert(sem->count>0);
	sem->count--;
	splx(spl);
}

void
V(struct semaphore *sem)
{
	int spl;
	assert(sem != NULL);
	spl = splhigh();
	sem->count++;
	assert(sem->count>0);
	thread_wakeup(sem);
	splx(spl);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *
lock_create(const char *name)
{
	struct lock *lock;

	lock = kmalloc(sizeof(struct lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->name = kstrdup(name);
	if (lock->name == NULL) {
		kfree(lock);
		return NULL;
	}
	
	// add stuff here as needed
	lock->holder = NULL;	// upon creation, no thread holds the lock	
	lock->held = 0;		// lock is not held
	return lock;
}

void
lock_destroy(struct lock *lock)
{
	assert(lock != NULL);

	// add stuff here as needed
	if(lock->held) {	// if lock is held by current thread
		lock->holder = NULL;	// no one holds lock after descruction
		lock->held = 0;		// current thread lets go of lock
	}	

	
	kfree(lock->name);
	kfree(lock);
}

void
lock_acquire(struct lock *lock)
{
	// Write this
	assert(in_interrupt == 0);	// check if an interrupt is not being currently handled
	assert(lock != NULL);		// check if lock exists
	assert(lock_do_i_hold(lock)==0);// check if lock can be held
	int spl = splhigh();		// set high priority
	while(lock->held == 1) {thread_sleep(lock);}	// current thread sleeps if lock is being held
	lock->held = 1;			// once no longer held, aquire lock by marking it as held
	lock->holder = curthread;	// and aquire by marking current thread as holder
	splx(spl);			// remove high priority
	

	//(void)lock;  // suppress warning until code gets written
}

void
lock_release(struct lock *lock)
{
	// Write this
	assert(lock != NULL);		// check if lock exists
	assert(lock->held == 1);	// check if lock is toggled to be held	
	assert(lock_do_i_hold(lock)==1);// check if lock is currently being held by current thread
	int spl = splhigh();		// set high priority
	lock->holder = NULL;		// assign no owner to lock
	lock->held = 0;			// toggle lock to not be held
	thread_wakeup(lock);		// awaken threads sleeping on lock
	splx(spl);			// remove high priority


	//(void)lock;  // suppress warning until code gets written
}

int
lock_do_i_hold(struct lock *lock)
{
	// Write this
	assert(lock != NULL);		// check if lock exists
	if (lock->held == 0) {return 0;}// return 0 if lock is not held by any thread
	if (lock->holder == curthread) {return 1;}	// return 1 if lock is held by current thread
	else {return 0;}		// return 0 otherwise


	//(void)lock;  // suppress warning until code gets written

	//return 1;    // dummy until code gets written
}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(struct cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->name = kstrdup(name);
	if (cv->name==NULL) {
		kfree(cv);
		return NULL;
	}
	
	// add stuff here as needed
	
	return cv;
}

void
cv_destroy(struct cv *cv)
{
	assert(cv != NULL);

	// add stuff here as needed
	
	kfree(cv->name);
	kfree(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	// 1. release the lock
	// 2. put the thread to sleep on cv
	// 3. re-aquire the lock

	assert(in_interrupt == 0);	// check if an interrupt is not being currently handled
	assert(lock != NULL);		// check if lock exists
	assert(cv != NULL);		// check if cv exists
	int spl = splhigh();		// set high priority
	lock_release(lock);		// release the lock
	thread_sleep(cv);		// put the thread to sleep on cv
	lock_acquire(lock);		// re-aquire the lock
	splx(spl);			// remove high priority

	// Write this
	//(void)cv;    // suppress warning until code gets written
	//(void)lock;  // suppress warning until code gets written
}

void
cv_signal(struct cv *cv, struct lock *lock)
{

	assert(lock != NULL);		// check if lock exists
	assert(cv != NULL);		// check if cv exists
	assert(lock_do_i_hold(lock)==1);// check if the lock is held by the current thread
	int spl = splhigh();		// set high priority
	lock_release(lock);		// release the lock
	thread_wakeup(curthread);	// wake up the current thread
	lock_acquire(lock);		// re-acquire the lock
	splx(spl);			// remove high priority

	// Write this
	//(void)cv;    // suppress warning until code gets written
	//(void)lock;  // suppress warning until code gets written
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	assert(lock != NULL);		// check if lock exists
	assert(cv != NULL);		// check if cv exists
	assert(lock_do_i_hold(lock)==1);// check if the lock is held by current thread
	int spl = splhigh();		// set high priority
	lock_release(lock);		// release the lock
	thread_wakeup(cv);		// wake up all threads sleeping on cv
	lock_acquire(lock);		// re-acquire the lock
	splx(spl);			// remove high priority

	// Write this
	//(void)cv;    // suppress warning until code gets written
	//(void)lock;  // suppress warning until code gets written
}
