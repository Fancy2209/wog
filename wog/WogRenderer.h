#pragma once

#include "BoyLib/UString.h"

class WogRenderer
{
    public:
        WogRenderer();
        virtual ~WogRenderer();

    private:
        Boy::UString *mUnkUString = new Boy::UString();
        int mUnknown1;
        int mUnknown2;
        //LoadingScreenRenderer mLoadingScreenRenderer;
        int mUnknown3;

};