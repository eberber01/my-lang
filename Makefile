
OUT= my-lang


all:
	gcc main.c lex.c parse.c ast.c asm.c util.c symtab.c -o $(OUT)

clean:
	rm  $(OUT)