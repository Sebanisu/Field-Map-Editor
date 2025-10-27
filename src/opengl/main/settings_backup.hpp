//
// Created by pcvii on 2/28/2023.
//

#ifndef FIELD_MAP_EDITOR_SETTINGS_BACKUP_HPP
#define FIELD_MAP_EDITOR_SETTINGS_BACKUP_HPP
#include "filter.hpp"
#include <cstdint>
namespace fme
{
struct map_sprite_settings
{
     bool                         draw_swizzle               = false;
     bool                         disable_texture_page_shift = false;
     bool                         disable_blends             = false;
     bool                         require_coo                = true;
     bool                         force_loading              = false;

     mutable ff_8::TileFilters    tile_filters               = { false };
     mutable ff_8::TextureFilters texture_filters            = { false };
};
template<typename T>
struct setting_backup
{
   private:
     T                         m_backup;
     std::reference_wrapper<T> m_value;

   public:
     setting_backup()                                  = delete;
     setting_backup(const setting_backup &)            = default;
     setting_backup &operator=(const setting_backup &) = default;
     setting_backup(setting_backup &&)                 = delete;
     setting_backup &operator=(setting_backup &&)      = delete;

     explicit setting_backup(T &in_value)
       : m_backup(in_value)
       , m_value(in_value)
     {
     }

     ~setting_backup() noexcept(noexcept(m_value.get() = m_backup))
     {
          m_value.get() = m_backup;
     }

     [[nodiscard]] const T &backup() const
     {
          return m_backup;
     }

     [[nodiscard]] const T &value() const
     {
          return m_value.get();
     }

     [[nodiscard]] T &value()
     {
          return m_value.get();
     }

     auto &operator=(const T &in_value)
     {
          m_value.get() = in_value;
          return *this;
     }

     [[nodiscard]] auto operator<=>(const setting_backup<T> &other) const
       = default;

     [[nodiscard]] auto operator<=>(const T &in_value) const
     {
          return m_value <=> in_value;
     }

     [[nodiscard]] bool operator==(const T &in_value) const
     {
          return m_value == in_value;
     }

     T *operator->()
     {
          return &m_value.get();
     }

     const T *operator->() const
     {
          return &m_value.get();
     }

     T &operator*()
     {
          return m_value.get();
     }

     const T &operator*() const
     {
          return m_value.get();
     }
};
struct settings_backup
{
   public:
     setting_backup<ff_8::TileFilters>   filters;
     setting_backup<map_sprite_settings> settings;
     // setting_backup<std::int32_t>  scale;

     settings_backup(
       ff_8::TileFilters   &in_filters,
       map_sprite_settings &in_settings)
       : filters{ in_filters }
       , settings{ in_settings }
     {
     }
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_SETTINGS_BACKUP_HPP
