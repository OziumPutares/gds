// NOLINTBEGIN
#include <fmt/base.h>
#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <future>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <mutex>
#include <numeric>
#include <print>
#include <ranges>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <vector>

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

template <typename T,
          template <typename, auto...> class Container = std::vector,
          auto... Arguments>
class AsyncronousContainerAdapter {
  using ContainerType = Container<T, Arguments...>;
  ContainerType m_Data;
  mutable std::shared_mutex m_Mutex;

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

  // Constructors
  constexpr AsyncronousContainerAdapter() = default;

  constexpr AsyncronousContainerAdapter(std::initializer_list<T> init)
      : m_Data() {
    std::copy(std::begin(init), std::end(init), std::begin(m_Data));
  }

  template <typename... Args>
  constexpr AsyncronousContainerAdapter(Args &&...args)
      : m_Data(std::forward<Args>(args)...) {}

  // Iterator access
  constexpr Iterator begin() { return Iterator(0, &m_Mutex, &m_Data); }
  constexpr Iterator end() {
    return Iterator(m_Data.size(), &m_Mutex, &m_Data);
  }

  // Modifiers
  template <typename... Args>
  constexpr void emplace_back(Args &&...args) {
    std::unique_lock lock(m_Mutex);
    m_Data.emplace_back(std::forward<Args>(args)...);
  }

  constexpr void push_back(T const &value) {
    std::unique_lock lock(m_Mutex);
    m_Data.push_back(value);
  }

  constexpr void push_back(T &&value) {
    std::unique_lock lock(m_Mutex);
    m_Data.push_back(std::move(value));
  }

  constexpr AsyncronousContainerAdapter &operator=(
      std::initializer_list<T> ilist) {
    std::unique_lock lock(m_Mutex);
    m_Data = ilist;
    return *this;
  }

  constexpr void assign(std::size_t count, T const &value) {
    std::unique_lock lock(m_Mutex);
    m_Data.assign(count, value);
  }

  template <typename InputIt>
  constexpr void assign(InputIt first, InputIt last) {
    std::unique_lock lock(m_Mutex);
    m_Data.assign(first, last);
  }

  constexpr void swap(AsyncronousContainerAdapter &other) noexcept {
    if (this != &other) {
      std::scoped_lock lock(m_Mutex, other.m_Mutex);
      m_Data.swap(other.m_Data);
    }
  }
};
// NOLINTEND
