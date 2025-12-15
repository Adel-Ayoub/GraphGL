#pragma once

#include "equation.h"
#include <string>
#include <vector>
#include <fstream>

namespace graphgl {

class DataManager {
public:
    DataManager() = default;
    ~DataManager() = default;

    // Import equations and points from file
    bool importData(const std::string& filename, 
                   std::vector<Equation>& equations,
                   std::vector<Point>& points) const;

    // Export equations and points to file
    bool exportData(const std::string& filename,
                   const std::vector<Equation>& equations,
                   const std::vector<Point>& points) const;

private:
    // Helper functions
    std::string removeQuotes(const std::string& str) const;
    void parseEquationLine(const std::string& line, Equation& eq) const;
    void parsePointLine(const std::string& line, Point& pt) const;
};

} // namespace graphgl

