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

#include "PiiBoundaryFinderOperation.h"

#include "PiiBoundaryFinder.h"
#include <PiiYdinTypes.h>

PiiBoundaryFinderOperation::Data::Data() :
  dThreshold(0),
  iMinLength(0),
  iMaxLength(INT_MAX)
{
}

PiiBoundaryFinderOperation::PiiBoundaryFinderOperation() : PiiDefaultOperation(new Data)
{
  setThreadCount(1);
  PII_D;
  addSocket(new PiiInputSocket("image"));
  addSocket(d->pBoundaryOutput = new PiiOutputSocket("boundary"));
  addSocket(d->pBoundariesOutput = new PiiOutputSocket("boundaries"));
  addSocket(d->pLimitsOutput = new PiiOutputSocket("limits"));
  addSocket(d->pMaskOutput = new PiiOutputSocket("mask"));
}

void PiiBoundaryFinderOperation::process()
{
  PiiVariant obj = readInput();

  switch (obj.type())
    {
      PII_GRAY_IMAGE_CASES(findBoundaries, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}

template <class T> void PiiBoundaryFinderOperation::findBoundaries(const PiiVariant& obj)
{
  PII_D;
  const PiiMatrix<T> image(obj.valueAs<PiiMatrix<T> >());
  PiiMatrix<unsigned char> matBoundaryMask;
  PiiBoundaryFinder finder(image, &matBoundaryMask);
  PiiMatrix<int> matPoints(0,2);
  matPoints.reserve(256);
  PiiMatrix<int> matLimits(1,32);
  matLimits.resize(1,0);
  int iLimit = 0;

  for (;;)
    {
      int iPoints = finder.findNextBoundary(image, std::bind2nd(std::greater<T>(), T(d->dThreshold)), matPoints);
      if (iPoints == 0)
        break;
      if (iPoints < d->iMinLength || iPoints > d->iMaxLength)
        matPoints.resize(matPoints.rows() - iPoints, 2);
      else
        {
          iLimit += iPoints;
          matLimits.appendColumn(iLimit);
        }
    }

  d->pBoundariesOutput->emitObject(matPoints);
  d->pLimitsOutput->emitObject(matLimits);
  d->pMaskOutput->emitObject(matBoundaryMask);

  if (d->pBoundaryOutput->isConnected())
    {
      d->pBoundaryOutput->startMany();
      int iStart = 0;
      for (int i=0; i<matLimits.columns(); ++i)
        {
          int iEnd = matLimits(0,i);
          d->pBoundaryOutput->emitObject(Pii::matrix(matPoints(iStart,0,iEnd-iStart,-1)));
          iStart = iEnd;
        }
      d->pBoundaryOutput->endMany();
    }
}


void PiiBoundaryFinderOperation::setThreshold(double threshold) { _d()->dThreshold = threshold; }
double PiiBoundaryFinderOperation::threshold() const { return _d()->dThreshold; }
void PiiBoundaryFinderOperation::setMinLength(int minLength) { _d()->iMinLength = minLength; }
int PiiBoundaryFinderOperation::minLength() const { return _d()->iMinLength; }
void PiiBoundaryFinderOperation::setMaxLength(int maxLength) { _d()->iMaxLength = maxLength; }
int PiiBoundaryFinderOperation::maxLength() const { return _d()->iMaxLength; }
