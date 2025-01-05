#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

class LoginController : public drogon::HttpController<LoginController> {
 public:
  METHOD_LIST_BEGIN
  // Remove initPathRouting since it's handled by the macro
  ADD_METHOD_TO(LoginController::Login, "/api/login", drogon::Post,
                drogon::Options);
  ADD_METHOD_TO(LoginController::Dashboard, "/api/dashboard", drogon::Get);
  ADD_METHOD_TO(LoginController::CreateAccount, "/api/createAccount",
                drogon::Get);
  METHOD_LIST_END

  void Login(drogon::HttpRequestPtr const &req,
             std::function<void(drogon::HttpResponsePtr const &)> &&callback);
  void Dashboard(
      drogon::HttpRequestPtr const &req,
      std::function<void(drogon::HttpResponsePtr const &)> &&callback);
  void CreateAccount(
      drogon::HttpRequestPtr const &req,
      std::function<void(drogon::HttpResponsePtr const &)> &&callback);
};
