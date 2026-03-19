#pragma once

#include "delimiter.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <generator>
#include <iostream>
#include <istream>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>

namespace iri
{
	enum class type
	{
		text,
		directive
	};

	struct value_type
	{
		iri::type content_type;
		std::string content;
	};

	struct reference_type
	{
		iri::type content_type;
		std::string_view content;

		[[nodiscard]] constexpr operator value_type() const { return value_type {content_type, std::string {content}}; }
	};

	inline auto read_line(std::istream& stream, const char newline_character = '\n') -> std::generator<std::string_view, std::string>
	{
		std::string buffer;
		while (std::getline(stream, buffer, newline_character))
		{
			co_yield buffer += newline_character;
		}
	}

	inline auto parse(const iri::delimiter_pair delimiters) -> std::generator<reference_type, value_type>
	{
		for (auto&& line : read_line(std::cin))
		{
			auto consume = [&line] [[nodiscard]] (const std::size_t offset) noexcept
			{
				const auto prefix = line.substr(0, offset);
				return line.remove_prefix(offset), prefix;
			};

			while (not line.empty())
			{
				if (const auto opening_offset = line.find(delimiters.opening()); opening_offset == std::string_view::npos)
				{
					co_yield {type::text, line};
					break;
				}
				else
				{
					if (opening_offset > 0)
					{
						co_yield {type::text, consume(opening_offset)};
					}

					line.remove_prefix(delimiters.opening().size());

					if (const auto closing_offset = line.find(delimiters.closing()); closing_offset == std::string_view::npos)
					{
						throw std::runtime_error {"missing end delimiter symbol"};
					}
					else
					{
						co_yield {type::directive, consume(closing_offset)};
						line.remove_prefix(delimiters.closing().size());
					}
				}
			}
		}
	}
}