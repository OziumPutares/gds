#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <json/value.h>

#include <format>
#include <string>

/**
 * @brief Creates a JSON response for contact operations
 * @param isSuccessful Boolean indicating if the operation was successful
 * @param message Message to be included in the response
 * @return Json::Value containing the response
 * @note This function creates a standardized JSON response format for
 * contact-related operations
 */
inline auto CreateContactJson(bool isSuccessful,
                              std::string const &message) -> Json::Value {
  Json::Value Result;
  Result["success"] = isSuccessful;
  Result["message"] = message;
  return Result;
}

/**
 * @brief Creates a JSON response for error scenarios
 * @param errorMessage String containing the error message
 * @return Json::Value containing the error response
 * @note This is a convenience wrapper around CreateContactJson for error cases
 */
inline auto CreateErrorJson(std::string const &errorMessage) -> Json::Value {
  return CreateContactJson(false, errorMessage);
}

/**
 * @brief Sends a JSON response to the client
 * @param Result The JSON response to send
 * @param callback The callback function to handle the HTTP response
 * @note This function handles the actual sending of the response and error
 * logging
 */
inline void ContactClient(
    Json::Value const &Result,
    std::function<void(drogon::HttpResponsePtr const &)> &callback) {
  try {
    LOG_DEBUG << (std::format("Sending response: {}", Result.toStyledString()));
    callback(drogon::HttpResponse::newHttpJsonResponse(Result));
  } catch (std::exception &Error) {
    LOG_DEBUG << (std::format("Error sending response: {}", Error.what()));
  }
}

/**
 * @brief Sends an error message to the client
 * @param errorMessage The error message to send
 * @param callback The callback function to handle the HTTP response
 * @note This is a convenience wrapper combining CreateErrorJson and
 * ContactClient
 */
inline void ContactClientAboutError(
    std::string const &errorMessage,
    std::function<void(drogon::HttpResponsePtr const &)> &callback) {
  ContactClient(CreateErrorJson(errorMessage), callback);
}
