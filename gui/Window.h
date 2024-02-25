#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/treelist.h>
#include <wx/dirdlg.h>

#include "../domain/Placeholder.h"
#include "../source/icon.xpm"

namespace wxWidget {

    class MyApp : public wxApp {
    public:
        bool OnInit() override;

    protected:
        wxLocale m_locale;
    };

    class MyFrame : public wxFrame {
    public:
        MyFrame();

    private:
        void OnLoadRailwaybill(wxCommandEvent& event);

        void OnExit(wxCommandEvent& event);

        wxPanel *panel_;

        wxGrid *grid_;
    };

    enum {
        ID_LoadRailwaybill = 1
    };
}

