#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <json/value.h>

#include <iostream>
#include <string>
#include <utility>

inline auto CreateContactJson(bool isSuccessful,
                              std::string const &message) -> Json::Value {
  Json::Value Result;
  Result["success"] = isSuccessful;
  Result["message"] = message;
  return Result;
}
inline auto CreateErrorJson(std::string const &errorMessage) -> Json::Value {
  return CreateContactJson(false, errorMessage);
}

inline void ContactClient(
    Json::Value const &Result,
    std::function<void(drogon::HttpResponsePtr const &)> &callback) {
  try {
    std::cout << Result;
    callback(drogon::HttpResponse::newHttpJsonResponse(Result));
  } catch (std::exception &Error) {
    std::cerr << Error.what();
  }
}
inline void ContactClientAboutError(
    std::string const &errorMessage,
    std::function<void(drogon::HttpResponsePtr const &)> &callback) {
  ContactClient(CreateErrorJson(errorMessage), callback);
}
