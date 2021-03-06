OUT_DIR := ./builds
OBJECTS := $(addprefix $(OUT_DIR)/, main.o myRegisterAllocation.o\
                                    myAaiBase.o myLiveness.o myCFGraph.o\
                                    myDirectedGraph.o myMovable.o myComputable.o\
                                    myControlable.o myMunch.o myCanonical.o\
                                    myCodeGen.o myFrame_X86.o myTranslate.o\
                                    myTemp.o mySymbol.o mySemantic.o myEnvironment.o\
                                    myIRTree.o makeMemory.o typeMaker.o myError.o\
                                    typeChecker.o recursiveDecs.o lValueTreater.o\
                                    myTable.o myEscape.o breakChecker.o forChecker.o\
                                    abstractSyntaxMaker.o frontTester.o breakGetter.o\
                                    lexString.o myReport.o stringEscape.o\
                                    y.tab.o lex.yy.o)

CXX_COMPILE_FLAGS := g++ -g -c -std=c++11
C_COMPILE_FLAGS := gcc -g -c -std=c99
COVERAGE_OPTIONS := -fprofile-arcs -ftest-coverage
COVERAGE_FOLDER := ./Coverage-Report

VPATH := ./AAI

tiger-compiler : $(COVERAGE_FOLDER) $(OBJECTS)
	g++ -g -coverage -o $(OUT_DIR)/$@ $(OBJECTS) -static -lcppunit

$(OBJECTS) : $(YACC) $(LEX)

$(OUT_DIR):
	mkdir -p $@

$(COVERAGE_FOLDER):
	mkdir -p $@

$(LEX): $(OUT_DIR)
	lex myTiger.lex
	$(C_COMPILE_FLAGS) -o $(OUT_DIR)/lex.yy.o lex.yy.c

$(YACC): $(OUT_DIR)
	yacc -d -v myTiger.y
	$(C_COMPILE_FLAGS) -o $(OUT_DIR)/y.tab.o y.tab.c

$(OUT_DIR)/%.o : %.cpp
	$(CXX_COMPILE_FLAGS) -o $@ $<

$(OUT_DIR)/%.o : %.c
	$(C_COMPILE_FLAGS) -o $@ $<

.PHONY : clean retest run show-coverage
clean:
	rm -f $(OBJECTS) $(OUT_DIR)/all.out *.gcda *.gcno

retest:
	make clean
	make
	$(OUT_DIR)/tiger-compiler

run:
	$(OUT_DIR)/tiger-compiler
