
OUT= my-lang


all:
	gcc main.c lex.c parse.c ast.c -o $(OUT)

clean:
	rm  $(OUT)