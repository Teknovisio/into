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

#include "TestPiiOperationCompound.h"

#include <PiiGenericTextOutputArchive.h>
#include <PiiGenericTextInputArchive.h>

#include <QtTest>

#include "TestOperation.h"

void TestPiiOperationCompound::initTestCase()
{
  _compound.createInputProxy("input");
  _compound.createOutputProxy("output");
  _compound.inputProxy("input")->output()->connectInput(_compound.outputProxy("output")->input());

  setOperation(&_compound);
}

void TestPiiOperationCompound::serialize()
{
  QByteArray array;
  QBuffer buffer(&array);

  try
    {
      buffer.open(QIODevice::ReadWrite);
      PiiGenericTextOutputArchive oa(&buffer);
      oa << &_compound;
    }
  catch (PiiSerializationException& ex)
    {
      QFAIL(("Serialization error: " + ex.message() + ". Additional info: " + ex.info()).toUtf8().constData());
    }

  //qDebug() << array;

  try
    {
      buffer.seek(0);
      PiiGenericTextInputArchive ia(&buffer);
      PiiOperationCompound* pCompound;
      ia >> pCompound;
      QCOMPARE(pCompound->outputProxy("output")->input()->connectedOutput(),
               pCompound->inputProxy("input")->output());
      delete pCompound;
    }
  catch (PiiSerializationException& ex)
    {
      QFAIL(("Serialization error: " + ex.message() + ". Additional info: " + ex.info()).toUtf8().constData());
    }
}

void TestPiiOperationCompound::clone()
{
  PiiOperationCompound* pCompound = _compound.clone();
  QVERIFY(pCompound->input("input") != 0);
  QVERIFY(pCompound->output("output") != 0);
  QCOMPARE(pCompound->outputProxy("output")->input()->connectedOutput(),
           pCompound->inputProxy("input")->output());
  delete pCompound;
}

void TestPiiOperationCompound::socketData()
{
  PiiOperationCompound* pCompound = _compound.clone();
  TestOperation* pTest = new TestOperation;
  pTest->connectOutput("output", pCompound, "input");
  QCOMPARE(pTest->socketData(pTest->output("output"), 1).toInt(), 1);
  QCOMPARE(pCompound->socketData(pCompound->output("output"), 1).toInt(), 1);
  delete pCompound;
  delete pTest;
}

void TestPiiOperationCompound::cleanupTestCase()
{
  setOperation(0);
}

QTEST_MAIN(TestPiiOperationCompound)
