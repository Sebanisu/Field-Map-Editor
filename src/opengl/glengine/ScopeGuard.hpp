//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_SCOPEGUARD_HPP
#define FIELD_MAP_EDITOR_SCOPEGUARD_HPP
#include <concepts>
#include <functional>
namespace glengine
{

//
// Created by pcvii on 11/15/2021.
//
struct [[nodiscard]] AnyScopeGuard
{
     virtual ~AnyScopeGuard() = default;
};
template<std::invocable functionT>
struct [[nodiscard]] ScopeGuard final : public AnyScopeGuard
{
     ScopeGuard()
       : m_executed{ true }
     {
     }

     explicit ScopeGuard(functionT &&input_function)
       : m_function(std::forward<functionT>(input_function))
     {
     }

     // Copy constructor
     ScopeGuard(const ScopeGuard &) = delete;

     // Move constructor
     ScopeGuard(ScopeGuard &&other) noexcept
       : m_executed(other.m_executed)
       , m_function(std::move(other.m_function))
     {
          other.m_executed = true;// Make sure the moved-from object doesn't execute the function
     }

     // Copy assignment operator
     ScopeGuard &operator=(const ScopeGuard &) = delete;

     // Move assignment operator
     ScopeGuard &operator=(ScopeGuard &&other) noexcept
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

     ~ScopeGuard() final
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


// struct [[nodiscard]] ScopeGuard
// {
//      constexpr ScopeGuard(void (*t)())
//        : func(std::move(t))
//      {
//      }

//      constexpr ~ScopeGuard()
//      {// bug in gcc making this not constexpr with -Werror=useless-cast
//           exec();
//      }

//      constexpr ScopeGuard &operator=(void (*t)())
//      {
//           exec();
//           func = std::move(t);
//           return *this;
//      }

//      constexpr ScopeGuard(const ScopeGuard &other)
//        : func(other.func)
//      {
//      }
//      constexpr ScopeGuard &operator=(const ScopeGuard &other)
//      {
//           exec();
//           func = other.func;
//           return *this;
//      }
//      constexpr ScopeGuard(ScopeGuard &&other) noexcept
//        : ScopeGuard()
//      {
//           swap(*this, other);
//      }
//      constexpr ScopeGuard &operator=(ScopeGuard &&other) noexcept
//      {
//           swap(*this, other);
//           return *this;
//      }

//      constexpr friend void swap(ScopeGuard &first,
//                                 ScopeGuard &second) noexcept// nothrow
//      {
//           // enable ADL (not necessary in our case, but good practice)
//           using std::swap;

//           // by swapping the members of two objects,
//           // the two objects are effectively swapped
//           swap(first.func, second.func);
//      }
//      template<size_t count>
//      [[nodiscard]] static constexpr auto array(void (*t)())
//      {
//           std::array<ScopeGuard, count> r{};
//           std::ranges::for_each(r, [&t](ScopeGuard &guard) { guard = t; });
//           return r;
//      }
//      constexpr ScopeGuard() = default;

//    private:
//      constexpr void exec() const
//      {
//           if (func != nullptr)
//           {
//                func();
//           }
//      }
//      void (*func)() = nullptr;
// };
// template<std::invocable functT>
// struct [[nodiscard]] ScopeGuard
// {
//      constexpr ScopeGuard(functT &&in_funct)
//        : func(std::forward<functT>(in_funct))
//        , enabled(true)
//      {
//      }

//      ~ScopeGuard()
//      {// bug in gcc making this not constexpr with -Werror=useless-cast
//           exec();
//      }

//      constexpr ScopeGuard &operator=(functT &&in_funct)
//      {
//           exec();
//           func    = std::forward<functT>(in_funct);
//           enabled = true;
//           return *this;
//      }

//      constexpr ScopeGuard(const ScopeGuard &other)
//        : func(other.func)
//      {
//      }
//      constexpr ScopeGuard &operator=(const ScopeGuard &other)
//      {
//           exec();
//           func    = other.func;
//           enabled = other.enabled;
//           return *this;
//      }
//      constexpr ScopeGuard(ScopeGuard &&other) noexcept
//        : ScopeGuard()
//      {
//           swap(*this, other);
//      }
//      constexpr ScopeGuard &operator=(ScopeGuard &&other) noexcept
//      {
//           exec();
//           swap(*this, other);
//           return *this;
//      }

//      constexpr friend void swap(ScopeGuard &first,
//                                 ScopeGuard &second) noexcept// nothrow
//      {
//           // enable ADL (not necessary in our case, but good practice)
//           using std::swap;

//           // by swapping the members of two objects,
//           // the two objects are effectively swapped
//           swap(first.func, second.func);
//           swap(first.enabled, second.enabled);
//      }
//      template<size_t count>
//      [[nodiscard]] static constexpr auto array(void (*t)())
//      {
//           std::array<ScopeGuard, count> r{};
//           std::ranges::for_each(r, [&t](ScopeGuard &guard) { guard = t; });
//           return r;
//      }
//      constexpr ScopeGuard() = default;

//    private:
//      constexpr void exec() const
//      {
//           if (enabled)
//           {
//                func();
//                enabled = false;
//           }
//      }
//      functT       func={};
//      mutable bool enabled = { false };
// };

// // struct [[nodiscard]] ScopeGuard_expensive
// //{
// //   ScopeGuard_expensive(std::function<void()> t);
// //   ScopeGuard_expensive(const ScopeGuard_expensive &other);
// //
// //   ScopeGuard_expensive &operator=(const ScopeGuard_expensive &other);
// //
// //   ScopeGuard_expensive(ScopeGuard_expensive &&other) noexcept;
// //
// //   ScopeGuard_expensive &operator=(ScopeGuard_expensive &&other) noexcept;
// //
// //   ScopeGuard_expensive &operator=(std::function<void()> t);
// //
// //   ~ScopeGuard_expensive();
// //
// //   friend void
// //     swap(ScopeGuard_expensive &first, ScopeGuard_expensive &second)
// //     noexcept;
// //
// //   template<std::convertible_to<std::function<void()>> T, size_t count>
// //   [[nodiscard]] static std::array<ScopeGuard_expensive, count> array(T t)
// //   {
// //     std::array<ScopeGuard_expensive, count> r{};
// //     std::ranges::for_each(r, [&t](ScopeGuard_expensive &guard) { guard = t;
// //     }); return r;
// //   }
// //
// //   ScopeGuard_expensive() = default;
// //
// // private:
// //   std::function<void()> func{};
// // };
}// namespace glengine
#endif// FIELD_MAP_EDITOR_SCOPEGUARD_HPP
