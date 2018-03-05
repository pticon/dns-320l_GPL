/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.cpp
// Purpose:     wxMiniFrame. Optional; identical to wxFrame if not supported.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id: minifram.cpp,v 1.1.1.1 2009/10/09 02:59:36 jack Exp $
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_MINIFRAME

#include "wx/minifram.h"

IMPLEMENT_DYNAMIC_CLASS(wxMiniFrame, wxFrame)

#endif // wxUSE_MINIFRAME
