//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
#define FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
template<std::invocable functionT>
struct [[nodiscard]] scope_guard
{
     explicit scope_guard(functionT &&input_function)
       : m_function(std::forward<functionT>(input_function))
     {
     }

     // Copy constructor
     scope_guard(const scope_guard &) = default;

     // Move constructor
     scope_guard(scope_guard &&other) noexcept
       : m_executed(other.m_executed)
       , m_function(std::move(other.m_function))
     {
          other.m_executed = true;// Make sure the moved-from object doesn't execute the function
     }

     // Copy assignment operator
     scope_guard &operator=(const scope_guard &) = default;

     // Move assignment operator
     scope_guard &operator=(scope_guard &&other) noexcept
     {
          m_executed       = other.m_executed;
          m_function       = std::move(other.m_function);
          other.m_executed = true;
          return *this;
     }

     void operator()() const
     {
          std::invoke(m_function);
          m_executed = true;
     }

     ~scope_guard()
     {
          if (!m_executed)
          {
               std::invoke(m_function);
          }
     }

   private:
     mutable bool m_executed = false;
     functionT    m_function;
};

#endif// FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
