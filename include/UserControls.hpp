#pragma once
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ostream>

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
inline auto LoadUsers(std::filesystem::path const& location) -> void {
  assert(std::filesystem::is_directory(location));
  std::println("Path {}", location.string());
  for (auto&& File : std::filesystem::directory_iterator(location)) {
    if (!File.is_regular_file()) continue;
    try {
      std::cout << File.path().string();
      std::cout << '\n';
      auto Tmp = Json::Value{};
      std::ifstream(File.path(), std::ifstream::binary) >> Tmp;
      std::println("Parsed Json adding user json");
      gds::users::GetUsers().emplace_back(Tmp);
    } catch (std::exception const& Error) {
      std::cerr << "An exception was thrown: \n" << Error.what();
      std::cout << "The corresponding json:\n "
                << std::ifstream(File.path()).rdbuf();
    }
  }
}
inline auto SaveUsers(std::filesystem::path const& location) -> void {
  assert(std::filesystem::is_directory(location));
  for (StoredUser&& User : GetUsers()) {
    std::ofstream(location / (User.GetUsername())) << User.m_Data;
  }
}
}  // namespace gds::users
