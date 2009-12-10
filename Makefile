test: qwedom.cpp qwelist.cpp
	g++ -g3 -I. $< -o $@

check: test
