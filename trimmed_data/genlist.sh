#!/bin/sh
num=$(find . -name "*.txt" | wc -l | tr -d ' ')
echo "Total:    $num"
num=$(find none -name "*.txt" | wc -l | tr -d ' ')
echo "None:     $num"
num=$(find unknown -name "*.txt" | wc -l | tr -d ' ')
echo "Unknown:  $num"
num=$(find seizures -name "*.txt" | wc -l | tr -d ' ')
echo "Seizures: $num"
ls none/*.txt > none.list
ls seizures/*.txt > seizures.list
