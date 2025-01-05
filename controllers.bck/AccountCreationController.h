#pragma once

#include <drogon/HttpController.h>

class AccountCreationController
    : public drogon::HttpController<AccountCreationController> {
 public:
  METHOD_LIST_BEGIN
  // use METHOD_ADD to add your custom processing function here;
  // METHOD_ADD(AccountCreationController::get, "/{2}/{1}", Get); // path is
  // /AccountCreationController/{arg2}/{arg1}
  // METHOD_ADD(AccountCreationController::your_method_name, "/{1}/{2}/list",
  // Get); // path is /AccountCreationController/{arg1}/{arg2}/list
  // ADD_METHOD_TO(AccountCreationController::your_method_name,
  // "/absolute/path/{1}/{2}/list", Get); // path is
  // /absolute/path/{arg1}/{arg2}/list
  ADD_METHOD_TO(AccountCreationController::CreateAccount, "/api/createAccount",
                drogon::Post, drogon::Options);

  METHOD_LIST_END

  void CreateAccount(
      drogon::HttpRequestPtr const &req,
      std::function<void(drogon::HttpResponsePtr const &)> &&callback);
  // your declaration of processing function maybe like this:
  // void get(const HttpRequestPtr& req, std::function<void (const
  // HttpResponsePtr &)> &&callback, int p1, std::string p2); void
  // your_method_name(const HttpRequestPtr& req, std::function<void (const
  // HttpResponsePtr &)> &&callback, double p1, int p2) const;
};
