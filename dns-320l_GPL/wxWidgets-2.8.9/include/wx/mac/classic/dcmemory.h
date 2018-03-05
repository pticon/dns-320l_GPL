/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id: dcmemory.h,v 1.1.1.1 2009/10/09 02:57:04 jack Exp $
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#include "wx/dcclient.h"

class WXDLLEXPORT wxMemoryDC: public wxPaintDC
{
  DECLARE_DYNAMIC_CLASS(wxMemoryDC)

  public:
    wxMemoryDC( const wxBitmap& bitmap = wxNullBitmap );
    wxMemoryDC( wxDC *dc ); // Create compatible DC
    virtual ~wxMemoryDC(void);
    virtual void SelectObject( const wxBitmap& bitmap );
    virtual void DoGetSize( int *width, int *height ) const;
        wxBitmap    GetSelectedObject() { return m_selected ; }
  private:
    wxBitmap  m_selected;
};

#endif
    // _WX_DCMEMORY_H_
