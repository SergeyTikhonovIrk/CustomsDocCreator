#include "Window.h"

namespace wxWidget {
    bool MyApp::OnInit() {
        m_locale.Init(wxLANGUAGE_RUSSIAN);
        MyFrame *frame = new MyFrame();
        frame->Show(true);
        return true;
    }

    using namespace std::literals;

    MyFrame::MyFrame()
            : wxFrame(nullptr, wxID_ANY, wxString::FromUTF8Unchecked("Загрузчик таможенных документов"s)) {
        SetIcon(wxIcon(icon_xpm));
        Centre();
        wxMenu *menuFile = new wxMenu;
        menuFile->Append(ID_LoadRailwaybill, wxString::FromUTF8Unchecked("&Загрузка списка накладных...\tCtrl-H"s));
        menuFile->AppendSeparator();
        menuFile->Append(wxID_EXIT, wxString::FromUTF8Unchecked("&Выход"s));

        wxMenuBar *menuBar = new wxMenuBar;
        menuBar->Append(menuFile, wxString::FromUTF8Unchecked("&Документы"s));

        SetMenuBar(menuBar);
        SetSize(1500, 900);

        CreateStatusBar();

        Bind(wxEVT_MENU, &MyFrame::OnLoadRailwaybill, this, ID_LoadRailwaybill);
        Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

        panel_ = new wxPanel(this, -1);
        wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
        panel_->SetSizer(vbox);
        grid_ = new wxGrid(panel_, -1);
        grid_->CreateGrid(0, 5);
        grid_->SetColLabelValue(0, wxString::FromUTF8Unchecked("Номер накладной"s));
        grid_->SetColLabelValue(1, wxString::FromUTF8Unchecked("Получатель"s));
        grid_->SetColLabelValue(2, wxString::FromUTF8Unchecked("Станция назначения"s));
        grid_->SetColLabelValue(3, wxString::FromUTF8Unchecked("Вес"s));
        grid_->SetColLabelValue(4, wxString::FromUTF8Unchecked("Кол-во цистерн"s));
        grid_->SetColSize(0, 200);
        grid_->SetColSize(1, 400);
        grid_->SetColSize(2, 200);
        grid_->SetColSize(3, 200);
        grid_->SetColSize(4, 200);
        vbox->Add(grid_, 1, wxEXPAND);
        Center();

    }

    void MyFrame::OnExit(wxCommandEvent& event) {
        Close(true);
    }

    void MyFrame::OnLoadRailwaybill(wxCommandEvent& event) {
        domain::Placeholder placeholder;
        wxString wildcard = wxT("Excel files (*.xlsx)| *.xlsx"s);
        if (wxFileDialog *openFileDialog = new wxFileDialog(this, wxString::FromUTF8Unchecked(
                                                                    "Откройте файл со списком накладных"s),
                                                            wxEmptyString, wxEmptyString, wildcard);
                openFileDialog->ShowModal() == wxID_OK) {
            auto result = placeholder.LoadingList(openFileDialog->GetPath().utf8_string().c_str());
            if (!result.success_) {
                wxLogMessage(result.error_description_.c_str());
            }
        }
        int row = 0;
        int weight = 0;
        size_t count = 0;
        for (const auto& [consegnee, bills]: placeholder.GetListOfRailwaybill()) {
            for (const auto& bill: bills) {
                grid_->AppendRows(1);
                grid_->SetReadOnly(row, 0);
                grid_->SetCellValue(row, 0, wxString::FromUTF8Unchecked(bill.first));
                grid_->SetCellValue(row, 1, wxString::FromUTF8Unchecked(consegnee.name_));
                grid_->SetCellValue(row, 2, wxString::FromUTF8Unchecked(consegnee.station_));
                grid_->SetCellValue(row, 3, wxString::FromUTF8Unchecked(std::to_string(bill.second.weigth_)));
                grid_->SetCellValue(row, 4,
                                    wxString::FromUTF8Unchecked(std::to_string(bill.second.tanks_.size())));
                grid_->SetReadOnly(row, 0);
                grid_->SetReadOnly(row, 1);
                grid_->SetReadOnly(row, 2);
                grid_->SetReadOnly(row, 3);
                grid_->SetReadOnly(row, 4);
                weight += bill.second.weigth_;
                count += bill.second.tanks_.size();
                ++row;
            }
        }
        grid_->AppendRows(1);
        grid_->SetCellTextColour(row, 2, *wxGREEN);
        grid_->SetCellValue(row, 2, wxString::FromUTF8Unchecked("Итого"s));
        grid_->SetCellValue(row, 3, wxString::FromUTF8Unchecked(std::to_string(weight)));
        grid_->SetCellValue(row, 4, wxString::FromUTF8Unchecked(std::to_string(count)));
        grid_->SetCellBackgroundColour(row, 3, wxColor(200, 80, 45));
        grid_->SetCellBackgroundColour(row, 4, wxColor(200, 80, 45));

        std::string path;
        if (wxDirDialog *dir = new wxDirDialog(this, wxString::FromUTF8Unchecked("Путь сохранения накладных"s), "/"s,
                                               wxDD_NEW_DIR_BUTTON); dir->ShowModal() == wxID_OK) {
            path = dir->GetPath().utf8_string();
        }

        wildcard = wxT("XML files (*.xml)| *.xml"s);
        for (const auto& railbill: placeholder.GetListOfRailwaybill()) {
            if (wxFileDialog *openFileDialog = new wxFileDialog(this,
                                                                wxString::FromUTF8Unchecked(
                                                                        "Открыть файл шаблона: "s +
                                                                        railbill.first.name_ + " " +
                                                                        railbill.first.station_),
                                                                wxEmptyString, wxEmptyString, wildcard);
                    openFileDialog->ShowModal() == wxID_OK) {
                placeholder.GenerateRailwayBills(railbill.second, openFileDialog->GetPath().utf8_string() ,path);
            }
        }
    }
}
