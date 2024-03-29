/* This file is part of Into.
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#include "PiiDefaultOperation.h"
#include "PiiYdinTypes.h"
#include "PiiSimpleProcessor.h"

#include <QMutex>

PiiSimpleProcessor::PiiSimpleProcessor(PiiDefaultOperation* parent) :
  PiiOperationProcessor(parent),
  _bReset(false), _bProcessing(false),
  _pStateMutex(&(parent->_d()->stateMutex))
{
}

bool PiiSimpleProcessor::tryToReceive(PiiAbstractInputSocket* sender, const PiiVariant& object) throw ()
{
  QMutexLocker lock(_pStateMutex);

  // If the processor has not been initialized for execution, we
  // discard of the object right away.
  if (!_bReset)
    return true;
  else if (_pParentOp->_d()->state member_of (PiiOperation::Stopped, PiiOperation::Paused))
    _pParentOp->setState(PiiOperation::Running);

  PiiInputSocket* pInput = static_cast<PiiInputSocket*>(sender);
  if (pInput->canReceive())
    {
      pInput->receive(object);
      /*PiiInputSocket* pInput = static_cast<PiiInputSocket*>(sender);
      qDebug("%s: %d objects in queue",
             qPrintable(pInput->objectName()), pInput->queueLength());
      */
      // Avoid recursive/concurrent calls to process().
      if (_bProcessing)
        return true;
      try
        {
          do
            {
              PiiFlowController::FlowState state;
              try
                {
                  // See if we can process the objects.
                  //qDebug("%s: calling flow controller", qPrintable(_pParentOp->objectName()));
                  state = _pFlowController->prepareProcess();
                  //qDebug("%s: flow controller returned %d", qPrintable(_pParentOp->objectName()), int(state));
                  if (state == PiiFlowController::IncompleteState)
                    break;
                }
              catch (...)
                {
                  // If a synchronization or any other error
                  // occurs, we need to ensure the lock is opened.
                  lock.unlock();
                  throw;
                }

              // Exit critical section. The objects have been
              // safely stored for process(), and we can receive
              // more objects into the inputs. But before exit we
              // still need to safely set the _bProcessing flag.
              _bProcessing = true;

              lock.unlock();

              _pParentOp->sendSyncEvents(_pFlowController);

              switch (state)
                {
                case PiiFlowController::ProcessableState:
                  _pParentOp->processLocked(); // may throw
                case PiiFlowController::SynchronizedState:
                case PiiFlowController::IncompleteState:
                  break;
                case PiiFlowController::ReconfigurableState:
                  _pParentOp->applyPropertySet(_pFlowController->propertySetName()); // may throw
                  break;
                case PiiFlowController::PausedState:
                  _pParentOp->operationPaused(); // throws
                case PiiFlowController::FinishedState:
                  _pParentOp->operationStopped(); // throws
                case PiiFlowController::ResumedState:
                  _pParentOp->operationResumed(); // may throw
                  break;
                }

              // Go back to critical section and reset the processing
              // flag.
              lock.relock();
              _bProcessing = false;
            }
          while (_bReset);
        }
      catch (PiiExecutionException& ex)
        {
          //qDebug("%s caught %s.", _pParentOp->metaObject()->className(),
          //       PiiExecutionException::errorName(ex.code()));
          // Exceptions won't be thrown from the critical section
          // above. Therefore, the mutex is not held if we are
          // here. Must relock...
          lock.relock();
          _bProcessing = false;

          if (ex.code() == PiiExecutionException::Paused &&
              // Someone may have interrupted the operation just while pausing.
              _pParentOp->state() != PiiOperation::Stopped)
            {
              _pParentOp->setState(PiiOperation::Paused);
              return true;
            }
          // If an error occured, signal the error. Due to this, the
          // engine will stop every operation.
          if (ex.code() == PiiExecutionException::Error)
            emit _pParentOp->errorOccured(_pParentOp, ex.message());

          // In any case (error/interruption/finish), the operation must
          // stop handling objects.
          _bReset = false;
          _pParentOp->setState(PiiOperation::Stopped);
        }
    } // if (sender->canReceive())
  else
    return false;

  return true;
}

void PiiSimpleProcessor::check(bool reset)
{
  _bProcessing = false;
  if (reset)
    _bReset = true;
}

void PiiSimpleProcessor::start()
{
  QMutexLocker lock(_pStateMutex);

  if (_pParentOp->state() == PiiOperation::Pausing)
    return;

  // If an operation resumes from pause and it has no connected
  // inputs, it must send a resume tag to all outputs now.
  if (_pParentOp->state() == PiiOperation::Paused)
    {
      if (_pFlowController == 0)
        {
          _pParentOp->setState(PiiOperation::Running);
          try { _pParentOp->operationResumed(); } catch (...) {}
        }
    }
  // Paused state changes to running when resume tags are received.
  // Other states change to running immediately.
  else
    _pParentOp->setState(PiiOperation::Running);
}

void PiiSimpleProcessor::interrupt()
{
  QMutexLocker lock(_pStateMutex);
  _bReset = false;
  _pParentOp->setState(PiiOperation::Stopped);
}

void PiiSimpleProcessor::reconfigure(const QString& propertySetName)
{
  try
    {
      // At least one connected input -> reconfigure only after
      // receiving tags.
      if (_pFlowController != 0)
        return;

      // Set properties and send tags.
      _pParentOp->applyPropertySet(propertySetName); // may throw
    }
  catch (PiiExecutionException& ex)
    {
      emit _pParentOp->errorOccured(_pParentOp,
                                    QCoreApplication::translate("PiiDefaultOperation",
                                                                "Reconfiguring %1 failed. %2")
                                    .arg(_pParentOp->metaObject()->className()).arg(ex.message()));
    }
}

void PiiSimpleProcessor::pause()
{
  stop(PiiOperation::Paused);
}

void PiiSimpleProcessor::stop()
{
  stop(PiiOperation::Stopped);
}

void PiiSimpleProcessor::stop(PiiOperation::State finalState)
{
  _pStateMutex->lock();
  if (_pParentOp->state() == PiiOperation::Running)
    {
      // If any of the inputs is connected, we just turn into an
      // intermediate state (Pausing/Stopping)
      if (_pFlowController != 0)
        {
          _pParentOp->setState(finalState == PiiOperation::Stopped ?
                               PiiOperation::Stopping :
                               PiiOperation::Pausing);
          _pStateMutex->unlock();
          return;
        }
      else
        // Otherwise, turn directly to the final state
        _pParentOp->setState(finalState);

      _pStateMutex->unlock();

      try
        {
          if (finalState == PiiOperation::Paused)
            _pParentOp->operationPaused(); // throws
          else
            _pParentOp->operationStopped(); // throws
        }
      catch (...)
        {}
    }
  else
    _pStateMutex->unlock();
}

bool PiiSimpleProcessor::wait(unsigned long /*time*/)
{
  return true;
}

void PiiSimpleProcessor::setProcessingPriority(QThread::Priority /*priority*/)
{ }

QThread::Priority PiiSimpleProcessor::processingPriority() const
{
  return QThread::NormalPriority;
}

int PiiSimpleProcessor::activeInputGroup() const
{
  return _pFlowController->activeInputGroup();
}
