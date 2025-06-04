#include "utilities.hpp"
#include <vector>
#include <string>
namespace fme
{
// Explicit instantiation for specific types
template void sort_and_remove_duplicates(std::vector<std::string> &, std::vector<bool> &);
}