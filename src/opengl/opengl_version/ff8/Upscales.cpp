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
  m_paths =
    std::vector{ tl::string::replace_slashes(
                   R"(D:\Angelwing-Ultima_Remastered_v1-0-a\field_bg)"s),
                 tl::string::replace_slashes(
                   R"(/mnt/D/Angelwing-Ultima_Remastered_v1-0-a/field_bg)"s)

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
  if (!std::ranges::empty(m_paths))
  {
    const auto     &path = m_paths.front();
    std::error_code ec{};
    const auto      handle_error = [&ec](int line, const auto &extra) {
      if (ec)
      {
        fmt::print(
          stderr,
          "error {}:{} - {}: {}, - \"{}\"\n",
          __FILE__,
          line,
          ec.value(),
          ec.message(),
          extra);
      }
    };
    auto recursive_dir =
      std::filesystem::recursive_directory_iterator(path, ec);
    if (ec)
    {
      handle_error(__LINE__, path);
      return;
    }
    using count_t =
      std::iter_difference_t<decltype(std::filesystem::directory_iterator(
        path, ec))>;
    count_t max_count = {};
    for (const auto &sub_path : recursive_dir)
    {
      const bool is_dir = std::filesystem::is_directory(sub_path, ec);
      handle_error(__LINE__, path);
      if (is_dir)
      {
        auto counting_path = std::filesystem::directory_iterator(sub_path, ec);
        if (ec)
        {
          handle_error(__LINE__, sub_path.path().string());
          continue;
        }
        const auto count = std::ranges::count_if(
          counting_path, [&handle_error, &ec](const auto &item) {
            bool is_regular_file = std::filesystem::is_regular_file(item, ec);
            handle_error(__LINE__, item.path().string());
            return is_regular_file;
          });
        if (count > 0)
        {
          fmt::print(
            "path: \"{}\",\tfile count: {}\n", sub_path.path().string(), count);
          if (max_count < count)
            max_count = count;
        }
      }
    }
    fmt::print("max file count is: {}\n", max_count);
  }
}
