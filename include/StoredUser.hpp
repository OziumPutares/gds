#include <json/value.h>
#include <strings.h>

#include <cassert>
#include <string>
#include <utility>

#include "AsynchronousContainerAbstraction.hpp"

struct StoredUser {
  Json::Value m_Data;
  [[nodiscard]] auto VerifyPassword(std::string const& username,
                                    std::string const& hashedPassword,
                                    std::string const& /*salt*/) noexcept
      -> bool {
    return m_Data["hashedPassword"] == hashedPassword &&
           m_Data["username"] == username;
  }
  auto GetUsername() -> std::string { return m_Data["username"].asString(); }
  [[nodiscard]] auto GetHashedPassword() const -> std::string {
    return m_Data["hashedPassword"].asString();
  }
  StoredUser(std::string const& username, std::string const& hashedPassword,
             std::string const& salt, Json::Value userData) {
    m_Data["username"] = username;
    m_Data["hashedPassword"] = hashedPassword;
    m_Data["salt"] = salt;
    m_Data["data"] = std::move(userData);
  }
  explicit StoredUser(Json::Value value) : m_Data(std::move(value)) {
    if (m_Data["username"].empty()) throw "User must have username";
    if (m_Data["hashedPassword"].empty()) throw "User must have hashedPassword";
    if (m_Data["salt"].empty()) throw "User must have salt";
    if (m_Data["data"].empty()) throw "User must have data";
  }
};

// Define alias for storing users to abstract the process away
using StoredUsers = gds::containers::AsyncronousContainerAdapter<StoredUser>;
