#ifndef E159DA9F_0523_409F_AF04_C5E64CCA560C
#define E159DA9F_0523_409F_AF04_C5E64CCA560C
#include "Selections.hpp"
namespace fme
{
class keyboard_shortcuts_window
{
     std::weak_ptr<Selections> m_selections = {};

   public:
     keyboard_shortcuts_window(std::weak_ptr<Selections>);
     void render() const;
};
}// namespace fme
#endif /* E159DA9F_0523_409F_AF04_C5E64CCA560C */
