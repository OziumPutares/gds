#include "AccountCreationController.h"

#include <drogon/HttpResponse.h>
#include <json/value.h>

#include <filesystem>
#include <fstream>

#include "../include/NotifyOfError.hpp"

// Add definition of your processing function here

void AccountCreationController::CreateAccount(
    drogon::HttpRequestPtr const &req,
    std::function<void(drogon::HttpResponsePtr const &)> &&callback) {
  Json::Value Result;
  auto JsonBody = req->getJsonObject();
  if (!JsonBody) {
    ContactClientAboutError("Invalid Json", callback);
  }
  std::string Username = (*JsonBody)["username"].asString();
  std::string HashedPassword = (*JsonBody)["hashedPassword"].asString();
  std::ofstream OutputFile(
      (std::filesystem::current_path() / "data/").string() + Username +
      ".data");
  OutputFile << ((Json::Value{}["username"] = Username)["hashedPassword"] =
                     HashedPassword)
                    .asString();
  ContactClient(CreateContactJson(true, "Account creation successful"),
                callback);
}
