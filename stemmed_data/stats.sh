#!/bin/sh
echo "category_0"
num=$(find category_0 -name "*.txt" | wc -l | tr -d ' ')
echo "Total:    $num"
num=$(find category_0/none -name "*.txt" | wc -l | tr -d ' ')
echo "None:     $num"
num=$(find category_0/unknown -name "*.txt" | wc -l | tr -d ' ')
echo "Unknown:  $num"
num=$(find category_0/seizures -name "*.txt" | wc -l | tr -d ' ')
echo "Seizures: $num"
echo
echo "category_1"
num=$(find category_1 -name "*.txt" | wc -l | tr -d ' ')
echo "Total:    $num"
num=$(find category_1/none -name "*.txt" | wc -l | tr -d ' ')
echo "None:     $num"
num=$(find category_1/unknown -name "*.txt" | wc -l | tr -d ' ')
echo "Unknown:  $num"
num=$(find category_1/seizures -name "*.txt" | wc -l | tr -d ' ')
echo "Seizures: $num"
echo
echo "category_2"
num=$(find category_2 -name "*.txt" | wc -l | tr -d ' ')
echo "Total:    $num"
num=$(find category_2/none -name "*.txt" | wc -l | tr -d ' ')
echo "None:     $num"
num=$(find category_2/unknown -name "*.txt" | wc -l | tr -d ' ')
echo "Unknown:  $num"
num=$(find category_2/seizures -name "*.txt" | wc -l | tr -d ' ')
echo "Seizures: $num"
echo
echo "category_3"
num=$(find category_3 -name "*.txt" | wc -l | tr -d ' ')
echo "Total:    $num"
num=$(find category_3/none -name "*.txt" | wc -l | tr -d ' ')
echo "None:     $num"
num=$(find category_3/unknown -name "*.txt" | wc -l | tr -d ' ')
echo "Unknown:  $num"
num=$(find category_3/seizures -name "*.txt" | wc -l | tr -d ' ')
echo "Seizures: $num"
echo
echo "category_4"
num=$(find category_4 -name "*.txt" | wc -l | tr -d ' ')
echo "Total:    $num"
num=$(find category_4/none -name "*.txt" | wc -l | tr -d ' ')
echo "None:     $num"
num=$(find category_4/unknown -name "*.txt" | wc -l | tr -d ' ')
echo "Unknown:  $num"
num=$(find category_4/seizures -name "*.txt" | wc -l | tr -d ' ')
echo "Seizures: $num"
echo
echo "category_5"
num=$(find category_5 -name "*.txt" | wc -l | tr -d ' ')
echo "Total:    $num"
num=$(find category_5/none -name "*.txt" | wc -l | tr -d ' ')
echo "None:     $num"
num=$(find category_5/unknown -name "*.txt" | wc -l | tr -d ' ')
echo "Unknown:  $num"
num=$(find category_5/seizures -name "*.txt" | wc -l | tr -d ' ')
echo "Seizures: $num"
