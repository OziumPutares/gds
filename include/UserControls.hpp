#pragma once
#include <trantor/utils/Logger.h>

#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>

#include "StoredUser.hpp"

namespace {

StoredUsers DefualtValue;
}  // namespace

namespace gds::users {

/**
 * @brief Retrieves the global users collection
 * @return Reference to the StoredUsers collection
 * @note This function provides access to the global user storage
 */
inline auto GetUsers() -> StoredUsers& { return DefualtValue; }

/**
 * @brief Updates the global users collection
 * @param newDefualt New StoredUsers collection to set as default
 * @note This function replaces the entire user collection with a new one
 */
inline auto SetUsers(StoredUsers const& newDefualt) -> void {
  DefualtValue = newDefualt;
}

/**
 * @brief Loads user data from files in the specified directory
 * @param location Path to the directory containing user data files
 * @note Each file in the directory is expected to contain JSON data for a
 * single user
 * @note Files that cannot be parsed will be logged but won't halt the loading
 * process
 */
inline auto LoadUsers(std::filesystem::path const& location) -> void {
  assert(std::filesystem::is_directory(location));
  LOG_DEBUG << (std::format("Loading users from path: {}", location.string()));

  for (auto&& File : std::filesystem::directory_iterator(location)) {
    if (!File.is_regular_file()) continue;
    try {
      LOG_DEBUG << (std::format("Processing file: {}", File.path().string()));
      auto Tmp = Json::Value{};
      std::ifstream(File.path(), std::ifstream::binary) >> Tmp;
      LOG_DEBUG << std::format("Successfully parsed JSON, adding user {}", Tmp);
      gds::users::GetUsers().emplace_back(Tmp);
    } catch (std::exception const& Error) {
      std::string FileContent;
      std::ifstream FileVal(File.path());
      FileContent.assign(std::istreambuf_iterator<char>(FileVal),
                         std::istreambuf_iterator<char>());

      LOG_DEBUG << (std::format(
          "Error processing file {}: {}\nFile contents:\n{}",
          File.path().string(), Error.what(), FileContent));
    }
  }

  std::string UserList;
  for (auto&& User : gds::users::GetUsers()) {
    UserList += std::format("{}", User);
  }
  LOG_DEBUG << UserList;
}

/**
 * @brief Saves all users to individual files in the specified directory
 * @param location Directory where user files should be saved
 * @note Each user is saved in a separate file named after their username
 * @note The directory must exist before calling this function
 */
inline auto SaveUsers(std::filesystem::path const& location) -> void {
  assert(std::filesystem::is_directory(location));
  LOG_DEBUG << ("Beginning user save operation");

  for (StoredUser const& User : GetUsers()) {
    auto Filepath = location / User.GetUsernameThrows();
    LOG_DEBUG << (std::format("Saving user to file: {}", Filepath.string()));
    std::ofstream(Filepath) << User.m_Data;
    LOG_DEBUG << (std::format("User data saved: {}",
                              User.m_Data.toStyledString()));
  }
}
}  // namespace gds::users
