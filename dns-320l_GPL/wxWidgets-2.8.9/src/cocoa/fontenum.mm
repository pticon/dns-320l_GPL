/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/fontenum.mm
// Purpose:     wxFontEnumerator class for Cocoa
// Author:      David Elliott
// Modified by:
// Created:     2003/07/23
// RCS-ID:      $Id: fontenum.mm,v 1.1.1.1 2009/10/09 02:58:57 jack Exp $
// Copyright:   (c) David Elliott
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#endif

#include "wx/fontenum.h"

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    return false;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
    return false;
}

