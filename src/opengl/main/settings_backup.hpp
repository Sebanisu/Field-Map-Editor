//
// Created by pcvii on 2/28/2023.
//

#ifndef FIELD_MAP_EDITOR_SETTINGS_BACKUP_HPP
#define FIELD_MAP_EDITOR_SETTINGS_BACKUP_HPP
#include "filter.hpp"
#include <cstdint>
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

     [[nodiscard]] auto operator<=>(const setting_backup<T> &other) const = default;

     [[nodiscard]] auto operator<=>(const T &in_value) const
     {
          return m_value <=> in_value;
     }

     [[nodiscard]] bool operator==(const T &in_value) const
     {
          return m_value == in_value;
     }
};
struct settings_backup
{
   public:
     setting_backup<ff_8::filters> filters;
     setting_backup<bool>          draw_swizzle;
     setting_backup<bool>          disable_texture_page_shift;
     setting_backup<bool>          disable_blends;
     setting_backup<std::int32_t>  scale;

     settings_backup(
       ff_8::filters &in_filters,
       bool          &in_draw_swizzle,
       bool          &in_disable_texture_page_shift,
       bool          &in_disable_blends,
       std::int32_t  &in_scale)
       : filters{ in_filters }
       , draw_swizzle{ in_draw_swizzle }
       , disable_texture_page_shift{ in_disable_texture_page_shift }
       , disable_blends{ in_disable_blends }
       , scale{ in_scale }
     {
     }
};
#endif// FIELD_MAP_EDITOR_SETTINGS_BACKUP_HPP
