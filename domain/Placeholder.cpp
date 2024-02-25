#include "Placeholder.h"

domain::Result domain::Placeholder::LoadingList(const std::string& filepath) {
    Result result;
    try {
        wb.load(filepath);
        XlDocHandler();
    } catch (std::exception& ex) {
        result.success_ = false;
        result.error_description_ = ex.what();
    }
    return result;
}

void domain::Placeholder::XlDocHandler() {
    using namespace std::literals;
    auto ws = wb.active_sheet();
    for (auto row: ws.rows(false)) {
        std::vector<std::string> shipment;
        for (auto cell: row) {
            if (cell.to_string().empty() || cell.to_string() == "Номер декларации"s) {
                break;
            }
            shipment.push_back(cell.to_string());
        }
        if (!shipment.empty()) {
            Consegnee consegnee{shipment[7], shipment[6]};
            std::string bill = shipment[1].c_str();
            std::string tank = shipment[2];
            if (size_t ch = shipment[3].find('.'); ch != 0) {
                shipment[3][ch] = ',';
            }
            int weight = std::round(std::stod(shipment[3]) * 1000);
            Date date = ConvertFromExcelDays(std::stoi(shipment[4]));
            std::string passport = shipment[8];
            list_of_railwaybill_[consegnee][bill].tanks_.push_back({tank, weight});
            list_of_railwaybill_[consegnee][bill].passports_.insert(passport);
            list_of_railwaybill_[consegnee][bill].date_ = date;
            list_of_railwaybill_[consegnee][bill].weigth_ += weight;
        }
    }
}

domain::Date domain::Placeholder::ConvertFromExcelDays(int days) {
    Date date;
    if (days == 60) {
        date.day_ = 29;
        date.month_ = 2;
        date.year_ = 1900;
        return date;
    } else if (days < 60) {
        days++;
    }
    int l = days + 68569 + 2415019;
    int n = int((4 * l) / 146097);
    l = l - int((146097 * n + 3) / 4);
    int i = int((4000 * (l + 1)) / 1461001);
    l = l - int((1461 * i) / 4) + 31;
    int j = int((80 * l) / 2447);
    date.day_ = l - int((2447 * j) / 80);
    l = int(j / 11);
    date.month_ = j + 2 - (12 * l);
    date.year_ = 100 * (n - 49) + i + l;
    return date;
}

const domain::ListRailbills& domain::Placeholder::GetListOfRailwaybill() const {
    return list_of_railwaybill_;
}


void domain::Placeholder::GenerateRailwayBills(const Railbills& lists, const std::string& path_of_template,
                                               const std::string& dir) {
    using namespace std::literals;
    for (const auto& [bill, info]: lists) {
        tinyxml2::XMLDocument doc;
        auto result = doc.LoadFile(path_of_template.c_str());
        auto root = doc.RootElement();
        auto number = root->FirstChildElement("Number");
        if(number != NULL) {
            number->SetText(bill.c_str());
        } else {
            root->InsertNewChildElement("Number")->InsertNewText(bill.c_str());
        }
        number = root->FirstChildElement("RegistrationDocument")->FirstChildElement("cat_ru:PrDocumentNumber");
        number->SetText(bill.c_str());
        if (number = root->FirstChildElement("RegistrationDocument")->FirstChildElement("cat_ru:PrDocumentDate"); number != NULL) {
            number->SetText(info.date_.ToString().c_str());
        } else {
            number = root->FirstChildElement("RegistrationDocument");
            number->InsertNewChildElement("cat_ru:PrDocumentDate")->InsertNewText(info.date_.ToString().c_str());
        }
        auto weight = root->FirstChildElement("GrossWeightQuantity");
        weight->SetText(info.weigth_);
        weight = root->FirstChildElement("RWBGoods")->FirstChildElement("GrossWeightQuantity");
        weight->SetText(info.weigth_);
        auto date = root->FirstChildElement("ContractCarrierDate")->FirstChildElement("Date");
        date->SetText(info.date_.ToString().c_str());
        for (const auto& tanks: info.tanks_) {
            tinyxml2::XMLElement *node = root->InsertNewChildElement("Carriage");
            node->InsertNewChildElement("CarriageID")->InsertNewText(tanks.number_.c_str());
            node->InsertNewChildElement("OwnerType")->InsertNewText("О");
        }
        for (const auto& passport: info.passports_) {
            tinyxml2::XMLElement *node = root->InsertNewChildElement("ConsignorDocument");
            node->InsertNewChildElement("cat_ru:PrDocumentName")->InsertNewText("Паспорт качества");
            node->InsertNewChildElement("cat_ru:PrDocumentNumber")->InsertNewText(passport.c_str());
            node->InsertNewChildElement("catTrans_ru:ModeCode")->InsertNewText("1006088Е");
        }
        doc.SaveFile((dir + "//" + bill + ".xml").c_str());
    }
}


bool domain::Consegnee::operator<(const domain::Consegnee& other) const {
    return std::tie(name_, station_) < std::tie(other.name_, other.station_);
}

bool domain::Date::operator<(const domain::Date& other) {
    return std::tuple(day_, month_, year_) < std::tuple(other.day_, other.month_, other.year_);
}

std::string domain::Date::ToString() const {
    return std::to_string(day_) + "." + std::to_string(month_) + "." + std::to_string(year_);
}

bool domain::QualityPasport::operator<(const domain::QualityPasport& other) {
    return std::lexicographical_compare(number_.begin(), number_.end(), other.number_.begin(), other.number_.end()) &&
           date_ < other.date_;
}
