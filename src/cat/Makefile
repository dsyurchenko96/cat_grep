CFLAGS = -Wall -Wextra -Werror -std=c11
CAT = s21_cat
ASCII = generate_ascii

all: $(CAT)

$(CAT): $(CAT).c $(CAT).h
	gcc $(CFLAGS) $(CAT).c -o $(CAT)

clean:
	rm -rf *.o $(CAT) $(ASCII) *.out

$(ASCII): $(ASCII).c
	gcc $(CFLAGS) $(ASCII).c -o $(ASCII)

test: $(CAT) $(ASCII)
	touch tests/auto_test_{1..100}.txt
	./$(ASCII)
	./test.sh

test_with_leaks: $(CAT) $(ASCII)
	touch tests/auto_test_{1..100}.txt
	./$(ASCII)
	./test.sh leaks

rebuild: clean all

cpp: 
	cppcheck --enable=all --suppress=missingIncludeSystem .

clang:
	clang-format -i *.c

.PHONY: all clean rebuild cpp clang test test_with_leaks