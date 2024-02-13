CFLAGS = -Wall -Wextra -Werror -std=c11
DEBUG_FLAGS = -g -fsanitize=address
GREP = s21_grep
TEST_DIR = ../../tests/grep_tests

all: $(GREP)

$(GREP): $(GREP).c $(GREP).h
	gcc $(CFLAGS) $(GREP).c -o $(GREP)

debug: $(GREP).c $(GREP).h
	gcc $(CFLAGS) $(DEBUG_FLAGS) $(GREP).c -o $(GREP)

test: $(GREP)
	$(TEST_DIR)/grep_test.sh

test_with_leaks: $(GREP)
	$(TEST_DIR)/grep_test.sh leaks

clean:
	rm -rf *.o $(GREP) *.out

rebuild: clean all

cpp: 
	cppcheck --enable=all --suppress=missingIncludeSystem .

clang:
	clang-format -i *.c

.PHONY: all clean rebuild cpp clang test test_with_leaks debug