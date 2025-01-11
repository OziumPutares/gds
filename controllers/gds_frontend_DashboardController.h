#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

namespace gds::frontend {

/**
 * @brief Controller for dashboard functionality
 * @note Handles dashboard data operations in a thread-safe manner
 */
class DashboardController : public drogon::HttpController<DashboardController> {
 public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(DashboardController::AddData, "/api/addData", drogon::Post,
                drogon::Get);
  METHOD_LIST_END

  /**
   * @brief Adds new data to user's dashboard
   * @param req HTTP request containing data to add
   * @param callback Callback to send HTTP response
   * @note Expects JSON with key and value fields
   * @note Requires active session with logged_in flag
   * @note Thread-safe implementation for concurrent access
   */
  auto AddData(drogon::HttpRequestPtr const& req,
               std::function<void(drogon::HttpResponsePtr const&)>&& callback)
      -> void;
};

}  // namespace gds::frontend
