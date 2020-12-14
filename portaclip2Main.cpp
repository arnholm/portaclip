/***************************************************************
 * Name:      portaclip2Main.cpp
 * Purpose:   Code for Application Frame
 * Author:    Carsten Arnholm ()
 * Created:   2013-08-31
 * Copyright: Carsten Arnholm ()
 * License:
 **************************************************************/

#include "portaclip2Main.h"
#include <wx/msgdlg.h>
#include <wx/settings.h>

#include <string>
#include <math.h>
#include <sstream>
#include <vector>

#include "images/portaclip16X16.xpm"
#include "images/portaclip32X32.xpm"
#include "images/portaclip48X48.xpm"
#include "images/portaclip64X64.xpm"

//(*InternalHeaders(portaclip2Dialog)
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

static wxColor wxDarkGreen(0,164,0);
const long portaclip2Dialog::UDP_SOCKET_ID = wxNewId();

//(*IdInit(portaclip2Dialog)
const long portaclip2Dialog::ID_PANEL1 = wxNewId();
const long portaclip2Dialog::ID_STATICTEXT1 = wxNewId();
const long portaclip2Dialog::ID_TEXTCTRL1 = wxNewId();
const long portaclip2Dialog::ID_BUTTON3 = wxNewId();
const long portaclip2Dialog::ID_STATICTEXT2 = wxNewId();
const long portaclip2Dialog::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(portaclip2Dialog,wxDialog)
    //(*EventTable(portaclip2Dialog)
    //*)
   EVT_SOCKET(UDP_SOCKET_ID, portaclip2Dialog::OnUDPSocket)
END_EVENT_TABLE()


