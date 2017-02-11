CXX := g++
CXXFLAGS := -Wall -O2 -std=c++11

TARGET := asm
OBJS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))
TESTS := $(patsubst %.s, %, $(wildcard test/*.s))


$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)


.PRECIOUS: test/%.zoi
test/%.zoi: test/%.s $(TARGET)
	./$(TARGET) -no-debug $<

.PHONY: test/%
test/%: test/%.exp.zoi test/%.zoi
	cmp $^

.PHONY: test
test: $(TESTS)
	@echo "\033[32mPASSED!\033[m"


.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -f $(TESTS:%=%.zoi)

