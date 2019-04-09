.PHONY: clean
clean:
	rm *.o

t:test.cpp md2html.cpp
	g++ -o $@ $^
