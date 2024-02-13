#!/bin/bash

dir="tests"
output="$dir/test_output"
grep_out="$output/grep_output.txt"
s21_out="$output/s21_grep_output.txt"
log="$output/cmp.log"

cur_time=$(date "+%Y-%m-%d %H:%M:%S")
echo "------------------------------------ $cur_time ------------------------------------" >> $log

flags=(i v c l n s h o)
all_num_flags=${#flags[@]}
success_rate=0
failure_rate=0
leak_rate=0
num_cycles=100

all_files=()
for file in "$dir"/*
	do
	if [ -f "$file" ]
		then
		all_files+=( "$file" )
	fi
done
all_num_files=${#all_files[@]}

patterns=("BuT" "line" "but" "hey" "hi" "notfound" "[g]r" "Prince Andrew" "[:alpha:]")
all_num_pats=${#patterns[@]}
pat_dir="$dir/patterns"
pat_files=()

for file in "$pat_dir"/*
	do
	if [ -f "$file" ]
		then
		pat_files+=( "$file" )
	fi
done
all_num_pat_files=${#pat_files[@]}


for ((cycle = 0; cycle < num_cycles; cycle++)); do
	cur_files=()
	cur_num_files=$(( 1 + (RANDOM * cycle) % all_num_files ))
	for ((i = 0; i < cur_num_files; i++)); do
		file_ind=$(( (RANDOM * cycle) % all_num_files ))
		cur_files+=("${all_files[file_ind]}")
	done
	e_pats=""
	num_pats=$(((RANDOM * cycle) % all_num_pats))
	for ((i = 0; i < num_pats; i++)); do
		pattern_ind=$(((RANDOM * cycle) % all_num_pats))
		pattern=${patterns[pattern_ind]}
		e_pats+="-e \"$pattern\" "
	done
	f_pats=""
	num_pat_files=$(((RANDOM * cycle) % all_num_pat_files))
	for ((i = 0; i < num_pat_files; i++)); do
		pat_file_ind=$(((RANDOM * cycle) % all_num_pat_files))
		pat_file=${pat_files[pat_file_ind]}
		f_pats+=" -f \"$pat_file\""
	done
	if (( num_pats == 0 && num_pat_files == 0 )); then
		pattern_ind=$(((RANDOM * cycle) % all_num_pats))
		pattern=\"${patterns[pattern_ind]}\"
	else
		if (( num_pat_files == 0 )); then
			pattern=$e_pats
		else
			pattern=$f_pats
		fi
	fi
	

	num_flags=$(((RANDOM * cycle) % all_num_flags))
	if (( num_flags == 0 )); then
		grep "$pattern" "${cur_files[@]}" > $grep_out
		./s21_grep "$pattern" "${cur_files[@]}" > $s21_out
		# echo "grep $pattern ${cur_files[*]} > $grep_out" >> $log #uncomment for verbosity

		if [ "$1" == "leaks" ] && ! leaks -quiet -atExit -- ./s21_grep "$pattern" "${cur_files[@]}" > /dev/null 2>&1; then
			echo "leaked with ./s21_grep $pattern ${cur_files[*]}" >> $log
			((leak_rate++))
		fi
		
	else
		flag_combo="-"
		for ((i = 0; i < num_flags; i++)); do
			flag_ind=$(((RANDOM * cycle) % ${#flags[@]}))
			flag_combo+=${flags[$flag_ind]}
		done
		grep "$flag_combo" "$pattern" "${cur_files[@]}" > $grep_out
		# echo "grep $flag_combo $pattern ${cur_files[*]} > $grep_out" >> $log #uncomment for verbosity
		./s21_grep "$flag_combo" "$pattern" "${cur_files[@]}" > $s21_out
		
		if [ "$1" == "leaks" ] && ! leaks -quiet -atExit -- ./s21_grep "$flag_combo" "$pattern" "${cur_files[@]}" > /dev/null 2>&1; then
			echo "leaked with ./s21_grep $flag_combo $pattern ${cur_files[*]}" >> $log
			((leak_rate++))
		fi
	fi
	
	

	if cmp $grep_out $s21_out >> $log 2>>$log; then
		((success_rate++))
	else
		echo "${cur_files[*]} have differences with $pattern pattern and $flag_combo flags" >> $log
		((failure_rate++))
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
if (( failure_rate == 0)); then
	echo -e "${green_colour}No mistakes were found${no_colour}"
fi

# debug_file="warm_and_piss.txt"
# debug_pat="Prince Andrew"
# grep -io "$debug_pat" "$dir/$debug_file" > $grep_out
# ./s21_grep -io "$debug_pat" "$dir/$debug_file" > $s21_out
# cmp $grep_out $s21_out >> $log 2>>$log