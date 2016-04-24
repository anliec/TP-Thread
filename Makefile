default: help

help:
	@echo Useful targets:
	@echo "  small.txt medium.txt large.txt many.txt:  generate some input files "
	@echo "  question1 question2: compile your programs"
	@echo "  run1 run2:  run your programs through the 'time' utility"
	@echo "  clean:  delete all generated files"

#########################
# workload generation

tiny.txt:
	./generator.cs 20 20 0 > $@

small.txt:
	./generator.cs 20 32 0 > $@

medium.txt:
	./generator.cs 20 50 0 > $@

large.txt:
	./generator.cs 30 64 0 > $@

many.txt:
	./generator.cs 1000 50 75 > $@

#########################
## program compilation

question1: question1.c
	gcc -Wall -pthread -o question1 question1.c -lm 

question2: question2.c
	gcc -Wall -pthread -o question2 question2.c -lm 

question3: question3.c
	gcc -Wall -pthread -o question3 question3.c -lm 

question5: question5.c
	gcc -Wall -pthread -o question5 question5.c -lm
	
question6: question6.c
	gcc -Wall -pthread -o question6 question6.c -lm  

question10: question10.c
	gcc -Wall -pthread -Ofast -o question10 question10.c -lm 	
	
%:%.c
	gcc -Wall -pthread -o $<.run $< -lm

# add your own rules when you create new programs

#########################
## program execution

run1: question1
	time ./question1

run2: question2
	time ./question2

#########################
## utilities

clean:
	rm -f question1 question2 tiny.txt small.txt medium.txt large.txt many.txt 

