#include "gds_frontend_DashboardController.h"

#include <json/value.h>

#include <algorithm>
#include <filesystem>
#include <format>
#include <iterator>
#include <ostream>
#include <string>

#include "../include/AsynchronousContainerAbstraction.hpp"
#include "../include/NotifyOfError.hpp"
#include "../include/ScopeGuard.hpp"
#include "../include/UserControls.hpp"

/**
 * @brief Handle adding data should only be called by drogon
 * @note This function is multiple-thread safe.
 */
auto gds::frontend::DashboardController::AddData(
    drogon::HttpRequestPtr const &req,
    std::function<void(drogon::HttpResponsePtr const &)> &&callback) -> void {
  Json::Value Result;
  auto JsonBody = req->getJsonObject();
  auto Session = req->getSession();

  std::cout << "Adding data\n";
  if (!JsonBody) {
    ContactClientAboutError(
        std::format("Invalid json see error message{}", req->getJsonError()),
        callback);
    return;
  }
  try {
    auto Key = (*JsonBody)["key"].asString();
    auto Value = (*JsonBody)["value"].asString();
    if (!Session->getOptional<bool>("logged_in")) {
      ContactClientAboutError("Must be logged in", callback);
    }

    auto Username = Session->getOptional<std::string>("username");
    if (!Username) {
      ContactClientAboutError("Invalid username", callback);
      return;
    }
    auto &Users = gds::users::GetUsers();
    auto UserToEdit =
        std::find_if(std::begin(Users), std::end(Users),  // NOLINT
                     [&Username](StoredUser User) {
                       return User.GetUsernameThrows() == Username;
                     });
    if (UserToEdit == std::end(Users)) {
      ContactClientAboutError("Server error no such username found", callback);
      return;
    }
    StoredUser Tmp = *UserToEdit;
    Tmp.m_Data["data"][Key] = Value;
    *UserToEdit = Tmp;
    Users.erase(UserToEdit);
    Users.push_back(Tmp);
    // std::println("{}", static_cast<StoredUser>(*UserToEdit));
    Session->erase("data");
    Session->insert("data", static_cast<StoredUser>(*UserToEdit).GetData());
    // std::println("{}", Session->get<Json::Value>("data"));
    gds::users::SaveUsers(std::filesystem::current_path() / "data");

  } catch (Json::Exception &Error) {
    ContactClientAboutError(
        std::format("Invalid json state see error {}", Error.what()), callback);
    return;
  }
  ContactClient(CreateContactJson(true, "Creation successful"), callback);
}
