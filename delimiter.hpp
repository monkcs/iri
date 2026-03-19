#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace iri
{
	class delimiter
	{
		std::string value;

	  public:
		[[nodiscard]] constexpr delimiter(std::string&& input) : value {std::move(input)}
		{
			if (value.empty())
			{
				throw std::invalid_argument {"delimiter: cannot be empty"};
			}
		}
		[[nodiscard]] constexpr delimiter(const std::string& input) : value {input}
		{
			if (value.empty())
			{
				throw std::invalid_argument {"delimiter: cannot be empty"};
			}
		}

		[[nodiscard]] constexpr bool operator==(const delimiter&) const noexcept = default;
		[[nodiscard]] constexpr operator std::string_view() const noexcept { return value; }

		[[nodiscard]] constexpr auto size() const noexcept { return value.size(); }

		[[nodiscard]] constexpr auto string() && noexcept -> std::string { return std::move(value); }
		[[nodiscard]] constexpr auto string() const& noexcept -> const std::string& { return value; }
	};

	class delimiter_pair
	{
		delimiter begin;
		delimiter end;

		[[nodiscard]] constexpr delimiter_pair(delimiter&& begin, delimiter&& end) noexcept :
			begin {std::move(begin)}, end {std::move(end)} { };
		[[nodiscard]] constexpr delimiter_pair(const delimiter& begin, const delimiter& end) : begin {begin}, end {end} { };

	  public:
		[[nodiscard]] constexpr delimiter_pair(const std::string_view media_type) : delimiter_pair {parse(media_type)} { };

		[[nodiscard]] constexpr bool operator==(const delimiter_pair&) const noexcept = default;

		[[nodiscard]] constexpr auto opening() && noexcept -> delimiter { return std::move(begin); }
		[[nodiscard]] constexpr auto closing() && noexcept -> delimiter { return std::move(end); }

		[[nodiscard]] constexpr auto opening() const& noexcept -> const delimiter& { return begin; }
		[[nodiscard]] constexpr auto closing() const& noexcept -> const delimiter& { return end; }

	  private:
		[[nodiscard]] static constexpr auto parse(const std::string_view media_type) -> delimiter_pair
		{
			using namespace std::literals::string_literals;

			if (media_type == "text/html" or media_type == "text/xml")
			{
				return {delimiter {"<!-- include "}, delimiter {" -->"}};
			}

			if (media_type == "text/css" or media_type == "text/javascript")
			{
				return {delimiter {"/* include "}, delimiter {" */"}};
			}

			throw std::invalid_argument {"delimiter: media type not supported"};
		}
	};
}