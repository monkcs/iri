#include "argument.hpp"
#include "delimiter.hpp"
#include "parser.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <generator>
#include <iostream>
#include <print>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

auto main(const int argc, char const* const argv[]) -> int
{
	try
	{
		const utilities::arguments arguments {argc, argv};

		const auto media_type = arguments.option<iri::delimiter_pair, 1>("media-type");
		const auto included_files = arguments.option<std::filesystem::path>("include");

		for (const auto&& [content_type, content] : iri::parse(media_type))
		{
			switch (content_type)
			{
				using enum iri::type;

				case text:
				{
					std::print("{}", content);
					continue;
				}

				case directive:
				{
					auto predicate = [&](const auto& element) noexcept { return element.filename() == content; };

					if (const auto match = std::ranges::find_if(included_files, predicate); match == included_files.end())
					{
						throw std::runtime_error {std::format("missing include of file '{}'", content)};
					}
					else
					{
						std::cout << std::ifstream {*match, std::ios::binary}.rdbuf();
					}

					continue;
				}

				default:
				{
					throw std::logic_error {"charlie messed up"};
				}
			}
		}
	}
	catch (const std::runtime_error& error)
	{
		std::println(std::cerr, "iri: error: {}", error.what());
		return 1;
	}
	catch (const std::logic_error& error)
	{
		std::println(std::cerr, "iri: logic_error: {}", error.what());
		return 1;
	}
	catch (...)
	{
		std::println(std::cerr, "iri: error: uncaught exception");
		throw;
	}
}