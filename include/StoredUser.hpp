#include <json/value.h>
#include <strings.h>
#include <trantor/net/callbacks.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
struct StoredUser {
  Json::Value m_Data;
  [[nodiscard]] auto VerifyPassword(
      std::string const &username, std::string const &hashedPassword,
      std::string const & /*salt*/) noexcept -> bool {
    return m_Data["hashedPassword"] == hashedPassword &&
           m_Data["username"] == username;
  }
  auto GetUsername() -> std::string { return m_Data["username"].asString(); }
  [[nodiscard]] auto GetHashedPassword() const -> std::string {
    return m_Data["hashedPassword"].asString();
  }
  StoredUser(std::string const &username, std::string const &hashedPassword) {
    m_Data["username"] = username;
    m_Data["hashedPassword"] = hashedPassword;
  }
  explicit StoredUser(Json::Value value) : m_Data(std::move(value)) {}
};
void SaveUsers(std::filesystem::path const &location);
struct UsersClass {
  using UserIterator = std::vector<StoredUser>::iterator;
  using ConstUserIterator = std::vector<StoredUser>::const_iterator;
  std::vector<StoredUser> m_UserData{StoredUser(
      "demo",
      "c6935dba6cb8add354ba421a202166c36845a612fe7cc5e2d891e5e52ad109a3")};
  std::mutex m_ThreadControl;
  auto AddUser(Json::Value const &usersdata) -> void {
    m_ThreadControl.lock();
    SaveUsers(std::filesystem::current_path() / "data");
    m_UserData.emplace_back(usersdata);
    m_ThreadControl.unlock();
  }
  auto AddUserNoSave(Json::Value const &usersdata) -> void {
    m_ThreadControl.lock();
    m_UserData.emplace_back(usersdata);
    m_ThreadControl.unlock();
  }
  auto erase(UserIterator iterator1) -> UserIterator {  // NOLINT
    assert(m_UserData.begin() <= iterator1 && iterator1 < m_UserData.end());
    std::cout << "Waiting for getting vector";
    m_ThreadControl.lock();
    std::cout << "Getting vector";
    m_ThreadControl.unlock();
    return m_UserData.erase(iterator1);
  }
  constexpr auto erase(ConstUserIterator iterator1) -> UserIterator {  // NOLINT
    assert(m_UserData.begin() <= iterator1 && iterator1 < m_UserData.end());
    std::cout << "Waiting for getting vector";
    m_ThreadControl.lock();
    std::cout << "Getting vector";
    m_ThreadControl.unlock();
    return m_UserData.erase(iterator1);
  }
  auto erase(UserIterator iterator1,                    // NOLINT
             UserIterator iterator2) -> UserIterator {  // NOLINT
    assert(m_UserData.begin() <= iterator1 && iterator1 < m_UserData.end() &&
           m_UserData.begin() <= iterator2 && iterator2 < m_UserData.end());
    std::cout << "Waiting for getting vector";
    m_ThreadControl.lock();
    std::cout << "Getting vector";
    m_ThreadControl.unlock();
    return m_UserData.erase(iterator1, iterator2);
  }
  auto erase(ConstUserIterator iterator1,                    // NOLINT
             ConstUserIterator iterator2) -> UserIterator {  // NOLINT
    assert(m_UserData.begin() <= iterator1 && iterator1 < m_UserData.end() &&
           m_UserData.begin() <= iterator2 && iterator2 < m_UserData.end());
    std::cout << "Waiting for getting vector";
    m_ThreadControl.lock();
    std::cout << "Getting vector";
    m_ThreadControl.unlock();
    return m_UserData.erase(iterator1, iterator2);
  }
  auto operator[](std::size_t index) -> StoredUser {
    std::cout << "Waiting for getting vector";
    assert(index < m_UserData.size());
    m_ThreadControl.lock();
    std::cout << "Getting vector";
    m_ThreadControl.unlock();
    return m_UserData[index];
  }
  auto end() -> UserIterator {  // NOLINT
    std::cout << "Waiting for getting vector";
    m_ThreadControl.lock();
    std::cout << "Getting vector";
    m_ThreadControl.unlock();
    return m_UserData.end();
  }
  auto begin() -> UserIterator {  // NOLINT
    std::cout << "Waiting for getting vector";
    m_ThreadControl.lock();
    std::cout << "Getting vector";
    m_ThreadControl.unlock();
    return m_UserData.begin();
  }
};
inline UsersClass Users = {};

inline void LoadUsers(std::filesystem::path const &location) {
  assert(std::filesystem::is_directory(location));
  for (auto &&File : location) {
    Users.AddUserNoSave(Json::Value(File.string()));
  };
}
inline void SaveUsers(std::filesystem::path const &location) {
  assert(std::filesystem::is_directory(location));
  for (auto &&User : Users) {
    std::ofstream((location / (User.m_Data["username"]).asString()).string())
        << User.m_Data;
  }
}
