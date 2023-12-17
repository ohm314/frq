
CXX=clang++

patience: main.cpp
	${CXX} -g -o $@ $^
