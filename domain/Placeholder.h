#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include <tinyxml2.h>
#include <iostream>
#include <xlnt/xlnt.hpp>

namespace domain {
    struct Date {
        int day_;
        uint32_t month_;
        uint32_t year_;
        std::string ToString() const;
        bool operator<(const Date& other);
    };

    struct Result {
        bool success_ = true;
        std::string error_description_;
    };

    struct RailwayTank {
        std::string number_;
        int weigth_{0};
    };

    struct QualityPasport {
        std::string number_;
        Date date_;

        bool operator<(const QualityPasport& other);
    };

    struct Consegnee {
        std::string name_;
        std::string station_;

        bool operator<(const Consegnee& other) const;
    };

    struct Railwaybill {
        Date date_;
        std::vector<RailwayTank> tanks_;
        std::set<std::string> passports_;
        int weigth_{0};

        bool operator<(const Railwaybill& bill) const;

        bool operator==(const Railwaybill& bill) const;
    };

    using ListRailbills = std::map<Consegnee, std::map<std::string, Railwaybill>>;
    using Railbills = std::map<std::string, Railwaybill>;

    class Placeholder {
    public:
        Result LoadingList(const std::string& filepath);

        void XlDocHandler();

        const ListRailbills& GetListOfRailwaybill() const;

        void GenerateRailwayBills(const Railbills& lists, const std::string& path_of_template, const std::string& dir);

    private:
        Date ConvertFromExcelDays(int days);
        xlnt::workbook wb;
        ListRailbills list_of_railwaybill_;
    };
} // namespace domain
