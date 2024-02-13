#!/bin/bash

dir="tests"
cat_out="$dir/test_output/cat_output.txt"
s21_out="$dir/test_output/s21_cat_output.txt"
log="$dir/test_output/cmp.log"

flags=(b e n s t v)
success_rate=0
failure_rate=0
leak_rate=0
cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo "------------------------------------ $cur_time ------------------------------------" >> $log

for file in "$dir"/*
	do
	if [ -f "$file" ]
		then
		flag_combo=""
		num_flags=$((1 + RANDOM % 6))
		for ((i = 0; i < num_flags; i++)); do
			flag_ind=$((RANDOM % 6))
			flag_combo+=${flags[$flag_ind]}
		done
		cat -"$flag_combo" "$file" > $cat_out
		./s21_cat -"$flag_combo" "$file" > $s21_out
		if [ "$1" == "leaks" ] && ! leaks -quiet -atExit -- ./s21_cat -"$flag_combo" "$file" > /dev/null 2>&1; then
			echo "leaked with ./s21_cat -$flag_combo $file" >> $log
			((leak_rate++))
		fi

		if cmp $cat_out $s21_out >> $log; then
			((success_rate++))
		else
			echo "$file" has differences with -"$flag_combo" flags >> $log
			((failure_rate++))
		fi

	fi
done

red_colour='\033[0;31m'
green_colour='\033[0;32m'
no_colour='\033[0m'

echo -e "${green_colour}SUCCESS${no_colour}: $success_rate cases"
echo -e "${red_colour}FAILURE${no_colour}: $failure_rate cases"
if [ "$1" == "leaks" ]; then
	echo -e "${red_colour}LEAKS${no_colour}: $leak_rate cases"
	if (( leak_rate == 0 )); then
		echo -e "${green_colour}No leaks were found${no_colour}"
	fi
fi
if (( failure_rate == 0 )); then
	echo -e "${green_colour}No mistakes were found${no_colour}"
fi

# debug_file="numbered_lines.txt"
# cat -bne "$dir/$debug_file" > $cat_out
# ./s21_cat -bne "$dir/$debug_file" > $s21_out
# cmp $cat_out $s21_out >> $log