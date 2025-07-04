#ifndef D502712C_C54A_4D5C_AD6F_13895889DA06
#define D502712C_C54A_4D5C_AD6F_13895889DA06
#include <filesystem>
#include <fmt/std.h>
#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <system_error>
namespace fme
{
[[nodiscard]] static inline ImFont *icons_font()
{
     static ImFont *fa_icons = nullptr;
     if (fa_icons)
     {
          return fa_icons;
     }
     std::error_code ec                = {};
     const auto      current_directory = std::filesystem::current_path(ec);
     if (ec)
     {
          spdlog::error("Failed to get current directory: {}", ec.message());
          return nullptr;
     }

     const auto font_path = current_directory / "fonts" / FONT_ICON_FILE_NAME_FAS;
     spdlog::info("Font path: {}", font_path.string());

     // Check if the font file exists
     if (!std::filesystem::exists(font_path, ec))
     {
          spdlog::error("Font file not found: {}", font_path.string());
          return nullptr;
     }
     if (ec)
     {
          spdlog::error("Error checking font file existence: {}", ec.message());
          return nullptr;
     }

     ImGuiIO &io = ImGui::GetIO();
     io.Fonts->AddFontDefault();
     static ImFontConfig config{};
     config.MergeMode = true;
     std::strncpy(config.Name, FONT_ICON_FILE_NAME_FAS, sizeof(config.Name) - 1);
     config.Name[sizeof(config.Name) - 1]   = '\0';// Ensure null-termination
     config.GlyphMinAdvanceX                = 13.0f;// Use if you want to make the icon monospaced
     static const ImWchar     icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
     static const std::string str_path      = font_path.string();
     fa_icons                               = io.Fonts->AddFontFromFileTTF(str_path.c_str(), 13.0f, &config, icon_ranges);
     if (!fa_icons)
     {
          spdlog::error("Failed to load font: {}", str_path);
          return nullptr;
     }
     return fa_icons;
}
}// namespace fme
#endif /* D502712C_C54A_4D5C_AD6F_13895889DA06 */
