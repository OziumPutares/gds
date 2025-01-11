#pragma once
#include <mutex>
#include <ostream>
#include <shared_mutex>
template <typename T>
class ScopeGuard {
  std::shared_mutex *m_Mutex;
  T *m_Value;

 public:
  ScopeGuard(T &valueToStore, std::shared_mutex *mutex)
      : m_Value(&valueToStore), m_Mutex{mutex} {}

  ~ScopeGuard() = default;

  // Prevent copying
  ScopeGuard(ScopeGuard const &) = delete;
  auto operator=(ScopeGuard const &) -> ScopeGuard & = delete;

  // Allow moving
  ScopeGuard(ScopeGuard &&other) noexcept
      : m_Value(other.m_Value), m_Mutex(other.m_Mutex) {
    other.m_Mutex = nullptr;  // Prevent double unlock
  }

  friend auto operator<<(std::ostream &outputStream,
                         ScopeGuard const &guard) -> std::ostream & {
    outputStream << guard.m_Value;
    return outputStream;
  }

  auto operator=(T &&value) -> ScopeGuard & {
    std::unique_lock<std::shared_mutex> Lock(*m_Mutex);
    m_Value = std::move(value);
    return *this;
  }

  auto operator=(T const &value) -> ScopeGuard & {
    std::unique_lock<std::shared_mutex> Lock(*m_Mutex);
    *m_Value = value;
    return *this;
  }
  auto operator+(T const &value) -> ScopeGuard & {
    std::unique_lock<std::shared_mutex> Lock(*m_Mutex);
    m_Value += value;
    return *this;
  }
  auto operator+=(T const &value) -> void {
    std::unique_lock<std::shared_mutex> Lock(*m_Mutex);
    m_Value += value;
  }

  operator T() const { return *m_Value; }
};
template <typename T>
class std::formatter<ScopeGuard<T>> {
 public:
  constexpr auto parse(format_parse_context &ctx) {  // NOLINT
    return ctx.begin();
  }
  template <typename Context>  // NOLINTNEXTLINE
  constexpr auto format(ScopeGuard<T> const &guarded, Context &ctx) const {
    T Value = guarded;
    return format_to(ctx.out(), "({})", Value);
  }
};
