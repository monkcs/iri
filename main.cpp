#include "argument.hpp"
#include "delimiter.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

static void parser(std::string_view document, const iri::delimiter symbols, const std::span<const std::filesystem::path> files)
{
	auto handle_file = [&files](const std::filesystem::path& target)
	{
		auto predicate = [&target](const auto& element) noexcept { return element.filename() == target; };

		if (const auto match = std::ranges::find_if(files, predicate); match == files.end())
		{
			throw std::runtime_error {std::format("missing include of file '{}'", target.c_str())};
		}
		else
		{
			std::cout << std::ifstream {*match, std::ios::binary}.rdbuf();
		}
	};

	auto handle_comment = [&symbols, handle_file](const std::string_view comment)
	{
		auto directive = comment;
		directive.remove_prefix(symbols.opening().size());
		directive.remove_suffix(symbols.closing().size());

		constexpr std::string_view magic_number = " include ";

		if (directive.starts_with(magic_number))
		{
			directive.remove_prefix(magic_number.size());

			if (directive.ends_with(' '))
			{
				directive.remove_suffix(1);
			}

			handle_file(directive);
		}
		else
		{
			std::print("{}", comment);
		}
	};

	auto consume_until = [&document] [[nodiscard]] (const std::size_t offset) noexcept
	{
		const auto prefix = document.substr(0, offset);
		document.remove_prefix(offset);
		return prefix;
	};

	while (not document.empty())
	{
		if (const auto position = document.find(symbols.opening()); position == document.npos)
		{
			std::print("{}", document);
			return;
		}
		else if (position > 0)
		{
			std::print("{}", consume_until(position));
		}

		if (const auto position = document.find(symbols.closing(), symbols.opening().size()); position == document.npos)
		{
			throw std::runtime_error {"missing end delimiter symbol"};
		}
		else
		{
			handle_comment(consume_until(position + symbols.closing().size()));
		}
	}
}
auto main(const int argc, char const* const argv[]) -> int
{
	try
	{
		const utilities::arguments arguments {argc, argv};

		const auto media_type = arguments.option<iri::delimiter, 1>("media-type");
		const auto included_files = arguments.option<std::filesystem::path>("include");

		const auto base_document = std::string {std::istreambuf_iterator<char>(std::cin), {}};

		parser(base_document, media_type, included_files);
	}
	catch (const std::runtime_error& error)
	{
		std::println(std::cerr, "iri: {}", error.what());
		return 1;
	}
	catch (const std::invalid_argument& error)
	{
		std::println(std::cerr, "iri: {}", error.what());
		return 1;
	}
	catch (...)
	{
		return 1;
	}
}