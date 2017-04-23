// BeginLicense:
// Copyright (C) 2016-2017 Carsten Arnholm
//
// This file may be used under the terms of either the GNU General
// Public License version 2 or 3 (at your option) as published by the
// Free Software Foundation.
//
// This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
// INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE.
// EndLicense:

#ifndef portaclipMAIN_H
#define portaclipMAIN_H

#include <wx/socket.h>
#include <wx/clipbrd.h>
#include <wx/config.h>

#include <vector>
using namespace std;

//(*Headers(portaclipDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/timer.h>
//*)

class portaclipDialog: public wxDialog {
public:

   portaclipDialog(wxWindow* parent,wxWindowID id = -1);
   virtual ~portaclipDialog();

private:

   //(*Handlers(portaclipDialog)
   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);
   void OnTimer1Trigger(wxTimerEvent& event);
   void OnUpdateClick(wxCommandEvent& event);
   void OnForceCopyButtonClick(wxCommandEvent& event);
   //*)

   //(*Identifiers(portaclipDialog)
   static const long ID_STATICTEXT1;
   static const long ID_STATICTEXT2;
   static const long ID_TEXTCTRL1;
   static const long ID_BUTTON3;
   static const long ID_CHECKBOX1;
   static const long ID_BUTTON1;
   static const long ID_TIMER1;
   //*)

   //(*Declarations(portaclipDialog)
   wxTextCtrl* m_sendtoIP;
   wxStaticText* StaticText2;
   wxCheckBox* m_auto_copy;
   wxStaticText* StaticText1;
   wxButton* Button3;
   wxBoxSizer* BoxSizer1;
   wxButton* ForceCopyButton;
   wxTimer Timer1;
   //*)

   void OnUDPSocket(wxSocketEvent& event);
   static const long UDP_SOCKET_ID;

   DECLARE_EVENT_TABLE()

   // fetch the current clipboard and store in m_clipboard_backup
   void backup_clipboard();

   // transmit the clipboard backup to the other machine
   void transmit_clipboard_backup();


private:
   wxPoint           m_center;
   wxPoint           m_mouse_prev;
   bool              m_inside;

   wxTextDataObject  m_clipboard_backup; // this is our clipboard backup

   wxIPV4address     m_addr;
   wxIPV4address     m_sendto_addr;  // For sending
   wxDatagramSocket* m_udp;
   wxConfig*         m_config;
};

#endif // portaclipMAIN_H
