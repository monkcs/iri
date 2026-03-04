#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace iri
{
	class delimiter
	{
		std::string begin;
		std::string end;

	  public:
		[[nodiscard]] constexpr delimiter(const std::string_view media_type) : delimiter {parse(media_type)} { };
		[[nodiscard]] constexpr delimiter(std::string begin, std::string end) : begin {std::move(begin)}, end {std::move(end)}
		{
			if (opening().empty() or closing().empty())
			{
				throw std::invalid_argument {"delimiter: delimiters cannot be empty"};
			}
		};

		[[nodiscard]] constexpr bool operator==(const delimiter&) const noexcept = default;

		[[nodiscard]] constexpr auto opening() const& noexcept -> const std::string& { return begin; }
		[[nodiscard]] constexpr auto closing() const& noexcept -> const std::string& { return end; }

		[[nodiscard]] constexpr auto opening() && noexcept -> std::string { return std::move(begin); }
		[[nodiscard]] constexpr auto closing() && noexcept -> std::string { return std::move(end); }

	  private:
		[[nodiscard]] static constexpr auto parse(const std::string_view media_type) -> delimiter
		{
			if (media_type == "text/html" or media_type == "text/xml")
			{
				return {"<!-- include ", " -->"};
			}

			if (media_type == "text/css" or media_type == "text/javascript")
			{
				return {"/* include ", " */"};
			}

			throw std::invalid_argument {"delimiter: media type not supported"};
		}
	};
}