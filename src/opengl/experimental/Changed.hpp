//
// Created by pcvii on 7/4/2022.
//

#ifndef FIELD_MAP_EDITOR_CHANGED_HPP
#define FIELD_MAP_EDITOR_CHANGED_HPP
#include <source_location>
struct Changed
{
     void operator=(bool) const = delete;
     void set_if_true(
       bool                 in,
       std::source_location source_location
       = std::source_location::current()) const
     {
          if (in)
               operator()(source_location);
     }
     void operator()(
       std::source_location source_location
       = std::source_location::current()) const
     {

          if (!m_current)
          {
               spdlog::debug(
                 "Changed\n\r{}:{}",
                 source_location.file_name(),
                 source_location.line());
               m_current = true;
          }
     }
     [[nodiscard]] operator bool() const
     {
          return m_current;// || m_previous;
     }
     [[nodiscard]] auto unset() const
     {
          return glengine::ScopeGuard([this] { m_current = false; });
     }

   private:
     /**
      * if Current is true redrawn scene.
      */
     mutable bool m_current = { true };
};
#endif// FIELD_MAP_EDITOR_CHANGED_HPP
