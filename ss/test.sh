rm -r *seq.txt *output.txt

gcc -Werror -Wall -Wshadow -O3 sorting.c sorting_main.c -o proj1

#./proj1 1 i 1000.txt i1_1000seq.txt 1000outputi1.txt
#./proj1 1 i 10000.txt i1_10000seq.txt 10000output.txt
#./proj1 1 i 100000.txt i1_100000seq.txt 100000output.txt
echo ./proj1 1 i 1000000.txt 1000000seq.txt 1000000output.txt
./proj1 1 i 1000000.txt 1000000seq.txt 1000000output.txt

echo ./proj1 1 b 1000.txt b1_1000seq.txt b1_1000output.txt
./proj1 1 b 1000.txt b1_1000seq.txt b1_1000output.txt
#./proj1 1 b 10000.txt b1_10000seq.txt b1_10000output.txt
#./proj1 1 b 100000.txt b1_100000seq.txt b1_100000output.txt
#./proj1 1 b 1000000.txt 1000000seq.txt b1_1000000output.txt

#./proj1 2 i 1000.txt 2i_1000seq.txt 2i_1000output.txt
#./proj1 2 i 10000.txt 2i_10000seq.txt 2i_10000output.txt
echo ./proj1 2 i 100000.txt 2i_100000seq.txt 2i_100000output.txt
./proj1 2 i 100000.txt 2i_100000seq.txt 2i_100000output.txt
#./proj1 2 i 1000000.txt 2i_1000000seq.txt 2i_1000000output.txt

#./proj1 2 b 1000.txt 2b_1000seq.txt 2b_1000output.txt
echo ./proj1 2 i 100000.txt 2i_100000seq.txt 2i_100000output.txt
./proj1 2 b 10000.txt 2b_10000b2seq.txt 2b_10000output.txt
#./proj1 2 b 100000.txt 2b_100000b2seq.txt 2b_100000output.txt
#./proj1 2 b 1000000.txt 2b_1000000b2seq.txt 2b_1000000output.txt
