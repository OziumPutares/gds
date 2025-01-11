#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

/**
 * @brief Controller handling user authentication and dashboard access
 * @note Uses Drogon framework for HTTP routing and session management
 */
class LoginController : public drogon::HttpController<LoginController> {
 public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(LoginController::Login, "/api/login", drogon::Post,
                drogon::Options);
  ADD_METHOD_TO(LoginController::Dashboard, "/dashboard", drogon::Get, );
  ADD_METHOD_TO(LoginController::CreateAccount, "/api/createAccount",
                drogon::Post, drogon::Options);
  METHOD_LIST_END

  /**
   * @brief Handles user login requests
   * @param req HTTP request containing login credentials
   * @param callback Callback to send HTTP response
   * @note Expects JSON with username, hashedPassword, and salt fields
   * @note Creates session on successful login
   */
  void Login(drogon::HttpRequestPtr const& req,
             std::function<void(drogon::HttpResponsePtr const&)>&& callback);

  /**
   * @brief Displays user dashboard
   * @param req HTTP request with session data
   * @param callback Callback to send dashboard HTML
   * @note Requires active session with logged_in flag
   * @note Displays user's stored data and data entry form
   */
  void Dashboard(
      drogon::HttpRequestPtr const& req,
      std::function<void(drogon::HttpResponsePtr const&)>&& callback);

  /**
   * @brief Handles account creation requests
   * @param req HTTP request containing new account details
   * @param callback Callback to send HTTP response
   * @note Expects JSON with username, hashedPassword, and salt fields
   * @note Validates username uniqueness before creation
   */
  void CreateAccount(
      drogon::HttpRequestPtr const& req,
      std::function<void(drogon::HttpResponsePtr const&)>&& callback);
};
