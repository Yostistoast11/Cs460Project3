P3.out : Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o Object.o
	g++ -g -o P3.out Project2.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o Object.o

Project2.o : Project2.cpp SetLimits.h SyntacticalAnalyzer.h
	g++ -g -c Project2.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c SetLimits.cpp

LexicalAnalyzer.o : LexicalAnalyzer.cpp LexicalAnalyzer.h
	g++ -g -c LexicalAnalyzer.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h Object.h CodeGenerator.h
	g++ -g -c SyntacticalAnalyzer.cpp

Object.o : Object.cpp Object.h
	g++ -g -c Object.cpp

CodeGen.o : CodeGenerator.cpp CodeGenerator.h
	g++ -g -c CodeGenerator.cpp

clean : 
	rm *.o P3.out *.gch *.lst *.dbg *.p1 *.p2

submit : Project2.cpp LexicalAnalyzer.h LexicalAnalyzer.cpp SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp makefile README.txt
	rm -rf Team2P3
	mkdir Team2P3
	cp Project2.cpp Team2P3
	cp LexicalAnalyzer.h Team2P3
	cp LexicalAnalyzer.cpp Team2P3
	cp SyntacticalAnalyzer.h Team2P3
	cp SyntacticalAnalyzer.cpp Team2P3
	cp Object.h Team2P3
	cp Object.cpp Team2P3
	cp CodeGenerator.h Team2P3
	cp CodeGenerator.cpp Team2P3
	cp makefile Team2P3
	cp README.txt Team2P3
	tar cfvz TeamZP2.tgz Team2P3
	cp Team2P3.tgz ~tiawatts/cs460drop
