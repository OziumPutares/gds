#pragma once
#include <json/value.h>
#include <strings.h>

#include <cassert>
#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

#include "AsynchronousContainerAbstraction.hpp"
#include "JsonFormatting.hpp"

struct StoredUser {
 public:
  Json::Value m_Data;
  [[nodiscard]] auto VerifyPassword(
      std::string const& username, std::string const& hashedPassword,
      std::string const& /*salt*/) noexcept -> bool {
    return m_Data["hashedPassword"] == hashedPassword &&
           m_Data["username"] == username;
  }
  [[nodiscard]] auto GetData() const noexcept -> Json::Value {
    return m_Data["data"];
  }
  [[nodiscard]] auto GetUsernameThrows() -> std::string {
    return m_Data["username"].asString();
  }
  [[nodiscard]] auto GetHashedPasswordThrows() const -> std::string {
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
    if (m_Data["username"].empty()) {
      throw std::invalid_argument("User must have username\n");
    }
    if (m_Data["hashedPassword"].empty()) {
      throw std::invalid_argument("User must have hashedPassword\n");
    }
    // if (m_Data["salt"].empty()) {
    //   throw std::invalid_argument("User must have salt\n");
    // }
    // if (m_Data["data"].empty()) {
    //   throw std::invalid_argument("User must have data\n");
    // }
  }
};

template <>
class std::formatter<StoredUser> {
 public:
  constexpr auto parse(format_parse_context& ctx) {  // NOLINT
    return ctx.begin();
  }
  template <typename Context>
  constexpr auto format(StoredUser const& user, Context& ctx) const {  // NOLINT
    return format_to(ctx.out(), "({})", (user.m_Data));
  }
};

// Define alias for storing users to abstract the process away
using StoredUsers = gds::containers::AsyncronousContainerAdapter<StoredUser>;
