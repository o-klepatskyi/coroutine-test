// src: https://stackoverflow.com/a/70968385
#pragma once
#include "gtest/gtest.h"
#include <crtdbg.h>

// Class for detecting memory leaks during testing
class MemoryLeakDetector 
{
    _CrtMemState _memState;
public:
    MemoryLeakDetector()
    {
        _CrtMemCheckpoint(&_memState);
    }

    ~MemoryLeakDetector()
    {
        _CrtMemState stateNow, stateDiff;
        _CrtMemCheckpoint(&stateNow);
        int diffResult = _CrtMemDifference(&stateDiff, &_memState, &stateNow);
        if (diffResult)
            reportFailure(stateDiff.lSizes[1]);
    }
private:
    static void reportFailure(size_t unfreedBytes)
    {
        FAIL() << "Memory leak of " << unfreedBytes << " byte(s) detected.";
    }
};