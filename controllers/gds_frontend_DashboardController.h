#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

namespace gds::frontend {
class DashboardController : public drogon::HttpController<DashboardController> {
 public:
  METHOD_LIST_BEGIN
  // use METHOD_ADD to add your custom processing function here;
  // METHOD_ADD(DashboardController::get, "/{2}/{1}", Get); // path is
  // /gds/frontend/DashboardController/{arg2}/{arg1}
  // METHOD_ADD(DashboardController::your_method_name, "/{1}/{2}/list", Get); //
  // path is /gds/frontend/DashboardController/{arg1}/{arg2}/list
  // ADD_METHOD_TO(DashboardController::your_method_name,
  // "/absolute/path/{1}/{2}/list", Get); // path is
  // /absolute/path/{arg1}/{arg2}/list
  ADD_METHOD_TO(DashboardController::AddData, "/api/addData", drogon::Post,
                drogon::Get);

  METHOD_LIST_END
  auto AddData(drogon::HttpRequestPtr const &req,
               std::function<void(drogon::HttpResponsePtr const &)> &&callback)
      -> void;
};
}  // namespace gds::frontend
