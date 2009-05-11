# before typing make, run this in your terminal:
help:
	@echo "how to use make:"
	@echo "   shell> export TEXMFCNF=/remote/pclg-05/jingliu/usr/share/texmf/web2c"
	@echo "   shell> make       # create .dvi"
	@echo "   shell> make ps    # create .ps"
	@echo "   shell> make pdf   # create .pdf"
	@echo "   shell> make clean # delete temporary files"

latex:
	latex memo
ps:
	dvips memo.dvi -o memo.ps
pdf:
	pdflatex -shell-escape memo
clean:
	rm -f *.dvi memo.pdf *.bak *.pstex* *.aux *.toc *.log *.out *.ps
