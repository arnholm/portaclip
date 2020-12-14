/***************************************************************
 * Name:      portaclip2App.cpp
 * Purpose:   Code for Application Class
 * Author:    Carsten Arnholm ()
 * Created:   2013-08-31
 * Copyright: Carsten Arnholm ()
 * License:
 **************************************************************/

#include "portaclip2App.h"

//(*AppHeaders
#include "portaclip2Main.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(portaclip2App);

bool portaclip2App::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	portaclip2Dialog Dlg(0);
    	SetTopWindow(&Dlg);
    	Dlg.ShowModal();
    	wxsOK = false;
    }
    //*)
    return wxsOK;

}
