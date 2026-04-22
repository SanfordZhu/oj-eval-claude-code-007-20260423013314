/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"

/* Implementation of the Statement class */

Statement::Statement() = default;
Statement::~Statement() = default;

RemStatement::RemStatement(TokenScanner &scanner) {
    while (scanner.hasMoreTokens()) scanner.nextToken();
}
void RemStatement::execute(EvalState &state, Program &program) { (void) state; (void) program; }

LetStatement::LetStatement(TokenScanner &scanner) { exp = parseExp(scanner); }
LetStatement::~LetStatement() { delete exp; }
void LetStatement::execute(EvalState &state, Program &program) {
    int v = exp->eval(state);
    (void) v;
}

PrintStatement::PrintStatement(TokenScanner &scanner) { exp = parseExp(scanner); }
PrintStatement::~PrintStatement() { delete exp; }
void PrintStatement::execute(EvalState &state, Program &program) {
    int v = exp->eval(state);
    std::cout << v << std::endl;
}

InputStatement::InputStatement(TokenScanner &scanner) {
    if (!scanner.hasMoreTokens()) error("SYNTAX ERROR");
    std::string t = scanner.nextToken();
    TokenType tp = scanner.getTokenType(t);
    if (tp != WORD && tp != NUMBER) error("SYNTAX ERROR");
    var = t;
}
void InputStatement::execute(EvalState &state, Program &program) {
    (void) program;
    std::string line;
    if (!std::getline(std::cin, line)) error("INPUT ERROR");
    TokenScanner sc(line);
    sc.ignoreWhitespace();
    sc.scanNumbers();
    if (!sc.hasMoreTokens()) error("SYNTAX ERROR");
    std::string tok = sc.nextToken();
    if (sc.hasMoreTokens()) error("SYNTAX ERROR");
    TokenType tp = sc.getTokenType(tok);
    if (tp != NUMBER) error("SYNTAX ERROR");
    int val = stringToInteger(tok);
    state.setValue(var, val);
}

EndStatement::EndStatement() {}
void EndStatement::execute(EvalState &state, Program &program) {
    (void) state; program.terminate();
}

GotoStatement::GotoStatement(TokenScanner &scanner) {
    std::string t = scanner.nextToken();
    if (scanner.getTokenType(t) != NUMBER) error("SYNTAX ERROR");
    target = stringToInteger(t);
}
void GotoStatement::execute(EvalState &state, Program &program) {
    (void) state; program.setNextLine(target);
}

IfStatement::IfStatement(TokenScanner &scanner) {
    lhs = readE(scanner);
    if (!scanner.hasMoreTokens()) error("SYNTAX ERROR");
    op = scanner.nextToken();
    if (op != "=" && op != "<" && op != ">") error("SYNTAX ERROR");
    rhs = readE(scanner);
    scanner.verifyToken("THEN");
    std::string t = scanner.nextToken();
    if (scanner.getTokenType(t) != NUMBER) error("SYNTAX ERROR");
    target = stringToInteger(t);
}
IfStatement::~IfStatement() { delete lhs; delete rhs; }
void IfStatement::execute(EvalState &state, Program &program) {
    int lv = lhs->eval(state);
    int rv = rhs->eval(state);
    bool cond = false;
    if (op == "=") cond = (lv == rv);
    else if (op == "<") cond = (lv < rv);
    else if (op == ">") cond = (lv > rv);
    if (cond) program.setNextLine(target);
}
