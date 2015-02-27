///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __APPWINDOW_H__
#define __APPWINDOW_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

namespace dit
{
	///////////////////////////////////////////////////////////////////////////////
	/// Class AppWindow
	///////////////////////////////////////////////////////////////////////////////
	class AppWindow : public wxFrame 
	{
		private:
		
		protected:
			wxMenuBar* MenuBar;
			wxMenu* FileMenu;
			wxCheckBox* vertNormalsCheckbox;
			wxCheckBox* faceNormalsCheckbox;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnOpenFile( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnPropGridChanging( wxPropertyGridEvent& event ) { event.Skip(); }
			virtual void OnFlagSolidMesh( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnFlagCoordFrame( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnFlagVertNormals( wxCommandEvent& event ) { event.Skip(); }
			virtual void OnFlagFaceNormals( wxCommandEvent& event ) { event.Skip(); }
			
		
		public:
			wxPropertyGrid* propGrid;
			wxPGProperty* modelFile;
			wxPGProperty* triCount;
			wxPGProperty* vertCount;
			wxCheckBox* solidMeshCheckbox;
			wxCheckBox* coordFrameCheckbox;
			wxPanel* gameWindowPanel;
			
			AppWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("chartly - portfolio: Sample Skeleton Application"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1280,720 ), long style = wxDEFAULT_FRAME_STYLE|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL|wxWANTS_CHARS );
			
			~AppWindow();
		
	};
	
} // namespace dit

#endif //__APPWINDOW_H__
