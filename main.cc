#include <drogon/drogon.h>
#include <json/value.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <print>

#include "include/UserControls.hpp"

auto main() -> int {
  // Configure Drogon app
  std::filesystem::create_directory(std::filesystem::current_path() / "data");
  std::println("Entering server");
  gds::users::LoadUsers(std::filesystem::current_path() / "data");

  Json::Value User{};
  User["username"] = "demo";
  User["hashedPassword"] =
      "c6935dba6cb8add354ba421a202166c36845a612fe7cc5e2d891e5e52ad109a3";
  User["salt"] = "demoOfMixedSalt";
  User["data"] = {};
  gds::users::GetUsers().emplace_back(User);
  // Start app
  drogon::app()
      .loadConfigFile("./config.json")
      .setDocumentRoot("./static")
      .setUploadPath("./uploads")
      .addListener("0.0.0.0", 8080)
      .setLogPath("./logs")
      .setLogLevel(
          trantor::Logger::kDebug)  // Set to Debug for more information
      .enableSession(1200)          // Enable sessions with 20 minute timeout
      .registerPostHandlingAdvice([](drogon::HttpRequestPtr const &req,
                                     drogon::HttpResponsePtr const &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
      })
      .run();

  return 0;
}
