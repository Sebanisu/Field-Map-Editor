#include "push_pop_id.hpp"
[[nodiscard]] int &get_imgui_id()
{
     static int imgui_id = {};
     return imgui_id;
}