portaclip2Dialog::portaclip2Dialog(wxWindow* parent,wxWindowID id)
{
   //(*Initialize(portaclip2Dialog)
   wxBoxSizer* BoxSizer2;
   wxBoxSizer* BoxSizer3;

   Create(parent, id, _("Portaclip2"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX, _T("id"));
   BoxSizer1 = new wxBoxSizer(wxVERTICAL);
   BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
   m_led = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(25,-1), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
   BoxSizer3->Add(m_led, 0, wxALL|wxEXPAND, 5);
   m_prompt = new wxStaticText(this, ID_STATICTEXT1, _("Send to IP:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
   BoxSizer3->Add(m_prompt, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   m_ip_address = new wxTextCtrl(this, ID_TEXTCTRL1, _("10.0.0.111"), wxDefaultPosition, wxSize(100,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
   BoxSizer3->Add(m_ip_address, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   SaveButton = new wxButton(this, ID_BUTTON3, _("Save"), wxDefaultPosition, wxSize(60,-1), 0, wxDefaultValidator, _T("ID_BUTTON3"));
   BoxSizer3->Add(SaveButton, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   BoxSizer1->Add(BoxSizer3, 1, wxLEFT|wxRIGHT|wxEXPAND, 5);
   BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
   m_message = new wxStaticText(this, ID_STATICTEXT2, _("..."), wxDefaultPosition, wxSize(300,-1), 0, _T("ID_STATICTEXT2"));
   BoxSizer2->Add(m_message, 9, wxALL|wxEXPAND, 5);
   BoxSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND, 5);
   SetSizer(BoxSizer1);
   m_timer.SetOwner(this, ID_TIMER1);
   BoxSizer1->Fit(this);
   BoxSizer1->SetSizeHints(this);

   Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&portaclip2Dialog::OnSaveButtonClick);
   Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&portaclip2Dialog::OTimerTrigger);
   //*)

   m_config = new wxConfig(wxT("portaclip"),wxT("Arnholm"));

   wxIconBundle icons;
   icons.AddIcon(wxIcon(portaclip16X16_xpm));
   icons.AddIcon(wxIcon(portaclip32X32_xpm));
   icons.AddIcon(wxIcon(portaclip48X48_xpm));
   icons.AddIcon(wxIcon(portaclip64X64_xpm));
   SetIcons(icons);

   m_inside = true;
   m_mouse_prev = wxGetMousePosition();
   m_screen = wxSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X),wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
   m_screen_center = wxPoint(m_screen.x/2,m_screen.y/2);

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
   // if not configured, the user types the address and pushes the "Save" button
   wxString ip_address="255.255.255.0";
   m_config->Read("SendTo",&ip_address);
   if(ip_address.length() > 0) {
      m_sendto_addr.Hostname(ip_address);
   }

   // display the IP address in the text control
   m_sendto_addr.Service(7552);
   m_ip_address->SetValue(m_sendto_addr.IPAddress());

   m_led->SetBackgroundColour(wxDarkGreen);

   backup_clipboard(true);

   // we need a high frequency timer in order to catch rapid mouse movements
   int millisec = 20;
   m_timer.Start(millisec);
}

portaclip2Dialog::~portaclip2Dialog()
{
   // save configuration
   m_config->Write("SendTo",m_sendto_addr.IPAddress());
   delete m_config;
   delete m_udp;
}

void portaclip2Dialog::OnQuit(wxCommandEvent& event)
{
    Close();
}

void portaclip2Dialog::OTimerTrigger(wxTimerEvent& event)
{
   wxPoint mouse = wxGetMousePosition();

   int dxcen = mouse.x - m_screen_center.x;
   int dycen = mouse.y - m_screen_center.y;
   int dx    = mouse.x - m_mouse_prev.x;
   int dy    = mouse.y - m_mouse_prev.y;

   int dcen = sqrt(dxcen*dxcen + dycen*dycen);

   // Here, we try to detect the "event" that the mouse moves
   // from one screen to the other. We change state only when
   // such an event is detected

   if((abs(dx)> m_screen.x/3)) {
      // A sudden jump in X implies moving from one screen to another

      bool inside_before = m_inside;

      // if at the same time we seem to be near the screen center
      // (which kind of impossible), we take it as a sign we just jumped
      // to the remote screen. Otherwise we must be on the local screen
      if(dcen<500) {
         m_inside = false;  // moving from local to remote screen

         // send clipboard
         transmit_clipboard_backup();
      }
      else  {
         m_inside = true;   // moving from remote to local screen
      }

      #if defined(__WXMSW__)
         m_inside = (dcen!=0);
      #endif

      if(m_inside != inside_before) {
         // change "led" color to indicate the new state
         m_led->SetBackgroundColour((m_inside)?  wxDarkGreen : *wxBLUE );
         m_led->Refresh();
      }
   }

   // take a backup copy of the clipboard, at somewhat more relaxed intervals
   wxTimeSpan time_since_backup = wxDateTime::Now().Subtract(m_clipboard_time);
   if(time_since_backup.GetMilliseconds() > 250) {
      backup_clipboard();
   }
   m_mouse_prev = mouse;

}



void portaclip2Dialog::OnUDPSocket(wxSocketEvent& event)
{
   if(event.GetSocketEvent() == wxSOCKET_INPUT)  {

      // UDP socked event received. The UDP message will contain a copy of
      // the other computer clipboard text in UTF8 format

      // buffer to receive the data
      std::vector<char> buf(1024*1024);

      // we don't care which IP address the data came from
      wxIPV4address addr;
      addr.AnyAddress();
      m_udp->RecvFrom(addr, &buf[0], buf.size());
      size_t nbytes = m_udp->LastCount();
      if(m_udp->Error()) {
         m_message->SetLabel("OnUDPSocket- error");
         return;
      }

      if(nbytes > 0) {

         // Interpret the buffer data as UTF8
         buf[nbytes]=0;
         wxString text  = wxString::FromUTF8(&buf[0],nbytes);

         // Write the received text to the local clipboard
         if (wxTheClipboard->Open())
         {
             m_clipboard_backup.SetText(text);
             m_clipboard_time = wxDateTime::Now();
             wxTheClipboard->SetData( new wxTextDataObject(text) );
             wxTheClipboard->Close();
             m_message->SetLabel(wxString::Format("Received/Saved clipboard %d bytes", (int)text.length()) + m_clipboard_time.Format(" %H:%M:%S"));
         }
      }
   }
}

void portaclip2Dialog::backup_clipboard(bool force)
{
   if(force) {
      m_clipboard_time = wxDateTime::Now();
   }

   try {

   // Read from local system clipboard and store in our clipboard backup object
      if(wxTheClipboard->Open()) {
         if(wxTheClipboard->IsSupported( wxDF_TEXT )) {
            wxTheClipboard->GetData( m_clipboard_backup );
            m_clipboard_time = wxDateTime::Now();
            wxString txt = m_clipboard_backup.GetText();
            m_message->SetLabel(wxString::Format("Saved clipboard backup %d bytes", (int)txt.length()) + m_clipboard_time.Format(" %H:%M:%S"));
         }
         wxTheClipboard->Close();
      }
   }
   catch(...) {
      m_message->SetLabel(wxString::Format(" wxTheClipboard->GetData() failed"));
   }
}

void portaclip2Dialog::transmit_clipboard_backup()
{
   wxString msg = m_clipboard_backup.GetText();
   if(msg.Length() > 0) {
      // send the clipboard text as UTF8
      wxScopedCharBuffer buffer = msg.ToUTF8();
      m_udp->SendTo(m_sendto_addr,buffer.data(),buffer.length());
      m_message->SetLabel(wxString::Format("Sent clipboard: %d bytes",(int) buffer.length() )+ m_clipboard_time.Format("%H:%M:%S"));
   }
   else {
      m_message->SetLabel("Clipboard backup was empty");
   }
}

void portaclip2Dialog::OnSaveButtonClick(wxCommandEvent& event)
{
   m_sendto_addr.Hostname(m_ip_address->GetValue());
   m_sendto_addr.Service(7552);
}
