.PHONY: test doc check

test: qwedom.cpp qwelist.cpp
	g++ -g3 -I. $< -o $@

doc: Doxyfile
	doxygen $<

check: test
