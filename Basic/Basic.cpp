/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    // Parse numbered line vs command
    std::string first = scanner.nextToken();
    TokenType ft = scanner.getTokenType(first);
    if (ft == NUMBER) {
        // Program line
        if (!scanner.hasMoreTokens()) {
            program.removeSourceLine(stringToInteger(first));
            return;
        }
        std::string rest = line.substr(line.find(first) + first.size());
        // Keep original formatting including leading zeros
        program.addSourceLine(stringToInteger(first), trim(first + rest));
        return;
    }

    // Immediate commands
    std::string cmd = toUpperCase(first);
    if (cmd == "REM") {
        // ignore
        return;
    } else if (cmd == "LET") {
        LetStatement stmt(scanner);
        stmt.execute(state, program);
        return;
    } else if (cmd == "PRINT") {
        PrintStatement stmt(scanner);
        stmt.execute(state, program);
        return;
    } else if (cmd == "INPUT") {
        InputStatement stmt(scanner);
        stmt.execute(state, program);
        return;
    } else if (cmd == "END") {
        EndStatement stmt;
        stmt.execute(state, program);
        return;
    } else if (cmd == "GOTO") {
        GotoStatement stmt(scanner);
        stmt.execute(state, program);
        return;
    } else if (cmd == "IF") {
        IfStatement stmt(scanner);
        stmt.execute(state, program);
        return;
    } else if (cmd == "RUN") {
        // Execute stored program
        program.resetFlow();
        int cur = program.getFirstLineNumber();
        while (cur != -1 && !program.isTerminated()) {
            std::string src = program.getSourceLine(cur);
            TokenScanner sc;
            sc.ignoreWhitespace(); sc.scanNumbers();
            sc.setInput(src.substr(src.find(' ') + 1));
            std::string kw = toUpperCase(sc.nextToken());
            Statement *stmt = program.getParsedStatement(cur);
            if (!stmt) {
                if (kw == "REM") stmt = new RemStatement(sc);
                else if (kw == "LET") stmt = new LetStatement(sc);
                else if (kw == "PRINT") stmt = new PrintStatement(sc);
                else if (kw == "INPUT") stmt = new InputStatement(sc);
                else if (kw == "END") stmt = new EndStatement();
                else if (kw == "GOTO") stmt = new GotoStatement(sc);
                else if (kw == "IF") stmt = new IfStatement(sc);
                else error("SYNTAX ERROR");
                program.setParsedStatement(cur, stmt);
            }
            stmt->execute(state, program);
            int jump = program.consumeNextLine();
            if (jump != -1) {
                if (!program.hasLine(jump)) error("LINE NUMBER ERROR");
                cur = jump;
            } else {
                cur = program.getNextLineNumber(cur);
            }
        }
        return;
    } else if (cmd == "LIST") {
        int cur = program.getFirstLineNumber();
        while (cur != -1) {
            std::cout << program.getSourceLine(cur) << std::endl;
            cur = program.getNextLineNumber(cur);
        }
        return;
    } else if (cmd == "CLEAR") {
        program.clear();
        state.Clear();
        return;
    } else if (cmd == "QUIT") {
        exit(0);
    }

    error("SYNTAX ERROR");
}

