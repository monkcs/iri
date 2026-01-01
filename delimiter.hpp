#pragma once

#include <stdexcept>
#include <string_view>

namespace iri
{
	class delimiter
	{
		std::string_view begin;
		std::string_view end;

	  public:
		[[nodiscard]] consteval delimiter(const std::string_view begin, const std::string_view end) noexcept : begin {begin}, end {end} { };
		[[nodiscard]] constexpr delimiter(const std::string_view media_type) : delimiter {parser(media_type)} { };

		[[nodiscard]] constexpr bool operator==(const delimiter&) const noexcept = default;

		[[nodiscard]] constexpr auto opening() const noexcept { return begin; }
		[[nodiscard]] constexpr auto closing() const noexcept { return end; }

	  private:
		[[nodiscard]] static constexpr auto parser(const std::string_view media_type) -> delimiter
		{
			if (media_type == "text/html" or media_type == "text/xml")
			{
				return {"<!--", "-->"};
			}

			if (media_type == "text/css" or media_type == "text/javascript")
			{
				return {"/*", "*/"};
			}

			throw std::invalid_argument {"delimiter: media type not supported"};
		}
	};
}