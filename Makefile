lexer: lex.yy.o stringutils.o errorutils.o
	gcc -o lexer lex.yy.o stringutils.o errorutils.o

lex.yy.o: lex.yy.c
	gcc -c -o lex.yy.o lex.yy.c

lex.yy.c: lexer.l
	flex lexer.l

stringutils.o: stringutils.c
	gcc -c -o stringutils.o stringutils.c

errorutils.o: errorutils.c
	gcc -c -o errorutils.o errorutils.c

clean:
	rm -f lex.yy.c lexer *.o