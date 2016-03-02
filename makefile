CC=g++
CFLAGS=-std=c++11 -Ofast -march=native -mtune=native -I./ -c -Wall
TESTCFLAGS=-DTEST_PROJECT
LDFLAGS=-lpthread -lncurses
SOURCES=main.cpp CPU.cpp PMMU.cpp UART8250.cpp Coprocessor0.cpp COP0Register.cpp ConsoleUI.cpp MIPSException.cpp
TESTSOURCES=STest/testmain.cpp STest/STest.cpp STest/MIPS_Tests_Common.cpp STest/ArithmeticTests.cpp STest/LogicalTests.cpp STest/BranchAndJumpTests.cpp STest/InstructionControlInstructionTests.cpp STest/LoadStoreMemoryControlTests.cpp STest/InsertExtractTests.cpp STest/MoveTests.cpp STest/ShiftTests.cpp STest/TrapTests.cpp STest/ObsoleteBranchTests.cpp STest/PrivilegedInstructionTests.cpp CPU.cpp PMMU.cpp UART8250.cpp Coprocessor0.cpp COP0Register.cpp ConsoleUI.cpp MIPSException.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TEST_OBJECTS=$(TESTSOURCES:.cpp=.o)
EXECUTABLE=mips.x
TEST_EXEC=mips_tests.x

all: clean $(SOURCES) $(EXECUTABLE)

test: .testflags clean $(TESTSOURCES) $(TEST_EXEC)

.testflags:
	$(eval CFLAGS += $(TESTCFLAGS))

$(TEST_EXEC): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) $(LDFLAGS) -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(TEST_OBJECTS)
	rm -rf $(EXECUTABLE)
	rm -rf $(TEST_EXEC)
