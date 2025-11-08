//
// Created by pcvii on 9/5/2021.

#ifndef E9C12B82_BF66_405D_B9CA_BB704F701019
#define E9C12B82_BF66_405D_B9CA_BB704F701019
#include "Formatters.hpp"
#include <open_viii/archive/Archives.hpp>
namespace ff_8
{
struct ArchivesGroup
{
   private:
     open_viii::LangT             m_coo           = {};
     std::filesystem::path        m_path          = {};
     mutable bool                 m_failed        = true;
     open_viii::archive::Archives m_archives      = {};
     std::vector<std::string>     m_maplist       = {};
     std::vector<std::string>     m_mapdata       = {};
     std::vector<const char *>    m_mapdata_c_str = {};

     open_viii::archive::Archives get_archives() const;
     std::vector<std::string>     get_map_list() const;
     std::vector<std::string>     get_map_data() const;

   public:
     [[nodiscard]] static std::vector<const char *>
       get_c_str(const std::vector<std::string> &in_vector);
     ArchivesGroup() = default;
     [[maybe_unused]] ArchivesGroup(
       const open_viii::LangT       in_coo,
       const std::filesystem::path &in_path)
       : m_coo(in_coo)
       , m_path(in_path)
       , m_archives(get_archives())
       , m_maplist(get_map_list())
       , m_mapdata(get_map_data())
       , m_mapdata_c_str(get_c_str(m_mapdata))
     {
     }
     /**
      * Creates a new ArchivesGroup pointing to the provided path.
      * @param in_path new path.
      * @return new ArchivesGroup
      */
     [[nodiscard]] ArchivesGroup
       with_path(const std::filesystem::path &in_path) const;
     [[maybe_unused]] [[nodiscard]] const open_viii::LangT                                          &
       coo() const noexcept;
     [[nodiscard]] const std::filesystem::path &path() const noexcept;
     [[nodiscard]] const open_viii::archive::Archives &
       archives() const noexcept;
     [[nodiscard]] const open_viii::archive::FIFLFS<true> &fields() const;
     [[nodiscard]] bool failed() const noexcept;
     [[nodiscard]]      operator bool() const noexcept
     {
          return !m_failed;
     }
     [[nodiscard]] const std::vector<std::string> &
       map_data_from_maplist() const noexcept;
     [[nodiscard]] const std::vector<std::string> &mapdata() const noexcept;
     [[nodiscard]] std::shared_ptr<open_viii::archive::FIFLFS<false>>
                       field(int current_map) const;
     [[nodiscard]] int find_field(std::string_view needle) const;
};
}// namespace ff_8
#endif /* E9C12B82_BF66_405D_B9CA_BB704F701019 */
