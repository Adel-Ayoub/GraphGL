#include "data_manager.h"
#include <sstream>
#include <iostream>
#include <cstring>

namespace graphgl {

bool DataManager::importData(const std::string& filename,
                            std::vector<Equation>& equations,
                            std::vector<Point>& points) const {
    std::ifstream infile(filename);
    
    if (!infile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "Equation") {
            Equation eq;
            parseEquationLine(line, eq);
            equations.push_back(eq);
        } else if (type == "Point") {
            Point pt;
            parsePointLine(line, pt);
            points.push_back(pt);
        }
    }

    infile.close();
    return true;
}

bool DataManager::exportData(const std::string& filename,
                            const std::vector<Equation>& equations,
                            const std::vector<Point>& points) const {
    std::string filepath = filename;
    if (filepath.find(".mat") == std::string::npos) {
        filepath += ".mat";
    }

    std::ofstream outfile(filepath);
    
    if (!outfile.is_open()) {
        std::cerr << "Failed to create file: " << filepath << std::endl;
        return false;
    }

    // Export equations
    for (const auto& eq : equations) {
        outfile << "Equation \""
                << eq.color[0] << " " << eq.color[1] << " " << eq.color[2] << " "
                << eq.sampleSize << " "
                << eq.minX << " " << eq.maxX << " "
                << eq.minY << " " << eq.maxY << " "
                << (eq.isVisible ? 1 : 0) << " " << (eq.is3D ? 1 : 0) << " "
                << eq.expression << "\"\n";
    }

    // Export points
    for (const auto& pt : points) {
        outfile << "Point " 
                << pt.position.x << " " << pt.position.y << " " << pt.position.z << " "
                << pt.color[0] << " " << pt.color[1] << " " << pt.color[2] << "\n";
    }

    outfile.close();
    return true;
}

void DataManager::parseEquationLine(const std::string& line, Equation& eq) const {
    // Fields are wrapped in quotes by exportData, so we parse from within them.
    size_t firstQuote = line.find('"');
    size_t lastQuote = line.rfind('"');

    if (firstQuote == std::string::npos || lastQuote == std::string::npos || firstQuote == lastQuote) {
        std::cerr << "Invalid equation line format: " << line << std::endl;
        return;
    }

    std::string content = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
    std::istringstream iss(content);

    iss >> eq.color[0] >> eq.color[1] >> eq.color[2];
    iss >> eq.sampleSize;
    iss >> eq.minX >> eq.maxX >> eq.minY >> eq.maxY;
    
    int visible, is3D;
    iss >> visible >> is3D;
    eq.isVisible = (visible != 0);
    eq.is3D = (is3D != 0);

    // Expression is the remaining content after the numeric fields.
    std::string expr;
    std::getline(iss, expr);
    expr.erase(0, expr.find_first_not_of(" \t"));
    expr.erase(expr.find_last_not_of(" \t") + 1);
    eq.expression = expr;
}

void DataManager::parsePointLine(const std::string& line, Point& pt) const {
    std::istringstream iss(line);
    std::string type;
    iss >> type;

    if (type != "Point") {
        return;
    }

    // Parse position
    iss >> pt.position.x >> pt.position.y >> pt.position.z;
    
    // Parse color
    iss >> pt.color[0] >> pt.color[1] >> pt.color[2];
}

std::string DataManager::removeQuotes(const std::string& str) const {
    std::string result;
    result.reserve(str.size());

    for (char ch : str) {
        if (ch != '"' && ch != '\'') {
            result += ch;
        }
    }

    return result;
}

} // namespace graphgl

