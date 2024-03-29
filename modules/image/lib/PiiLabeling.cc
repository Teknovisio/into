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

#include "PiiLabeling.h"
#include <QStack>

namespace PiiImage
{
  // Mark a sequence of detected object pixels into the label buffer
  void markToBuffer(LabelInfo& info, int rowIndex, int start, int end)
  {
    // Mark the run into the label buffer
    int* runRow = info.matLabels[rowIndex];
    for (int c = start; c < end + info.iConnectivityShift; ++c)
      runRow[c] = info.iLabelIndex;
  }

  // On row rowIndex, find all runs that overlap with the range
  // start-end.
  void connectRunsRecursively(LabelInfo& info, int rowIndex, int start, int end)
  {
    // Out of image boundaries...
    if (rowIndex < 0 || rowIndex >= info.lstRuns.size())
      return;

    // Go through all runs on this row and find the overlapping ones
    for (RunNode* pNode = info.lstRuns[rowIndex].first; pNode != 0;)
      {
        // No overlap
        if (start > pNode->end ||
            end < pNode->start)
          {
            pNode = pNode->next;
            continue;
          }

        // Invalidate the current run to prevent loops in recursion
        int end = pNode->end;
        pNode->end = -1;
        pNode->seed = false;
        // Mark and recurse
        markToBuffer(info, rowIndex, pNode->start, end);
        connectRunsRecursively(info, rowIndex - 1, pNode->start, end);
        connectRunsRecursively(info, rowIndex + 1, pNode->start, end);
        // Destroy the node
        info.lstRuns[rowIndex].remove(pNode);
        RunNode* pNodeToDelete = pNode;
        pNode = pNode->next;
        delete pNodeToDelete;
      }
  }

  struct RecursiveCall
  {
    int rowIndex;
    int start;
    int end;
  };

  // On row rowIndex, find all runs that overlap with the range
  // start-end.
  void connectRuns(LabelInfo& info, int rowIndex, int start, int end)
  {
    QStack<RecursiveCall>  localStack;
    RecursiveCall currentCall = { rowIndex, start, end };
    localStack.push(currentCall);

    while (!localStack.isEmpty())
      {
        currentCall = localStack.pop();
        // Out of image boundaries...
        if (currentCall.rowIndex < 0 || currentCall.rowIndex >= info.lstRuns.size())
          continue; // to next item in the local stack

        // Go through all runs on this row and find the overlapping ones
        for (RunNode* pNode = info.lstRuns[currentCall.rowIndex].first; pNode != 0;)
          {
            // No overlap
            if (currentCall.start > pNode->end ||
            currentCall.end < pNode->start)
              {
                pNode = pNode->next;
                continue; // to the next node
              }

            // Invalidate the current run to prevent loops in "recursion"
            int end = pNode->end;
            pNode->end = -1;
            pNode->seed = false;
            // Mark the run and push "recursion" calls to the stack
            markToBuffer(info, currentCall.rowIndex, pNode->start, end);
            RecursiveCall call;
            call.rowIndex = currentCall.rowIndex - 1;
            call.start = pNode->start;
            call.end = end;
            localStack.push(call);
            call.rowIndex = currentCall.rowIndex + 1;
            localStack.push(call);

            // Destroy current node
            info.lstRuns[currentCall.rowIndex].remove(pNode);
            RunNode* pNodeToDelete = pNode;
            pNode = pNode->next;
            delete pNodeToDelete;
          }
      }
  }
}
