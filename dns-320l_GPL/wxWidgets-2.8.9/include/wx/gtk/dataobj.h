///////////////////////////////////////////////////////////////////////////////
// Name:        gtk/dataobj.h
// Purpose:     declaration of the wxDataObject
// Author:      Robert Roebling
// RCS-ID:      $Id: dataobj.h,v 1.1.1.1 2009/10/09 02:56:56 jack Exp $
// Copyright:   (c) 1998, 1999 Vadim Zeitlin, Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_DATAOBJ_H_
#define _WX_GTK_DATAOBJ_H_

// ----------------------------------------------------------------------------
// wxDataObject is the same as wxDataObjectBase under wxGTK
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDataObject : public wxDataObjectBase
{
public:
    wxDataObject();
    virtual ~wxDataObject();

    virtual bool IsSupportedFormat( const wxDataFormat& format, Direction dir = Get ) const;
};

#endif // _WX_GTK_DATAOBJ_H_

