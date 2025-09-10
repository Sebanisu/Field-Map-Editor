//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_ARCHIVE_HPP
#define FIELD_MAP_EDITOR_ARCHIVE_HPP
#include "Coos.hpp"
#include "Paths.hpp"
#include <open_viii/archive/Archives.hpp>
namespace ff_8
{
class Archive
{
   public:
     Archive();
     void               on_update(float) const {}
     void               on_render() const {}
     [[nodiscard]] bool on_im_gui_update() const;
     void               on_event(const glengine::event::Item &) const {}

     [[nodiscard]] const open_viii::archive::Archives     *operator->() const;
     [[nodiscard]] const open_viii::archive::FIFLFS<true> &fields() const;
     [[nodiscard]] std::string_view                        coo() const;

   private:
     Paths                                m_paths    = {};
     Coos                                 m_coos     = {};
     mutable open_viii::archive::Archives m_archives = {};
};
static_assert(glengine::Renderable<Archive>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_ARCHIVE_HPP
