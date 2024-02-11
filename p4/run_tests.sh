#!/bin/bash

# NOTE: THESE TESTS MUST BE PERFORMED WITH num_reducers SET TO 1 IN main.c 
# OTHERWISE, NON-DETERMINISM FROM MULTITHREADING WILL CAUSE THEM TO FAIL.

t() {
  ./client-wordcount tests/$1/in/*.txt > tests/$1/$1-out-actual.txt
  expected="tests/$1/$1-out-expected.txt"
  actual="tests/$1/$1-out-actual.txt"

  if cmp -s "$expected" "$actual"; then
      echo "Test $i PASS"
  else
      echo "TEST $i FAIL"
  fi
}

max=8
for (( i=1; i <= $max; i++))
do
	t $i
done
