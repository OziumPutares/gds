#include "LoginController.h"

#include <botan/hash.h>
#include <botan/hex.h>
#include <botan/pbkdf2.h>
#include <drogon/HttpTypes.h>
#include <json/json.h>
#include <json/value.h>
#include <trantor/utils/Logger.h>

#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <iterator>
#include <ostream>
#include <print>
#include <sstream>
#include <stacktrace>
#include <string>

#include "../include/NotifyOfError.hpp"
#include "../include/StoredUser.hpp"
#include "../include/UserControls.hpp"

void LoginController::Login(
    drogon::HttpRequestPtr const &req,
    std::function<void(drogon::HttpResponsePtr const &)> &&callback) {
  Json::Value Result;
  auto JsonBody = req->getJsonObject();

  if (!JsonBody) {
    ContactClientAboutError(
        std::format("Invalid json see error message{}", req->getJsonError()),
        callback);
  }

  std::string Username = (*JsonBody)["username"].asString();
  std::string HashedPassword = (*JsonBody)["hashedPassword"].asString();
  std::string Salt = (*JsonBody)["salt"].asString();

  try {
    // Find user
    auto &Users = gds::users::GetUsers();
    auto UserIt = std::find_if(std::begin(Users), std::end(Users),  // NOLINT
                               [&Username](StoredUser user) mutable {
                                 return user.GetUsernameThrows() == Username;
                               });

    if (UserIt != gds::users::GetUsers().end()) {
      // Verify password hash
      if (StoredUser(*UserIt).VerifyPassword(Username, HashedPassword,
                                             "mixed_salt")) {
        req->session()->clear();
        Result["success"] = true;
        Result["message"] = "Login successful";

        auto Response = drogon::HttpResponse::newHttpJsonResponse(Result);
        auto Session = req->session();
        Session->insert("logged_in", true);
        Session->insert("username", Username);
        Session->insert("hashedPassword", HashedPassword);
        Session->insert("salt", Salt);
        StoredUser User = *UserIt;
        Session->insert("data", User.GetData());

        callback(Response);
        return;
      }
    }

    // If we get here, login failed
    ContactClientAboutError("Invalid credentials", callback);
    std::cout << "List of users is";
    for (StoredUser &&User : gds::users::GetUsers()) {
      std::cout << User.m_Data;
    }
  } catch (std::exception const &Expception) {
    ContactClientAboutError(
        std::format("Server error during login: {}", Expception.what()),
        callback);
  }
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
  std::string Salt = (*JsonFromClient)["salt"].asString();
  auto &Users = gds::users::GetUsers();
  // NOLINTNEXTLINE
  if (std::find_if(std::begin(Users), std::end(Users),
                   [Username](StoredUser user) {  // NOLINT
                     return user.GetUsernameThrows() == Username;
                   }) != std::end(Users)) {
    ContactClientAboutError("User already exists", callback);
  }

  Json::Value UserJson;
  UserJson["username"] = Username;
  UserJson["hashedPassword"] = HashedPassword;
  UserJson["salt"] = Salt;
  UserJson["data"] = {};
  gds::users::GetUsers().emplace_back(UserJson);
  gds::users::SaveUsers(std::filesystem::current_path() / "data");
  // REset session
  req->session()->clear();

  // TODO(Eugeniusz Lewandowski) add User db/control:
  std::cout << CreateContactJson(true, "Account creation successful");
  ContactClient(CreateContactJson(true, "Account creation successful"),
                callback);
}
// Dashboard handler remains the same
void LoginController::Dashboard(
    drogon::HttpRequestPtr const &req,
    std::function<void(drogon::HttpResponsePtr const &)> &&callback) {
  auto Session = req->session();
  if (!Session->find("logged_in")) {
    auto Responseonse = drogon::HttpResponse::newRedirectionResponse("/login");
    Responseonse->setStatusCode(drogon::k302Found);
    callback(Responseonse);
    return;
  }

  auto Username = Session->getOptional<std::string>("username");
  auto HashedPassword = Session->getOptional<std::string>("hashedPassword");
  auto Salt = Session->getOptional<std::string>("salt");
  if (!Username.has_value() || !HashedPassword.has_value() ||
      !Salt.has_value()) {
    auto Response = drogon::HttpResponse::newRedirectionResponse("/login");
    Response->setBody("Invalid user");
    Response->setContentTypeCode(drogon::CT_TEXT_HTML);
    callback(Response);
    return;
  }

  std::string OutputHtml =
      R"html(
<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover" />
    <meta name="theme-color" content="#0b0c0c" />
    <title>Your Data</title>

    <!-- Favicons -->
    <link rel="icon" sizes="48x48" href="assets/images/favicon.ico" />
    <link rel="icon" sizes="any" href="assets/images/favicon.svg" type="image/svg+xml" />
    <link rel="mask-icon" href="assets/images/govuk-icon-mask.svg" color="#0b0c0c" />

    <!-- Stylesheets -->
    <link rel="stylesheet" href="css/govuk-frontend.min.css" />
    <link rel="stylesheet" href="css/scp.css" />
    <link rel="stylesheet" href="css/govuk-frontend.min.css" />

    <!-- JavaScript -->
    <script src="js/page-start.js" defer></script>
  </head>

  <body class="govuk-template__body govuk-frontend-supported">
    <!-- Header -->
    <header class="govuk-header" role="banner">
      <div class="govuk-header__container govuk-width-container">
        <div class="govuk-header__logo">
          <a href="https://www.gov.uk" class="govuk-header__link govuk-header__link--homepage">
            <svg role="img" focusable="false" class="govuk-header__logotype" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 148 30" height="30" width="148" aria-label="GOV.UK">
              <title>GOV.UK</title>
              <path d="..."></path>
            </svg>
          </a>
        </div>
      </div>
    </header>

    <!-- Main Content -->
    <div class="govuk-width-container">
      <main class="govuk-main-wrapper govuk-!-padding-top-4" id="main-content" role="main">
        <h1 class="govuk-heading-xl">Your Data</h1>

        <!-- Data Section Heading -->
        <h2 class="govuk-heading-l">Existing Data</h2>

        <!-- Dashboard container with GOV.UK styling classes -->
        <div id="dashboard-container" class="govuk-grid-row">
          <!-- Loading state with GOV.UK styling -->
        </div>

        <!-- JSON Data Handling -->
        )html";

  auto JsonContent = Session->get<Json::Value>("data");
  if (JsonContent.empty()) {
    OutputHtml += R"html(
    <p class="govuk-body">No User data stored yet</p>
    )html";
  } else {
    OutputHtml += R"html(
    <dl class="govuk-summary-list">
    )html";

    for (auto Iterator = std::begin(JsonContent);
         Iterator != std::end(JsonContent); Iterator++) {
      OutputHtml += std::format(
          "<div class=\"govuk-summary-list__row\">\n"
          "<dt class=\"govuk-summary-list__key\">{}</dt>\n"
          "<dd class=\"govuk-summary-list__value\">{}</dd>\n"
          "</div>",
          Iterator.key().asString(), Iterator->asString());
    }
    OutputHtml += R"html(
    </dl>
    )html";
  }

  OutputHtml += R"html(
        <!-- Add Data Section Heading -->
        <h2 class="govuk-heading-l">Add More Data</h2>

        <!-- Add Data Form -->
        <div class="govuk-grid-row">
          <div class="govuk-grid-column-full">
            <div class="govuk-form-group">
              <label class="govuk-label" for="data-key">Key</label>
              <input class="govuk-input govuk-input--width-20" id="data-key" name="data-key" type="text" />
            </div>

            <div class="govuk-form-group">
              <label class="govuk-label" for="data-value">Value</label>
              <input class="govuk-input govuk-input--width-20" id="data-value" name="data-value" type="text" />
            </div>

            <button type="button" class="govuk-button" onclick="addData()">Add Data</button>

            <div id="status-message"></div>
          </div>
        </div>

        <script>
          async function addData() {
            const key = document.getElementById("data-key").value;
            const value = document.getElementById("data-value").value;

            console.log("Adding data");
            try {
              const response = await fetch("/api/addData", {
                method: "POST",
                headers: {
                  "Content-Type": "application/json",
                },
                body: JSON.stringify({
                  key: key,
                  value: value,
                }),
              });

              // Handle redirection
              if (response.status === 301 || response.status === 302 || response.status === 303) {
                const redirectUrl = response.headers.get("Location");
                if (redirectUrl) {
                  window.location.replace(redirectUrl); // Redirect the user to the new location
                }
                return;
              }

              const data = await response.json();
              console.log("Added data");
              const statusDiv = document.getElementById("status-message");

              if (data.success) {
                window.location.reload(); // Use window.location.reload to reload the page
                statusDiv.innerHTML = `<div class="govuk-notification-banner govuk-notification-banner--success" role="alert">
                    <div class="govuk-notification-banner__content">
                      <p class="govuk-notification-banner__heading">${data.message}</p>
                    </div>
                  </div>`;
              } else {
                statusDiv.innerHTML = `<div class="govuk-error-summary" role="alert">
                    <div class="govuk-error-summary__body">
                      <p>${data.message}</p>
                    </div>
                  </div>`;
              }

              // Clear inputs after submission
              document.getElementById("data-key").value = "";
              document.getElementById("data-value").value = "";

              // Reload dashboard to show updated data
              window.location.reload(); // Refresh the page to show updated data
            } catch (error) {
              console.error("Failed to add data:", error);
              document.getElementById("status-message").innerHTML = `<div class="govuk-error-summary" role="alert">
                  <div class="govuk-error-summary__body">
                    <p>Failed to add data. Please try again later.</p>
                  </div>
                </div>`;
            }
          }
        </script>
      </main>
    </div>

    <!-- Footer -->
    <footer class="govuk-footer" role="contentinfo">
      <div class="govuk-width-container">
        <div class="govuk-footer__meta">
          <ul class="govuk-footer__inline-list">
            <li class="govuk-footer__inline-list-item">
              <a href="/accessibility" class="govuk-footer__link">Accessibility</a>
            </li>
            <li class="govuk-footer__inline-list-item">
              <a href="/cookies" class="govuk-footer__link">Cookies</a>
            </li>
            <li class="govuk-footer__inline-list-item">
              <a href="/privacy" class="govuk-footer__link">Privacy</a>
            </li>
            <li class="govuk-footer__inline-list-item">
              <a href="/terms" class="govuk-footer__link">Terms</a>
            </li>
          </ul>
          <span class="govuk-footer__licence-description">
            All content is available under the
            <a href="https://www.nationalarchives.gov.uk/doc/open-government-licence/version/3/" class="govuk-footer__link">Open Government Licence v3.0</a>, except where otherwise stated.
          </span>
        </div>
      </div>
    </footer>

    <!-- JavaScript -->
    <script src="js/page-complete.js" defer></script>
    <script src="js/device-profile.js" defer></script>
  </body>
</html>
)html";

  auto Response = drogon::HttpResponse::newHttpResponse();
  Response->setBody(OutputHtml);

  Response->setContentTypeCode(drogon::CT_TEXT_HTML);
  callback(Response);
}
/*

 */
