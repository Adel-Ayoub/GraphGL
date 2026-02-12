#pragma once

#include "equation.h"
#include <string>
#include <vector>
#include <fstream>

namespace graphgl {

/// Reads and writes equations/points in the .mat plain-text format.
class DataManager {
public:
    DataManager() = default;
    ~DataManager() = default;

    [[nodiscard]] bool importData(const std::string& filename, 
                                  std::vector<Equation>& equations,
                                  std::vector<Point>& points);

    [[nodiscard]] bool exportData(const std::string& filename,
                                  const std::vector<Equation>& equations,
                                  const std::vector<Point>& points);

    /// Returns a human-readable message after a failed import/export.
    std::string getLastError() const { return lastError_; }

private:
    std::string lastError_;

    std::string removeQuotes(const std::string& str) const;
    void parseEquationLine(const std::string& line, Equation& eq) const;
    void parsePointLine(const std::string& line, Point& pt) const;
};

} // namespace graphgl

