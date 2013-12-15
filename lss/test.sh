gcc -Werror -Wall -Wshadow -O3 sorting.c sorting_main.c -o proj2

echo proj2 1000.txt out_1000.txt
proj2 1000.txt out_1000.txt
echo
echo proj2 10000.txt out_10000.txt
proj2 10000.txt out_10000.txt
echo
echo proj2 100000.txt out_100000.txt
proj2 100000.txt out_100000.txt
echo
echo proj2 1000000.txt out_100000.txt
proj2 1000000.txt out_100000.txt
