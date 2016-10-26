gcc cs481_hw1.c -o cs481_hw1 -lm -O3

for var in 1 2 3 4 5
do
	echo Computing for Sample$var
	./cs481_hw1 samples/sample$var/text samples/sample$var/pattern
done
