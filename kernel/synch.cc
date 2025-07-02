/*! \file synch.cc 
//  \brief Routines for synchronizing threads.  
//
//      Three kinds of synchronization routines are defined here: 
//      semaphores, locks and condition variables.
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation. We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts. While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original counter (whether
// that be disabled or enabled).
 * -----------------------------------------------------
 * This file is part of the Nachos-RiscV distribution
 * Copyright (c) 2022 University of Rennes 1.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details 
 * (see see <http://www.gnu.org/licenses/>).
 * -----------------------------------------------------
*/

#include "kernel/system.h"
#include "kernel/msgerror.h"
#include "kernel/scheduler.h"
#include "kernel/synch.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
/*! 	Initializes a semaphore, so that it can be used for synchronization.
//
// \param debugName is an arbitrary name, useful for debugging only.
// \param initialcounter is the initial counter of the semaphore.
*/
//----------------------------------------------------------------------
Semaphore::Semaphore(char* debugName, uint32_t initialCount)
{
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  counter = initialCount;
  waiting_queue = new Listint;
  type = SEMAPHORE_TYPE;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
/*! 	De-allocates a semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
*/
//----------------------------------------------------------------------
Semaphore::~Semaphore()
{
  type = INVALID_TYPE;
  if (!waiting_queue->IsEmpty()) {
    DEBUG('s', (char *)"Destructor of semaphore \"%s\", queue is not empty!!\n",name);
    Thread *t =  (Thread *)waiting_queue->Remove();
    DEBUG('s', (char *)"Queue contents %s\n",t->GetName());
    waiting_queue->Append((void *)t);
  }
  ASSERT(waiting_queue->IsEmpty());
  delete [] name;
  delete waiting_queue;
}

//----------------------------------------------------------------------
// Semaphore::P
/*!
//      Decrement the counter, and wait if it becomes < 0. Checking the
//	counter and decrementing must be done atomically, so we
//	need to disable interrupts before checking the counter.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void
Semaphore::P() {
  printf("**** Warning: method Semaphore::P is not implemented yet\n");

  exit(ERROR);

}
#endif


#ifdef ETUDIANTS_TP
void Semaphore::P() {

	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	counter--;
	
	if(counter < 0){

		DEBUG('s', (char *)"P() bloquant (sem %s) par le thread %s\n", name, g_current_thread->GetName());
		Thread *courant = g_current_thread;
		waiting_queue->Append(courant);
		DEBUG('s', (char *)"P() test waiting_queue vide (sem %s) après ajout du thread : %i\n", name, waiting_queue->IsEmpty());
		courant->Sleep();
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif

//----------------------------------------------------------------------
// Semaphore::V
/*! 	Increment semaphore counter, waking up a waiting thread if any.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that interrupts
//	are disabled when it is called.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void
Semaphore::V() {
   printf("**** Warning: method Semaphore::V is not implemented yet\n");

    exit(ERROR);

}
#endif

#ifdef ETUDIANTS_TP
void
Semaphore::V() {

	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	counter++;
	DEBUG('s', (char *)"V() (sem %s) test waiting_queue vide par le thread %s : %i\n", name, g_current_thread->GetName(), waiting_queue->IsEmpty());
	//On relance le premier thread de la file d'attente
	if(!waiting_queue->IsEmpty()){

		Thread *temp = (Thread *)waiting_queue->Remove();
		DEBUG('s', (char *)"V() (sem %s) par le thread %s (resultat %p)\n", name, g_current_thread->GetName(), temp);
		g_scheduler->ReadyToRun(temp);
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif

//----------------------------------------------------------------------
// Lock::Lock
/*! 	Initialize a Lock, so that it can be used for synchronization.
//      The lock is initialy free
//  \param "debugName" is an arbitrary name, useful for debugging.
*/
//----------------------------------------------------------------------
Lock::Lock(char* debugName) {
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  waiting_queue = new Listint;
  free = true;
  owner = NULL;
  type = LOCK_TYPE;
}


//----------------------------------------------------------------------
// Lock::~Lock
/*! 	De-allocate lock, when no longer needed. Assumes that no thread
//      is waiting on the lock.
*/
//----------------------------------------------------------------------
Lock::~Lock() {
  type = INVALID_TYPE;
  ASSERT(waiting_queue->IsEmpty());
  delete [] name;
  delete waiting_queue;
}

//----------------------------------------------------------------------
// Lock::Acquire
/*! 	Wait until the lock become free.  Checking the
//	state of the lock (free or busy) and modify it must be done
//	atomically, so we need to disable interrupts before checking
//	the counter of free.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Lock::Acquire() {
   printf("**** Warning: method Lock::Acquire is not implemented yet\n");

    exit(ERROR);

}
#endif

#ifdef ETUDIANTS_TP
void Lock::Acquire() {
  
  	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);

	if(free){
		free = false;
		owner = g_current_thread;
	}
	else{
	
		Thread *courant = g_current_thread;
		waiting_queue->Append(courant);
		courant->Sleep();
	}

	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif

//----------------------------------------------------------------------
// Lock::Release
/*! 	Wake up a waiter if necessary, or release it if no thread is waiting.
//      We check that the lock is held by the g_current_thread.
//	As with Acquire, this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Lock::Release() {
    printf("**** Warning: method Lock::Release is not implemented yet\n");

    exit(ERROR);

}
#endif

#ifdef ETUDIANTS_TP
void Lock::Release() {
  
  //Sauvegarde du flag interruption et désactivation de celles-ci
IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);

if (isHeldByCurrentThread()){

  if (waiting_queue->IsEmpty()){
    free=true;
    owner=NULL;
  }
  else{
    owner = (Thread *)waiting_queue->Remove();
    g_scheduler->ReadyToRun(owner);
  }
}

//On rétablit le flag interruption à son état antérieur
g_machine->interrupt->SetStatus(save);
}
#endif

//----------------------------------------------------------------------
// Lock::isHeldByCurrentThread
/*! To check if current thread hold the lock
*/
//----------------------------------------------------------------------
bool Lock::isHeldByCurrentThread() {return (g_current_thread == owner);}	

//----------------------------------------------------------------------
// Condition::Condition
/*! 	Initializes a Condition, so that it can be used for synchronization.
//
//    \param  "debugName" is an arbitrary name, useful for debugging.
*/
//----------------------------------------------------------------------
Condition::Condition(char* debugName) { 
  name = new char[strlen(debugName)+1];
  strcpy(name,debugName);
  waiting_queue = new Listint;
  type = CONDITION_TYPE;
}

//----------------------------------------------------------------------
// Condition::~Condition
/*! 	De-allocate condition, when no longer needed.
//      Assumes that nobody is waiting on the condition.
*/
//----------------------------------------------------------------------
Condition::~Condition() {
  type = INVALID_TYPE;
  ASSERT(waiting_queue->IsEmpty());
  delete [] name;
  delete waiting_queue;
}

//----------------------------------------------------------------------
// Condition::Wait
/*! Block the calling thread (put it in the wait queue).
//  This operation must be atomic, so we need to disable interrupts.
*/	
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Condition::Wait() { 
    printf("**** Warning: method Condition::Wait is not implemented yet\n");

    exit(ERROR);

}
#endif

#ifdef ETUDIANTS_TP
void Condition::Wait() { 
    //Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	waiting_queue->Append(g_current_thread);
	g_current_thread->Sleep();
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif

//----------------------------------------------------------------------
// Condition::Signal
/*! Wake up the first thread of the wait queue (if any). 
// This operation must be atomic, so we need to disable interrupts.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Condition::Signal() { 
    printf("**** Warning: method Condition::Signal is not implemented yet\n");

    exit(ERROR);

}
#endif

#ifdef ETUDIANTS_TP
void Condition::Signal() { 
	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	if (!waiting_queue->IsEmpty()){
	
		g_scheduler->ReadyToRun((Thread *)waiting_queue->Remove());
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif

//----------------------------------------------------------------------
// Condition::Broadcast
/*! Wake up all threads waiting in the waiting_queue of the condition
// This operation must be atomic, so we need to disable interrupts.
*/
//----------------------------------------------------------------------
#ifndef ETUDIANTS_TP
void Condition::Broadcast() { 
  printf("**** Warning: method Condition::Broadcast is not implemented yet\n");

  exit(ERROR);

}
#endif

#ifdef ETUDIANTS_TP
void Condition::Broadcast() { 
	//Sauvegarde du flag interruption et désactivation de celles-ci
	IntStatus save = g_machine->interrupt->SetStatus(INTERRUPTS_OFF);
	
	while (!waiting_queue->IsEmpty()){
		g_scheduler->ReadyToRun((Thread *)waiting_queue->Remove());
	}
	
	//On rétablit le flag interruption à son état antérieur
	g_machine->interrupt->SetStatus(save);
}
#endif
