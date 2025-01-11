#pragma once
#include <json/value.h>
#include <strings.h>

#include <cassert>
#include <stdexcept>
#include <string>
#include <utility>

#include "AsynchronousContainerAbstraction.hpp"
#include "JsonFormatting.hpp"

/**
 * @brief Represents a stored user with authentication and custom data
 * @note Manages user credentials and associated JSON data in a thread-safe
 * manner
 */
struct StoredUser {
 public:
  /**
   * @brief JSON object containing all user data
   * @note Contains username, hashedPassword, salt, and custom data fields
   */
  Json::Value m_Data;

  /**
   * @brief Verifies user credentials
   * @param username Username to verify
   * @param hashedPassword Hashed password to check
   * @param salt Salt used in password hashing (currently unused)
   * @return true if credentials match, false otherwise
   * @note Currently ignores salt parameter
   */
  [[nodiscard]] auto VerifyPassword(
      std::string const& username, std::string const& hashedPassword,
      std::string const& /*salt*/) noexcept -> bool {
    return m_Data["hashedPassword"] == hashedPassword &&
           m_Data["username"] == username;
  }

  /**
   * @brief Retrieves user's custom data
   * @return JSON object containing user's custom data
   * @note Returns read-only view of data
   */
  [[nodiscard]] auto GetData() const noexcept -> Json::Value {
    return m_Data["data"];
  }

  /**
   * @brief Gets username, throwing on error
   * @return Username string
   * @throws std::runtime_error if username field is missing
   */
  [[nodiscard]] auto GetUsernameThrows() const -> std::string {
    return m_Data["username"].asString();
  }

  /**
   * @brief Gets hashed password, throwing on error
   * @return Hashed password string
   * @throws std::runtime_error if password field is missing
   */
  [[nodiscard]] auto GetHashedPasswordThrows() const -> std::string {
    return m_Data["hashedPassword"].asString();
  }

  /**
   * @brief Creates user from individual fields
   * @param username User's username
   * @param hashedPassword User's hashed password
   * @param salt Password salt
   * @param userData Additional user data as JSON
   * @note All fields are stored in m_Data JSON object
   */
  // NOLINTNEXTLINE
  StoredUser(std::string const& username, std::string const& hashedPassword,
             std::string const& salt, Json::Value userData) {
    m_Data["username"] = username;
    m_Data["hashedPassword"] = hashedPassword;
    m_Data["salt"] = salt;
    m_Data["data"] = std::move(userData);
  }

  /**
   * @brief Creates user from JSON object
   * @param value JSON object containing user data
   * @throws std::invalid_argument if required fields are missing
   * @note Validates presence of required fields
   */
  explicit StoredUser(Json::Value value) : m_Data(std::move(value)) {
    if (m_Data["username"].empty()) {
      throw std::invalid_argument("User must have username\n");
    }
    if (m_Data["hashedPassword"].empty()) {
      throw std::invalid_argument("User must have hashedPassword\n");
    }
  }
};

/**
 * @brief Formatter specialization for StoredUser
 * @note Allows StoredUser to work with std::format
 */
template <>
class std::formatter<StoredUser> {
 public:
  // NOLINTNEXTLINE
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename Context>
  // NOLINTNEXTLINE
  constexpr auto format(StoredUser const& user, Context& ctx) const {
    return format_to(ctx.out(), "({})", (user.m_Data));
  }
};

/**
 * @brief Type alias for thread-safe user storage
 * @note Uses AsyncronousContainerAdapter for thread safety
 */
using StoredUsers = gds::containers::AsyncronousContainerAdapter<StoredUser>;
