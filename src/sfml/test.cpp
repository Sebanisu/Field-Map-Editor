//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
#include "gui.hpp"
int main()
{
  //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  const auto the_gui = fme::gui{};
  the_gui.start();
  // _CrtDumpMemoryLeaks();
}