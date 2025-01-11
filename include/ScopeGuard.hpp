#pragma once
#include <mutex>
#include <ostream>
#include <shared_mutex>

/**
 * @brief RAII wrapper for thread-safe access to stored values
 * @tparam T Type of value to be guarded
 * @note Provides automatic locking/unlocking of mutex when accessing values
 * @note Supports move semantics but prevents copying to maintain RAII
 * guarantees
 */
template <typename T>
class ScopeGuard {
  std::shared_mutex* m_Mutex;
  T* m_Value;

 public:
  /**
   * @brief Constructs guard around value with associated mutex
   * @param valueToStore Reference to value that needs thread-safe access
   * @param mutex Pointer to mutex that guards the value
   * @note Takes pointer to mutex to allow for moving of guard object
   */
  ScopeGuard(T& valueToStore, std::shared_mutex* mutex)
      : m_Value(&valueToStore), m_Mutex{mutex} {}

  /**
   * @brief Default destructor
   * @note No explicit mutex unlocking needed as RAII handles it
   */
  ~ScopeGuard() = default;

  /**
   * @brief Deleted copy constructor to maintain RAII semantics
   */
  ScopeGuard(ScopeGuard const&) = delete;

  /**
   * @brief Deleted copy assignment to maintain RAII semantics
   */
  auto operator=(ScopeGuard const&) -> ScopeGuard& = delete;

  /**
   * @brief Move constructor
   * @param other ScopeGuard to move from
   * @note Transfers mutex ownership and nulls other's mutex to prevent double
   * unlock
   */
  ScopeGuard(ScopeGuard&& other) noexcept
      : m_Value(other.m_Value), m_Mutex(other.m_Mutex) {
    other.m_Mutex = nullptr;
  }

  /**
   * @brief Stream output operator for guarded value
   * @param outputStream Stream to output to
   * @param guard Guard containing value to output
   * @return Reference to output stream
   */
  friend auto operator<<(std::ostream& outputStream,
                         ScopeGuard const& guard) -> std::ostream& {
    outputStream << guard.m_Value;
    return outputStream;
  }

  /**
   * @brief Move assignment operator for stored value
   * @param value Value to move into guarded storage
   * @return Reference to this guard
   * @note Acquires unique lock before move assignment
   */
  auto operator=(T&& value) -> ScopeGuard& {
    std::unique_lock<std::shared_mutex> Lock(*m_Mutex);
    m_Value = std::move(value);
    return *this;
  }

  /**
   * @brief Copy assignment operator for stored value
   * @param value Value to copy into guarded storage
   * @return Reference to this guard
   * @note Acquires unique lock before copy assignment
   */
  auto operator=(T const& value) -> ScopeGuard& {
    std::unique_lock<std::shared_mutex> Lock(*m_Mutex);
    *m_Value = value;
    return *this;
  }

  /**
   * @brief Addition operator for guarded value
   * @param value Value to add to stored value
   * @return Reference to this guard
   * @note Acquires unique lock before addition
   */
  auto operator+(T const& value) -> ScopeGuard& {
    std::unique_lock<std::shared_mutex> Lock(*m_Mutex);
    m_Value += value;
    return *this;
  }

  /**
   * @brief Addition assignment operator for guarded value
   * @param value Value to add to stored value
   * @note Acquires unique lock before addition assignment
   */
  auto operator+=(T const& value) -> void {
    std::unique_lock<std::shared_mutex> Lock(*m_Mutex);
    m_Value += value;
  }

  /**
   * @brief Conversion operator to stored type
   * @return Copy of stored value
   */
  operator T() const { return *m_Value; }  // NOLINT
};

/**
 * @brief Formatter specialization for ScopeGuard
 * @note Allows ScopeGuard to work with std::format
 */
template <typename T>
class std::formatter<ScopeGuard<T>> {
 public:
  /**
   * @brief Parse formatting specification
   * @param ctx Format parse context
   * @return Iterator to end of parsed range
   */
  constexpr auto parse(format_parse_context& ctx) {  // NOLINT
    return ctx.begin();
  }

  /**
   * @brief Format ScopeGuard object
   * @param guarded ScopeGuard to format
   * @param ctx Formatting context
   * @return Iterator to end of formatted range
   */
  template <typename Context>
  constexpr auto format(ScopeGuard<T> const& guarded,  // NOLINT
                        Context& ctx) const {
    T Value = guarded;
    return format_to(ctx.out(), "({})", Value);
  }
};
