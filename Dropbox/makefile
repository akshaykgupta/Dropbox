linkers=-L/usr/lib -lssl -lcrypto -lsqlite3 -lboost_system -lboost_filesystem
warnings=-w -Wall
compiler=g++
src=SRC/SERVER/
srcc=SRC/CLIENT/
obj=OBJ/
exe=EXE/
doc=Documents/
tex=Tex\ Documents/

allserver:$(obj)server.o $(obj)servermain.o $(obj)UserDb.o $(obj)LoginDb.o
	$(compiler) $(warnings) $(obj)server.o $(obj)servermain.o $(obj)UserDb.o $(obj)LoginDb.o $(linkers) -o server
	mv server $(exe)server
allclient:
	cd $(srcc) ; make -f mymakefile 
	mv $(srcc)Mydropbox $(exe)Mydropbox
execute:$(exe)server
	./$<
$(obj)servermain.o:$(src)servermain.cpp
	$(compiler) -c $<
	mv servermain.o $@
$(obj)server.o:$(src)server.cpp $(src)server.h
	$(compiler) $(warnings) -c $<
	mv server.o $@
$(obj)UserDb.o:$(src)UserDb.cpp $(UserDb.h)
	$(compiler) $(warnings) -c $<
	mv UserDb.o $@
$(obj)LoginDb.o:$(src)LoginDb.cpp $(LoginDb.h)
	$(compiler) $(warnings) -c $<
	mv LoginDb.o $@
$(exe)server:
	make all
doc:$(tex)Design\ Document.tex
	cd $(tex) ; pdflatex Design\ Document.tex
	mv $(tex)Design\ Document.pdf $(doc)Design\ Document.pdf
	$(tex)Changes.tex $(tex)Design\ Document.tex
	pdflatex $(tex)changes
	pdflatex $(tex)Design\ Document.tex
	rm Changes.aux
	rm Changes.log
	rm -rf $(tex)Design\ Document.aux
	rm -rf $(tex)Design\ Document.log
clean:
	rm $(exe)server
	rm $(obj)server.o $(obj)servermain.o $(obj)UserDb.o $(obj)LoginDb.o