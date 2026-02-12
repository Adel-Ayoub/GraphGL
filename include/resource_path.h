#pragma once

#include <string>

namespace graphgl {

/// Resolves resource paths relative to the executable, not the CWD.
/// Falls back to the given relative path if the executable directory cannot be determined.
std::string resolveResourcePath(const std::string& relativePath);

} // namespace graphgl
