#include <filesystem>

#include "StoredUser.hpp"

namespace {

StoredUsers DefualtValue;
}  // namespace

namespace gds::users {
inline auto GetUsers() -> StoredUsers& { return DefualtValue; }
inline auto SetUsers(StoredUsers const& newDefualt) -> void {
  DefualtValue = newDefualt;
}
// Function that loads users from predefined path (usually ./data/*)
inline auto LoadUsers(std::filesystem::path const& location, StoredUsers users)
    -> StoredUsers {
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
}  // namespace gds::users
