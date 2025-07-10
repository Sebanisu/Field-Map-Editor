//
// Created by pcvii on 5/2/2022.
//

#include "Upscales.hpp"
#include "GenericCombo.hpp"
static constexpr auto upscale_paths_index  = std::string_view("upscale_paths_index");
static constexpr auto upscale_paths_vector = std::string_view("upscale_paths_vector");
bool                  ff_8::Upscales::on_im_gui_update() const
{
     if (glengine::GenericCombo("Swizzle Path", m_current, m_paths | std::ranges::views::transform([](auto &&value) -> decltype(auto) {
                                                                return value.template ref<std::string>();
                                                           })))
     {
          auto config = Configuration{};
          config->insert_or_assign(upscale_paths_index, m_current);
          config.save();
          return true;
     }
     return false;
}
const std::string &ff_8::Upscales::string() const
{
     if (std::cmp_less(m_current, std::ranges::size(m_paths)))
     {
          return m_paths.get(static_cast<std::size_t>(m_current))->ref<std::string>();
     }
     const static auto empty = std::string("");
     return empty;
}
static void remove_unreachable_paths(std::vector<std::string> &paths)
{
     const auto [first, last] = std::ranges::remove_if(paths, [](const std::filesystem::path path) {
          std::error_code ec{};
          const bool      found = std::filesystem::exists(path, ec);
          if (ec)
          {
               spdlog::error("{}:{} - {}: {} - \"{}\"", __FILE__, __LINE__, ec.value(), ec.message(), path.string());
               ec.clear();
          }
          const bool is_dir = std::filesystem::is_directory(path, ec);
          if (ec)
          {
               spdlog::error("{}:{} - {}: {} - \"{}\"", __FILE__, __LINE__, ec.value(), ec.message(), path.string());
               ec.clear();
          }
          return !found || !is_dir;
     });
     paths.erase(first, last);
}
static void check_paths(std::vector<std::string> &paths)
{
     if (!std::ranges::empty(paths))
     {
          const auto     &path = paths.front();
          std::error_code ec{};
          const auto      handle_error = [&ec](int line, const auto &extra) {
               if (ec)
               {
                    spdlog::error("{}:{} - {}: {}, - \"{}\"", __FILE__, line, ec.value(), ec.message(), extra);
               }
          };
          auto recursive_dir = std::filesystem::recursive_directory_iterator(path, ec);
          if (ec)
          {
               handle_error(__LINE__, path);
               return;
          }
          using CountT     = std::iter_difference_t<decltype(std::filesystem::directory_iterator(path, ec))>;
          CountT max_count = {};
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
                    const auto count = std::ranges::count_if(counting_path, [&handle_error, &ec](const auto &item) {
                         bool is_regular_file = std::filesystem::is_regular_file(item, ec);
                         handle_error(__LINE__, item.path().string());
                         return is_regular_file;
                    });
                    if (count > 0)
                    {
                         spdlog::debug("path: \"{}\",\tfile count: {}", sub_path.path().string(), count);
                         if (max_count < count)
                              max_count = count;
                    }
               }
          }
          spdlog::debug("max file count is: {}", max_count);
     }
}
static std::vector<std::string> get_default_paths()
{
     using namespace std::literals::string_literals;
     auto paths = std::vector{ tl::string::replace_slashes(R"(D:\Angelwing-Ultima_Remastered_v1-0-a\field_bg)"s),
                               tl::string::replace_slashes(R"(/mnt/D/Angelwing-Ultima_Remastered_v1-0-a/field_bg)"s)

     };
     remove_unreachable_paths(paths);
     check_paths(paths);
     return paths;
}
ff_8::Upscales::Upscales()
  : Upscales(Configuration{})
{
}
ff_8::Upscales::Upscales(Configuration config)
  : m_current(config[upscale_paths_index].value_or(int{}))
  , m_paths([&]() -> toml::array {
       if (!config->contains(upscale_paths_vector))
       {
            static const auto default_paths = get_default_paths();
            // todo get all default paths for linux and windows.
            toml::array       paths_array{};
            paths_array.reserve(default_paths.size());
            for (const auto &path : default_paths)
            {
                 paths_array.push_back(path);
            }
            config->insert_or_assign(upscale_paths_vector, std::move(paths_array));
            config.save();
       }
       return *(config->get_as<toml::array>(upscale_paths_vector));
  }())
{
}

ff_8::Upscales::operator std::filesystem::path() const
{
     return string();
}
