#NAME: Conner Yang
#EMAIL: conneryang@g.ucla.edu
#ID: 905417287


default:
	gcc -Wall -Wextra -g -o lab0 lab0.c

clean:
	rm -f *.txt *.tar.gz *.o lab0

dist:
	tar -cvzf lab0-905417287.tar.gz lab0.c Makefile README backtrace.png breakpoint.png

check: default stdCopyTest fdRedirectionTest segmentationTest catchTest invalidOptionsTest

stdCopyTest:
	@echo "test" > in.txt; \
	./lab0 < in.txt > out.txt; \
	if [[ $? -eq 0 ]] ; then \
		@echo "Correct exit status for stdin > stdout"; \
	else \
		@echo "Incorrect exit status for stdin > stdout"; \
	fi; \

	diff -s in.txt out.txt ; /
	@rm -f in.txt out.txt

fdRedirectionTest:
	@echo "test" > in.txt; \
	./lab0 --input in.txt --output out.txt; \
	if [[ $? -eq 0 ]]; then \
		@echo "Correct exit status for file descriptor redirection"; \
	else \
		@echo "Incorrect exit status for file descriptor redirection"; \
	fi; \

	diff -s in.txt out.txt ; /
	@rm -f in.txt out.txt

segmentationTest:
	./lab0 --segfault; \
	if [[ $? -eq 3 ]]; then \
		@echo "Correct Segmentation fault exit status"; \
	else \
		@echo "Incorrect Segmentation fault exit status"; \
	fi;

catchTest:
	./lab0 --catch; \
	if [[ $? -eq 4 ]]; then \
		@echo "Correct catch exit status"; \
	else \
		@echo "Incorrect catch exit status"; \
	fi

invalidOptionsTest:
	./lab0 --badoption; \
	if [[ $? -eq 1 ]]; then \
		@echo "Correct exit status for incorrect option"; \
	else \
		@echo "Incorrect exit status for incorrect option"; \
	fi
