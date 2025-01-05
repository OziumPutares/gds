#pragma once

#include <drogon/HttpSimpleController.h>

class DashboardController
    : public drogon::HttpSimpleController<DashboardController> {
 public:
  void asyncHandleHttpRequest(
      drogon::HttpRequestPtr const &req,
      std::function<void(drogon::HttpResponsePtr const &)> &&callback) override;
  PATH_LIST_BEGIN
  // list path definitions here;
  // PATH_ADD("/path", "filter1", "filter2", HttpMethod1, HttpMethod2...);
  PATH_ADD("/dashboard");
  PATH_LIST_END
};
