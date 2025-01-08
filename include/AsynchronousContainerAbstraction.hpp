// NOLINTBEGIN
#include <fmt/base.h>
#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <mutex>
#include <print>
#include <shared_mutex>
#include <utility>
#include <vector>

namespace gds {
namespace containers {
template <typename T>
class ScopeGuard {
  std::shared_mutex *m_Mutex;
  T &m_Value;

 public:
  ScopeGuard(T &valueToStore, std::shared_mutex *mutex)
      : m_Value(valueToStore), m_Mutex{mutex} {}

  ~ScopeGuard() {}

  // Prevent copying
  ScopeGuard(ScopeGuard const &) = delete;
  ScopeGuard &operator=(ScopeGuard const &) = delete;

  // Allow moving
  ScopeGuard(ScopeGuard &&other) noexcept
      : m_Value(other.m_Value), m_Mutex(other.m_Mutex) {
    other.m_Mutex = nullptr;  // Prevent double unlock
  }

  friend std::ostream &operator<<(std::ostream &os, ScopeGuard const &guard) {
    os << guard.m_Value;
    return os;
  }

  ScopeGuard &operator=(T &&value) {
    std::unique_lock<std::shared_mutex> lock(*m_Mutex);
    m_Value = std::move(value);
    return *this;
  }

  ScopeGuard &operator=(T const &value) {
    std::unique_lock<std::shared_mutex> lock(*m_Mutex);
    m_Value = value;
    return *this;
  }
  ScopeGuard &operator+(T const &value) {
    std::unique_lock<std::shared_mutex> lock(*m_Mutex);
    m_Value += value;
    return *this;
  }
  auto operator+=(T const &value) -> void {
    std::unique_lock<std::shared_mutex> lock(*m_Mutex);
    m_Value += value;
  }

  operator T() const { return m_Value; }
};

template <typename T, template <typename> class Container = std::vector>
  requires requires(Container<T> data1, std::size_t Index, Container<T> data2) {
    data1.size();
    data1[Index];
    std::swap(data1, data2);
  }
class AsyncronousContainerAdapter {
  using ContainerType = Container<T>;

 public:
  class Iterator {
    std::size_t m_Index;
    std::shared_mutex *m_Mutex;
    ContainerType *m_PointerToData;

   public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = ScopeGuard<T>;

    constexpr Iterator(std::size_t index, std::shared_mutex *mutex,
                       ContainerType *container)
        : m_Index(index), m_Mutex(mutex), m_PointerToData(container) {}

    // Iterator arithmetic operations
    constexpr Iterator &operator++() {
      ++m_Index;
      return *this;
    }

    constexpr Iterator operator++(int) {
      Iterator tmp = *this;
      ++m_Index;
      return tmp;
    }

    constexpr Iterator &operator--() {
      --m_Index;
      return *this;
    }

    constexpr Iterator operator--(int) {
      Iterator tmp = *this;
      --m_Index;
      return tmp;
    }

    constexpr Iterator operator+(difference_type n) const {
      return Iterator(m_Index + n, m_Mutex, m_PointerToData);
    }

    constexpr Iterator operator-(difference_type n) const {
      return Iterator(m_Index - n, m_Mutex, m_PointerToData);
    }

    constexpr difference_type operator-(Iterator const &other) const {
      return m_Index - other.m_Index;
    }

    constexpr ScopeGuard<T> operator*() const {
      return ScopeGuard<T>((*m_PointerToData)[m_Index], m_Mutex);
    }

    constexpr bool operator==(Iterator const &other) const {
      return m_PointerToData == other.m_PointerToData &&
             m_Index == other.m_Index;
    }

    constexpr bool operator!=(Iterator const &other) const {
      return !(*this == other);
    }
  };

 private:
  ContainerType m_Data;
  Iterator m_LastElementSet;
  mutable std::shared_mutex m_Mutex;

 public:
  // Constructors
  constexpr AsyncronousContainerAdapter()
      : m_Data(), m_LastElementSet(0, &m_Mutex, m_Data) {};

  constexpr AsyncronousContainerAdapter(std::initializer_list<T> init)
      : m_Data() {
    auto lock = std::unique_lock(m_Mutex);
    std::copy(std::begin(init), std::end(init), std::begin(m_Data));
  }

  constexpr AsyncronousContainerAdapter(AsyncronousContainerAdapter &&other) {
    auto lock = std::unique_lock(other.m_Mutex);
    m_Data = std::move(other.m_Data);
  }
  constexpr AsyncronousContainerAdapter(
      AsyncronousContainerAdapter const &other) {
    auto lock1 = std::shared_lock(other.m_Mutex);
    auto lock2 = std::shared_lock(m_Mutex);
    m_Data = other.m_Data;
  }
  constexpr AsyncronousContainerAdapter &operator=(
      AsyncronousContainerAdapter const &other) {
    auto lock1 = std::shared_lock(other.m_Mutex);
    auto lock2 = std::shared_lock(m_Mutex);
    m_Data = other.m_Data;
    return *this;
  }

  // Iterator access
  constexpr auto begin() -> Iterator { return Iterator(0, &m_Mutex, &m_Data); }
  constexpr auto end() -> Iterator {
    auto lock = std::shared_lock(m_Mutex);
    return Iterator(m_Data.size(), &m_Mutex, &m_Data);
  }

  // Modifiers
  template <typename... Args>
  constexpr auto emplace_back(Args &&...args) -> void {
    push_back(T(args...));
  }

  constexpr auto push_back(T const &value) -> void {
    *m_LastElementSet = value;
    m_LastElementSet++;
  }

  constexpr auto push_back(T &&value) -> void {
    std::unique_lock Lock(m_Mutex);
    m_Data.push_back(std::move(value));
  }

  constexpr auto operator=(std::initializer_list<T> ilist)
      -> AsyncronousContainerAdapter & {
    std::unique_lock Lock(m_Mutex);
    std::ranges::copy(std::begin(ilist), std::end(ilist), std::begin(m_Data));
    return *this;
  }

  constexpr auto size() -> std::size_t {
    std::shared_lock Lock(m_Mutex);
    return m_Data.size();
  }

  constexpr auto swap(AsyncronousContainerAdapter &other) noexcept -> void {
    if (this != &other) {
      std::scoped_lock Lock(m_Mutex, other.m_Mutex);
      std::swap(m_Data, other.m_Data);
    }
  }
};
}  // namespace containers
}  // namespace gds
// NOLINTEND
