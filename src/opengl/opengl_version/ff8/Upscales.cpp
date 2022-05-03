//
// Created by pcvii on 5/2/2022.
//

#include "Upscales.hpp"
#include "GenericCombo.hpp"
bool ff8::Upscales::OnImGuiUpdate() const
{
  return glengine::GenericCombo("Upscale Path", m_current, m_paths);
}
const std::string &ff8::Upscales::Path() const
{
  if (std::cmp_less(m_current, std::ranges::size(m_paths)))
  {
    return m_paths.at(static_cast<std::size_t>(m_current));
  }
  const static auto empty = std::string("");
  return empty;
}
ff8::Upscales::Upscales()
{
  using namespace std::literals::string_literals;
  m_paths = std::vector{
    tl::string::replace_slashes(R"(D:\Angelwing-Ultima_Remastered_v1-0-a)"s),
    tl::string::replace_slashes(R"(/mnt/D/Angelwing-Ultima_Remastered_v1-0-a)"s)

  };
  const auto [first, last] =
    std::ranges::remove_if(m_paths, [](const std::filesystem::path path) {
      std::error_code ec{};
      const bool      found = std::filesystem::exists(path, ec);
      if (ec)
      {
        std::cerr << "error " << __FILE__ << ":" << __LINE__ << " - "
                  << ec.value() << ": " << ec.message() << ec.value()
                  << " - path: " << path << std::endl;
        ec.clear();
      }
      const bool is_dir = std::filesystem::is_directory(path, ec);
      if (ec)
      {
        std::cerr << "error " << __FILE__ << ":" << __LINE__ << " - "
                  << ec.value() << ": " << ec.message() << " - path: " << path
                  << std::endl;
        ec.clear();
      }
      return !found || !is_dir;
    });
  m_paths.erase(first, last);
}
