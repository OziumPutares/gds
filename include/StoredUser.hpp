#include <json/value.h>
#include <strings.h>
#include <trantor/net/callbacks.h>

#include <cassert>
#include <filesystem>
#include <string>

#include "AsynchronousContainerAbstraction.hpp"
struct StoredUser {
  Json::Value m_Data;
  [[nodiscard]] auto VerifyPassword(
      std::string const& username, std::string const& hashedPassword,
      std::string const& /*salt*/) noexcept -> bool {
    return m_Data["hashedPassword"] == hashedPassword &&
           m_Data["username"] == username;
  }
  auto GetUsername() -> std::string { return m_Data["username"].asString(); }
  [[nodiscard]] auto GetHashedPassword() const -> std::string {
    return m_Data["hashedPassword"].asString();
  }
  StoredUser(std::string const& username, std::string const& hashedPassword) {
    m_Data["username"] = username;
    m_Data["hashedPassword"] = hashedPassword;
  }
  explicit StoredUser(Json::Value value) : m_Data(std::move(value)) {}
};

void SaveUsers(std::filesystem::path const& location);

using StoredUsers = AsyncronousContainerAdapter<StoredUser>;
inline StoredUsers LoadUsers(std::filesystem::path const& location,
                             StoredUsers users) {
  assert(std::filesystem::is_directory(location));
  for (auto&& File : location) {
    users.emplace_back(Json::Value(File.string()));
  };
  return std::move(users);
}
inline void SaveUsers(std::filesystem::path const& location) {
  assert(std::filesystem::is_directory(location));
  for (auto&& User : Users) {
    std::ofstream((location / (User.m_Data["username"]).asString()).string())
        << User.m_Data;
  }
}
