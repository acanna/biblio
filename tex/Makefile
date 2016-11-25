all:
	../bin/main ../articles/test_[1-9].pdf
	pdflatex biblio.tex
	bibtex biblio
	pdflatex biblio.tex
	pdflatex biblio.tex


clean:
	rm -rf *.bib *.pdf *.log *.aux *.bbl *.html *.big *.out *.blg
