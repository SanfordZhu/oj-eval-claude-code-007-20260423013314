/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"
#include "Utils/error.hpp"

Program::Program() {
    nextOverride = -1;
    terminated = false;
}

Program::~Program() {
    clear();
}

void Program::clear() {
    for (auto &p : parsed) delete p.second;
    parsed.clear();
    sourceLines.clear();
    resetFlow();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    if (hasLine(lineNumber)) {
        sourceLines[lineNumber] = line;
        if (parsed.count(lineNumber)) {
            delete parsed[lineNumber];
            parsed.erase(lineNumber);
        }
    } else {
        sourceLines.emplace(lineNumber, line);
    }
}

void Program::removeSourceLine(int lineNumber) {
    auto it = sourceLines.find(lineNumber);
    if (it == sourceLines.end()) return;
    sourceLines.erase(it);
    auto pit = parsed.find(lineNumber);
    if (pit != parsed.end()) {
        delete pit->second;
        parsed.erase(pit);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    auto it = sourceLines.find(lineNumber);
    if (it == sourceLines.end()) return "";
    return it->second;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    if (!hasLine(lineNumber)) error("LINE NUMBER ERROR");
    if (parsed.count(lineNumber)) {
        delete parsed[lineNumber];
    }
    parsed[lineNumber] = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
    auto it = parsed.find(lineNumber);
    if (it == parsed.end()) return nullptr;
    return it->second;
}

int Program::getFirstLineNumber() {
    if (sourceLines.empty()) return -1;
    return sourceLines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = sourceLines.upper_bound(lineNumber);
    if (it == sourceLines.end()) return -1;
    return it->first;
}

bool Program::hasLine(int lineNumber) const {
    return sourceLines.count(lineNumber) > 0;
}

void Program::setNextLine(int lineNumber) {
    nextOverride = lineNumber;
}

int Program::consumeNextLine() {
    int v = nextOverride;
    nextOverride = -1;
    return v;
}

void Program::terminate() { terminated = true; }

bool Program::isTerminated() const { return terminated; }

void Program::resetFlow() { nextOverride = -1; terminated = false; }
