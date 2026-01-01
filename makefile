.PHONY: install uninstall clean tidy 

WARNINGS = -Werror -Wall -Wextra -Wpedantic -pedantic-errors -Wnrvo -Wsign-conversion -Wdelete-non-virtual-dtor -Wredundant-move -Wpessimizing-move -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused

iri: makefile main.cpp *.hpp
	g++ -std=c++26 -O3 -march=native -fwhole-program -o $@ $(WARNINGS) main.cpp
	strip $@

install: iri
	cp --reflink=auto iri ~/.local/bin/

uninstall:
	rm -f ~/.local/bin/iri

clean: uninstall
	rm -f iri

tidy: main.cpp *.hpp
	clang-tidy -extra-arg=-std=c++26 -checks=clang-analyzer-*,modernize-*,misc-*,performance-*,readability-*,cppcoreguidelines-*,bugprone-* main.cpp *.hpp
