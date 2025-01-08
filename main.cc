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
  LoadUsers(std::filesystem::current_path() / "data");
  std::println("Entering server");

  drogon::app()
      .loadConfigFile("./config.json")
      .setDocumentRoot("./static")
      .setUploadPath("./uploads")
      .addListener("0.0.0.0", 8080)
      .setLogPath("./")
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
