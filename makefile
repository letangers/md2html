.PHONY: clean
clean:
	rm *.o

t:main.cpp md2html.cpp
	g++ -o $@ $^
