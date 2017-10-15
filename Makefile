CC=g++
LATEX=pdflatex

.PHONY: all clean bin doc

all: bin doc

bin:
	$(CC) src/main.cpp -o bin/sliding-window

doc:
	-$(LATEX) doc/report.tex -output-directory doc

clean:
	-rm bin/sliding-window
	-rm bin/sliding-window.exe
	-rm doc/report.aux
	-rm doc/report.log
	-rm doc/report.pdf