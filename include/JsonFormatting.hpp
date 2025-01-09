
#include <json/value.h>

#include <format>
#include <sstream>
template <>
class std::formatter<Json::Value> {
 public:
  constexpr auto parse(format_parse_context& ctx) {  // NOLINT
    return ctx.begin();
  }
  template <typename Context>  // NOLINTNEXTLINE
  constexpr auto format(Json::Value const& user, Context& ctx) const {
    auto Stringstream = std::stringstream{} << user;
    return format_to(ctx.out(), "({})", Stringstream.str());
  }
};
