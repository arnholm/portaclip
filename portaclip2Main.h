/***************************************************************
 * Name:      portaclip2Main.h
 * Purpose:   Defines Application Frame
 * Author:    Carsten Arnholm ()
 * Created:   2013-08-31
 * Copyright: Carsten Arnholm ()
 * License:
 **************************************************************/

#ifndef portaclip2MAIN_H
#define portaclip2MAIN_H

//(*Headers(portaclip2Dialog)
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
//*)


#include <wx/socket.h>
#include <wx/clipbrd.h>
#include <wx/config.h>

class portaclip2Dialog: public wxDialog {
public:

   portaclip2Dialog(wxWindow* parent,wxWindowID id = -1);
   virtual ~portaclip2Dialog();

private:

   //(*Handlers(portaclip2Dialog)
   void OnQuit(wxCommandEvent& event);
   void OTimerTrigger(wxTimerEvent& event);
   void OnSaveButtonClick(wxCommandEvent& event);
   //*)

   //(*Identifiers(portaclip2Dialog)
   static const long ID_PANEL1;
   static const long ID_STATICTEXT1;
   static const long ID_TEXTCTRL1;
   static const long ID_BUTTON3;
   static const long ID_STATICTEXT2;
   static const long ID_TIMER1;
   //*)

   //(*Declarations(portaclip2Dialog)
   wxBoxSizer* BoxSizer1;
   wxButton* SaveButton;
   wxPanel* m_led;
   wxStaticText* m_message;
   wxStaticText* m_prompt;
   wxTextCtrl* m_ip_address;
   wxTimer m_timer;
   //*)

   void OnUDPSocket(wxSocketEvent& event);
   static const long UDP_SOCKET_ID;

   // fetch the current clipboard and store in m_clipboard_backup
   void backup_clipboard(bool force=false);

   // transmit the clipboard backup to the other machine
   void transmit_clipboard_backup();

   DECLARE_EVENT_TABLE()

private:
   wxSize  m_screen;          // local screen size
   wxPoint m_screen_center;   // local screen center
   wxPoint m_mouse_prev;      // previous mouse position
   bool    m_inside;          // true when mouse is within local screen

   wxTextDataObject  m_clipboard_backup; // this is our clipboard backup
   wxDateTime        m_clipboard_time;   // time of clipboard backup

   wxIPV4address     m_addr;
   wxIPV4address     m_sendto_addr;  // IP address to other computer
   wxDatagramSocket* m_udp;
   wxConfig*         m_config;
};

#endif // portaclip2MAIN_H
