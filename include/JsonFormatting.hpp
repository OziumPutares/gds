#include <json/value.h>

#include <format>
#include <sstream>

/**
 * @brief Formatter specialization for Json::Value
 * @note Enables std::format support for Json::Value objects
 */
template <>
class std::formatter<Json::Value> {
 public:
  /**
   * @brief Parse formatting specification
   * @param ctx Format parse context
   * @return Iterator to end of parsed range
   */
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  /**
   * @brief Format Json::Value object
   * @param user Json::Value to format
   * @param ctx Formatting context
   * @return Iterator to end of formatted range
   * @note Uses stringstream for JSON serialization
   */
  template <typename Context>
  constexpr auto format(Json::Value const& user, Context& ctx) const {
    auto Stringstream = std::stringstream{} << user;
    return format_to(ctx.out(), "({})", Stringstream.str());
  }
};
