///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "layer_infos_control.h"

///////////////////////////////////////////////////////////////////////////

layer_infos_control::layer_infos_control( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* m_box_sizer;
	m_box_sizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* m_flex_grid_sizer;
	m_flex_grid_sizer = new wxFlexGridSizer( 2, 1, 0, 0 );
	m_flex_grid_sizer->AddGrowableCol( 0 );
	m_flex_grid_sizer->AddGrowableRow( 0 );
	m_flex_grid_sizer->SetFlexibleDirection( wxBOTH );
	m_flex_grid_sizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_text_control = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTE_MULTILINE|wxTE_READONLY );
	m_flex_grid_sizer->Add( m_text_control, 0, wxALL|wxEXPAND, 5 );
	
	m_std_buttons_sizer = new wxStdDialogButtonSizer();
	m_std_buttons_sizerOK = new wxButton( this, wxID_OK );
	m_std_buttons_sizer->AddButton( m_std_buttons_sizerOK );
	m_std_buttons_sizer->Realize();
	m_flex_grid_sizer->Add( m_std_buttons_sizer, 1, wxALIGN_RIGHT, 5 );
	
	m_box_sizer->Add( m_flex_grid_sizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( m_box_sizer );
	this->Layout();
	
	// Connect Events
	m_std_buttons_sizerOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( layer_infos_control::OnOKButtonClick ), NULL, this );
}

layer_infos_control::~layer_infos_control()
{
	// Disconnect Events
	m_std_buttons_sizerOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( layer_infos_control::OnOKButtonClick ), NULL, this );
}
