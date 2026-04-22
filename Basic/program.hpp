/*
 * File: program.h
 * ---------------
 * This interface exports a Program class for storing a BASIC
 * program.
 */

#ifndef _program_h
#define _program_h

#include <string>
#include <map>
#include "statement.hpp"

class Statement;

class Program {
public:
    Program();
    ~Program();

    void clear();
    void addSourceLine(int lineNumber, const std::string& line);
    void removeSourceLine(int lineNumber);
    std::string getSourceLine(int lineNumber);

    void setParsedStatement(int lineNumber, Statement *stmt);
    Statement *getParsedStatement(int lineNumber);

    int getFirstLineNumber();
    int getNextLineNumber(int lineNumber);

    bool hasLine(int lineNumber) const;

    // Flow control helpers used during RUN
    void setNextLine(int lineNumber);
    int consumeNextLine();
    void terminate();
    bool isTerminated() const;
    void resetFlow();

private:
    std::map<int, std::string> sourceLines;
    std::map<int, Statement*> parsed;
    int nextOverride; // -1 means no override
    bool terminated;
};

#endif
