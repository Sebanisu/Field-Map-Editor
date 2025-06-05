#include "utilities.hpp"
#include <vector>
#include <string>
namespace fme
{
// Explicit instantiation for specific types
template bool sort_and_remove_duplicates(std::vector<std::string> &, std::vector<bool> &);
template bool sort_and_remove_duplicates(std::vector<std::string> &);
}
