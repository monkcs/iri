#pragma once

#include <algorithm>
#include <concepts>
#include <format>
#include <initializer_list>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace utilities
{
	[[nodiscard]] constexpr auto operator+(const std::string_view first, const std::string_view second)
	{
		return std::string {first}.append(second);
	};

	class [[nodiscard]] arguments
	{
		std::vector<std::string> buffer;

	  public:
		[[nodiscard]] constexpr arguments(const std::vector<std::string>& raw) : buffer {raw} { }
		[[nodiscard]] constexpr arguments(std::vector<std::string>&& raw) noexcept : buffer {std::move(raw)} { }
		[[nodiscard]] constexpr arguments(const int argc, char const* const argv[]) :
			arguments {std::vector<std::string> {argv + 1, argv + argc}} { };

		[[nodiscard]] constexpr auto empty() const noexcept { return buffer.empty(); }

		[[nodiscard]] constexpr bool exist(const std::string_view name) const noexcept
		{
			const std::string option = "--" + name;
			return std::ranges::any_of(buffer, [&](const auto& element) { return element == option; });
		}

		[[nodiscard]] constexpr bool exist(const std::initializer_list<std::string_view> names) const noexcept
		{
			for (const auto& element : buffer)
			{
				for (const auto& name : names)
				{
					const std::string option = "--" + name;
					if (element == option)
					{
						return true;
					}
				}
			}
			return false;
		}

		template<int lenght>
		[[nodiscard]] constexpr auto option(const std::string_view name) const
		{
			return option<std::string, lenght>(name);
		}

		template<typename Type = std::string, int lenght = 0>
			requires std::constructible_from<Type, std::string_view>
		[[nodiscard]] constexpr auto option(const std::string_view name) const
		// noexcept(noexcept(std::is_nothrow_constructible<Type, std::string>::value))
		{
			static_assert(lenght >= 0, "argument lenght for option cannot be negative");

			if (name.empty())
			{
				throw std::logic_error {"option: name cannot be empty"};
			}
			else
			{
				if (const auto position = std::ranges::find(buffer, "--" + name); position == buffer.end())
				{
					throw std::runtime_error {std::format("option: '--{}' is required, but missing", name)};
				}
				else
				{
					auto option_name = [](const auto& element) { return element.starts_with("--"); };

					const std::span tail {position + 1, buffer.end()};
					const std::span<const std::string> arguments_list {tail.begin(), std::ranges::find_if(tail, option_name)};

					if constexpr (lenght == 0)
					{
						if (arguments_list.empty())
						{
							throw std::runtime_error {std::format("option: '--{}' requires arguments, but 0 are provided", name)};
						}
						else
						{
							return std::vector<Type> {arguments_list.begin(), arguments_list.end()};
						}
					}
					else if constexpr (lenght == 1)
					{
						if (arguments_list.size() == lenght)
						{
							return Type {arguments_list.front()};
						}
						else
						{
							throw std::runtime_error {
								std::format("option: '--{}' requires 1 argument, but {} are provided", name, arguments_list.size())};
						}
					}
					else
					{
						if (arguments_list.size() == lenght)
						{
							return std::vector<Type> {arguments_list.begin(), arguments_list.end()};
						}
						else
						{
							throw std::runtime_error {std::format("option: '--{}' requires {} arguments, but {} are provided", name, lenght,
																  arguments_list.size())};
						}
					}
				}
			}
		}
	};
}