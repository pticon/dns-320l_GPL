/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/main.cpp
// Purpose:     Entry point
// Author:      David Elliott
// Modified by:
// Created:     2002/11/11
// RCS-ID:      $Id: main.cpp,v 1.1.1.1 2009/10/09 02:58:57 jack Exp $
// Copyright:   (c) 2002 David Elliott
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////
/* DFE: ^^^^^ Was that really necessary :-) */

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

WXDLLEXPORT int main(int argc, char* argv[])
{
    return wxEntry(argc, argv);
}
