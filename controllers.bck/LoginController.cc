#include "LoginController.h"

#include <botan/hash.h>
#include <botan/hex.h>
#include <botan/pbkdf2.h>
#include <json/json.h>

#include <algorithm>

#include "../include/NotifyOfError.hpp"
#include "../include/StoredUser.hpp"

// Demo user database (replace with real database in production)

void LoginController::Login(
    drogon::HttpRequestPtr const &req,
    std::function<void(drogon::HttpResponsePtr const &)> &&callback) {
  Json::Value Result;
  auto JsonBody = req->getJsonObject();

  if (!JsonBody) {
    ContactClientAboutError("Invalid json", callback);
  }

  std::string Username = (*JsonBody)["username"].asString();
  std::string HashedPassword = (*JsonBody)["hashedPassword"].asString();

  try {
    // Find user
    auto UserIt =
        std::ranges::find_if(Users, [&Username](StoredUser const &user) {
          return user.m_Data["username"] == Username;
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
    ContactClientAboutError("Invalid credentials", callback);
  } catch (std::exception const &Expception) {
    ContactClientAboutError("Server error during login", callback);
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
