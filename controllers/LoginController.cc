#include "LoginController.h"

#include <botan/hash.h>
#include <botan/hex.h>
#include <botan/pbkdf2.h>
#include <json/json.h>
#include <trantor/utils/Logger.h>

#include <algorithm>
#include <format>
#include <iostream>
#include <ostream>
#include <print>
#include <sstream>
#include <stacktrace>
#include <string>

#include "../include/NotifyOfError.hpp"
#include "../include/StoredUser.hpp"
#include "../include/UserControls.hpp"

// Demo user database (replace with real database in production)

void LoginController::Login(
    drogon::HttpRequestPtr const &req,
    std::function<void(drogon::HttpResponsePtr const &)> &&callback) {
  std::println("Entering login");
  Json::Value Result;
  auto JsonBody = req->getJsonObject();

  if (!JsonBody) {
    ContactClientAboutError(
        std::format("Invalid json see error message{}", req->getJsonError()),
        callback);
  }

  std::string Username = (*JsonBody)["username"].asString();
  std::string HashedPassword = (*JsonBody)["hashedPassword"].asString();

  try {
    // Find user
    std::println("Stage 3.1");
    std::println("{}", std::size(gds::users::GetUsers()));
    auto UserIt = std::ranges::find_if(
        Users, [&Username, Count = 0](StoredUser &user) mutable {
          std::println("Time {}", ++Count);
          return user.GetUsername() == Username;
        });

    if (UserIt != Users.end()) {
      // Verify password hash
      if (UserIt->VerifyPassword(Username, HashedPassword, "mixed_salt")) {
        Result["success"] = true;
        Result["message"] = "Login successful";

        auto Response = drogon::HttpResponse::newHttpJsonResponse(Result);
        auto Session = req->session();
        Session->insert("logged_in", true);
        Session->insert("username", Username);

        callback(Response);
        return;
      }
    }

    // If we get here, login failed
    std::println("Stage 4");
    ContactClientAboutError("Invalid credentials", callback);
  } catch (std::exception const &Expception) {
    ContactClientAboutError(
        std::format("Server error during login: {}", Expception.what()),
        callback);
  }
}

// Dashboard handler remains the same
void LoginController::Dashboard(
    drogon::HttpRequestPtr const &req,
    std::function<void(drogon::HttpResponsePtr const &)> &&callback) {
  auto Session = req->session();
  if (!Session->find("logged_in")) {
    auto Responseonse = drogon::HttpResponse::newRedirectionResponse("/login");
    callback(Responseonse);
    return;
  }

  auto Response = drogon::HttpResponse::newHttpResponse();
  Response->setBody("<p>You are logged in as: " +
                    Session->get<std::string>("username") + "</p>");
  Response->setContentTypeCode(drogon::CT_TEXT_HTML);
  callback(Response);
}

void LoginController::CreateAccount(
    drogon::HttpRequestPtr const &req,
    std::function<void(drogon::HttpResponsePtr const &)> &&callback) {
  std::cout << "Start of account creation";
  if (!req->getJsonObject()) {
    ContactClientAboutError(
        std::format("Invalid json see error message{}", req->getJsonError()),
        callback);
    return;  // Add this return
  }
  auto JsonFromClient = req->getJsonObject();
  std::string Username = (*JsonFromClient)["username"].asString();
  std::string HashedPassword = (*JsonFromClient)["hashedPassword"].asString();

  Json::Value UserJson;
  UserJson["username"] = Username;
  UserJson["hashedPassword"] = HashedPassword;

  // TODO(Eugeniusz Lewandowski) add User db/control:
  std::cout << CreateContactJson(true, "Account creation successful");
  ContactClient(CreateContactJson(true, "Account creation successful"),
                callback);
}
