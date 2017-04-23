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

#include "portaclipMain.h"
#include <wx/msgdlg.h>
#include <wx/settings.h>

#include <string>
using namespace std;

#include "images/clipboard_16x16.xpm"
#include "images/clipboard_32x32.xpm"
#include "images/clipboard_64x64.xpm"

//(*InternalHeaders(portaclipDialog)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

const long portaclipDialog::UDP_SOCKET_ID = wxNewId();

//(*IdInit(portaclipDialog)
const long portaclipDialog::ID_STATICTEXT1 = wxNewId();
const long portaclipDialog::ID_STATICTEXT2 = wxNewId();
const long portaclipDialog::ID_TEXTCTRL1 = wxNewId();
const long portaclipDialog::ID_BUTTON3 = wxNewId();
const long portaclipDialog::ID_CHECKBOX1 = wxNewId();
const long portaclipDialog::ID_BUTTON1 = wxNewId();
const long portaclipDialog::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(portaclipDialog,wxDialog)
    //(*EventTable(portaclipDialog)
    //*)
   EVT_SOCKET(UDP_SOCKET_ID, portaclipDialog::OnUDPSocket)
END_EVENT_TABLE()

portaclipDialog::portaclipDialog(wxWindow* parent,wxWindowID id)
: m_udp(0)
{
   //(*Initialize(portaclipDialog)
   wxBoxSizer* BoxSizer2;
   wxBoxSizer* BoxSizer3;

   Create(parent, id, _("Portaclip"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX, _T("id"));
   BoxSizer1 = new wxBoxSizer(wxVERTICAL);
   StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Portaclip"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
   wxFont StaticText1Font(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_T("Noto Sans"),wxFONTENCODING_DEFAULT);
   StaticText1->SetFont(StaticText1Font);
   BoxSizer1->Add(StaticText1, 1, wxALL|wxEXPAND, 10);
   BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
   StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("SendTo IP"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
   BoxSizer3->Add(StaticText2, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   m_sendtoIP = new wxTextCtrl(this, ID_TEXTCTRL1, _("255.255.255.0"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
   BoxSizer3->Add(m_sendtoIP, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_TOP, 5);
   Button3 = new wxButton(this, ID_BUTTON3, _("Update IP"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
   BoxSizer3->Add(Button3, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   BoxSizer1->Add(BoxSizer3, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
   BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
   m_auto_copy = new wxCheckBox(this, ID_CHECKBOX1, _("Auto copy (mouse)"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
   m_auto_copy->SetValue(true);
   BoxSizer2->Add(m_auto_copy, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   ForceCopyButton = new wxButton(this, ID_BUTTON1, _("Force Copy"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
   BoxSizer2->Add(ForceCopyButton, 0, wxLEFT|wxRIGHT|wxALIGN_TOP, 5);
   BoxSizer1->Add(BoxSizer2, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
   SetSizer(BoxSizer1);
   Timer1.SetOwner(this, ID_TIMER1);
   BoxSizer1->Fit(this);
   BoxSizer1->SetSizeHints(this);

   Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&portaclipDialog::OnUpdateClick);
   Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&portaclipDialog::OnForceCopyButtonClick);
   Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&portaclipDialog::OnTimer1Trigger);
   //*)

   m_config = new wxConfig(wxT("portaclip"),wxT("Arnholm"));

   wxIconBundle icons;
   icons.AddIcon(wxIcon(clipboard_16x16_xpm));
   icons.AddIcon(wxIcon(clipboard_32x32_xpm));
   icons.AddIcon(wxIcon(clipboard_64x64_xpm));
   SetIcons(icons);

   // we run a timer to take backups of the clipboard & transmit it when required
   Timer1.Start(200);
   m_mouse_prev =  wxGetMousePosition();
   m_inside = true;
   m_center = wxPoint(wxSystemSettings::GetMetric(wxSYS_SCREEN_X)/2,wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)/2);

   // address for this computer (receiving end)
   m_addr.AnyAddress();
   m_addr.Service(7552);

   // set up UDP socket for communication
   // Also define an eventhandler for incoming data
   m_udp = new wxDatagramSocket(m_addr, wxSOCKET_REUSEADDR);
   m_udp->SetEventHandler(*this, UDP_SOCKET_ID);
   m_udp->SetNotify(wxSOCKET_INPUT_FLAG);
   m_udp->Notify(true);

   // get IP address to send to from configuration
   // if not configured, the user types the address and pushes the "Update" button
   wxString ip_address="255.255.255.0";
   m_config->Read("SendTo",&ip_address);
   if(ip_address.length() > 0) {
      m_sendto_addr.Hostname(ip_address);
   }

   bool autocopy = m_auto_copy->IsChecked();
   m_config->Read("AutoCopy",&autocopy);
   m_auto_copy->SetValue(autocopy);

   // display the IP address in the text control
   m_sendto_addr.Service(7552);
   m_sendtoIP->SetValue(m_sendto_addr.IPAddress());

   // take initial backup of clipboard
   backup_clipboard();
}

portaclipDialog::~portaclipDialog()
{
   // save configuration
   m_config->Write("SendTo",m_sendto_addr.IPAddress());
   m_config->Write("AutoCopy",m_auto_copy->IsChecked());
   delete m_config;
   delete m_udp;

    //(*Destroy(portaclipDialog)
    //*)
}

void portaclipDialog::OnQuit(wxCommandEvent& event)
{
    Close();
}

void portaclipDialog::OnAbout(wxCommandEvent& event)
{
   wxString msg = wxbuildinfo(long_f);
   wxMessageBox(msg, _("Welcome to..."));
}

void portaclipDialog::OnTimer1Trigger(wxTimerEvent& event)
{
   if(m_auto_copy->IsChecked()) {

      wxPoint mouse = wxGetMousePosition();

      // magic code to determine whether the mouse cursor is outside the current screen
      // this assumes that Synergy is actually running (with a broken clipboard handler)
      int dx  = mouse.x - m_mouse_prev.x;
      int dy  = mouse.y - m_mouse_prev.y;
      int dx2 = mouse.x - m_center.x;
      int dy2 = mouse.y - m_center.y;

      if( abs(dx)<60 && abs(dy)<60 && abs(dx2)<60 && abs(dy2)<60 ) {

         // we are outside the local screen

         if(m_inside) {
            // we just moved outside the screen, send the clipboard backup copy
            // (we cannot read the real clipboard here, because it looks like Synergy alread cleared it)
            StaticText1->SetLabel("Outside");
            transmit_clipboard_backup();
         }
         m_inside = false;
      }
      else {

         // we are inside the local screen
         if(!m_inside) {

            // we just moved into the local screen
            StaticText1->SetLabel("Inside");
         }

         // capture the clipboard data
         backup_clipboard();
         m_mouse_prev = mouse;
         m_inside = true;
      }
   }

}


void portaclipDialog::OnUDPSocket(wxSocketEvent& event)
{
   if(event.GetSocketEvent() == wxSOCKET_INPUT)  {

      // UDP socked event received. The UDP message will contain a copy of
      // the other computer clipboard text in UTF8 format

      // buffer to receive the data
      vector<char> buf(1024*1024);

      // we don't care which IP address the data came from
      wxIPV4address addr;
      addr.AnyAddress();
      m_udp->RecvFrom(addr, &buf[0], buf.size());
      size_t nbytes = m_udp->LastCount();
      if(m_udp->Error()) {
         StaticText1->SetLabel("OnUDPSocket- error");
         return;
      }
      StaticText1->SetLabel(wxString::Format("Received Clipboard: %d bytes",(int)nbytes));

      if(nbytes > 0) {

         // Interpret the buffer data as UTF8
         buf[nbytes]=0;
         wxString text  = wxString::FromUTF8(&buf[0],nbytes);

         // Write the received text to the local clipboard
         if (wxTheClipboard->Open())
         {
             // The data objects are held by the clipboard,
             // so do not delete them in the app.
             m_clipboard_backup.SetText(text);
             wxTheClipboard->SetData( new wxTextDataObject(text) );
             wxTheClipboard->Close();
         }
      }
   }
}

void portaclipDialog::OnUpdateClick(wxCommandEvent& event)
{
   m_sendto_addr.Hostname(m_sendtoIP->GetValue());
   m_sendto_addr.Service(7552);
}


void portaclipDialog::backup_clipboard()
{
   // Read from local system clipboard and store in our clipboard backup object
   if(wxTheClipboard->Open()) {
      if(wxTheClipboard->IsSupported( wxDF_TEXT )) {
         wxTheClipboard->GetData( m_clipboard_backup );
      }
      wxTheClipboard->Close();
   }
}

void portaclipDialog::OnForceCopyButtonClick(wxCommandEvent& event)
{
   backup_clipboard();
   transmit_clipboard_backup();
}


void portaclipDialog::transmit_clipboard_backup()
{
   wxString msg = m_clipboard_backup.GetText();
   if(msg.Length() > 0) {
      // send the clipboard text as UTF8
      wxScopedCharBuffer buffer = msg.ToUTF8();
      m_udp->SendTo(m_sendto_addr,buffer.data(),buffer.length());
      StaticText1->SetLabel(wxString::Format("Sent Clipboard: %d bytes",(int) buffer.length() ));
   }
   else {
       StaticText1->SetLabel("Clipboard text was empty");
   }
}
