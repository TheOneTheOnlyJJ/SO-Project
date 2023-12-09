#!/bin/sh

# Check argument count
if ! test $# -eq 1
then
	echo "Usage: ./$0 <c>"
	exit 1
fi

# Check if argument is alpha
is_alpha=`echo $1 | grep "^[a-zA-Z0-9]$"`
if test -z "$is_alpha"
then
	echo "Argument must be an alphanumeric character!"
	exit 1
fi

# Count correct sentences
no_correct_sentences=0
while read line
do
	line_pattern=`echo $line | grep -E  "^[A-Z][a-zA-Z0-9 ,]*[.!?]$" | grep -E -v ", ?si" | grep $1`
	if ! test -z "$line_pattern"
	then
		no_correct_sentences=`expr $no_correct_sentences + 1`	
	fi
done

# Output number of correct sentences
echo "$no_correct_sentences"

exit 0