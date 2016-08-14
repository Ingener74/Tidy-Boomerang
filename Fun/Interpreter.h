#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <condition_variable>

#include <Poco/Mutex.h>
#include <Poco/Condition.h>

#include <Visitor.h>

namespace fun {

class Terminal;

using Operands = std::vector<Terminal*>;

//using Memory = std::vector<std::unordered_map<std::string, Terminal*>>;
using Memory = std::unordered_map<std::string, Terminal*>;

class Breakpoint {
public:
    Breakpoint(const std::string& module = {}, int line = 0) :
            module(module), line(line) {
    }
    virtual ~Breakpoint() {
    }

    bool operator==(const Breakpoint &rhs) const {
        return module == rhs.module && line == rhs.line;
    }

    bool operator!=(const Breakpoint &rhs) const {
        return !(rhs == *this);
    }

    std::string module;
    int line;
};

using Breakpoints = std::vector<Breakpoint>;

class Interpreter;
class Printer;

class Debugger {
public:
    class WaitRun {
        bool _stepOver = false;
        Poco::Mutex _mutex;
        Poco::Condition _cond;

    public:
        WaitRun() = default;

        void wait() {
            Poco::Mutex::ScopedLock lock{_mutex};
            while (!_stepOver)
                _cond.wait(_mutex);
            _stepOver = false;
        }

        void run() {
            Poco::Mutex::ScopedLock lock(_mutex);
            _stepOver = true;
            _cond.signal();
        }
    };

    Debugger(Printer *printer) : _printer(printer) {
    }
    virtual ~Debugger() = default;

    virtual void setBreakpoint(const Breakpoint &breakpoint);
    virtual void enableBreakpoint(const Breakpoint &breakpoint);
    virtual void disableBreakpoint(const Breakpoint &breakpoint);
    virtual void removeBreakpoint(const Breakpoint &breakpoint);
    virtual const Breakpoints& getBreakpoints() const;

    virtual void pause();
    virtual void resume();
    virtual void stepOver();
    virtual void stepIn();
    virtual void stepOut();

    virtual void onBeforeStep(Statement *);

    virtual void onCatchBreakpoint(const Breakpoint&) = 0;
    virtual void onOperandsChanged(const Operands&) = 0;
    virtual void onMemoryChanged(const Memory&) = 0;

    virtual void list();

protected:
    Breakpoints vb;
    WaitRun _wr;

    Printer* _printer = nullptr;
    Statement* _currentStatement = nullptr;
};

class Interpreter: public Visitor {
public:
    Interpreter(Debugger* = nullptr);
    virtual ~Interpreter();


    virtual void iterateStatements(Statement*);

    virtual void visit(Break*);
    virtual void visit(Continue*);
    virtual void visit(For*);
    virtual void visit(Function*);
    virtual void visit(If*);
    virtual void visit(ElseIf*);
    virtual void visit(Else*);
    virtual void visit(IfElseIfsElse*);
    virtual void visit(Import*);
    virtual void visit(Print*);
    virtual void visit(Return*);
    virtual void visit(While*);
    virtual void visit(Class*);
    virtual void visit(Exception*);
    virtual void visit(Throw*);

    virtual void visit(Assign*);
    virtual void visit(BinaryOp*);
    virtual void visit(Call*);
    virtual void visit(Dictionary*);
    virtual void visit(Id*);
    virtual void visit(RoundBrackets*);

    virtual void visit(Boolean*);
    virtual void visit(Integer*);
    virtual void visit(Nil*);
    virtual void visit(Real*);
    virtual void visit(String*);

    const Operands& getOperands() const;
    const Memory& getMemory() const;

private:
    Operands operands;

    Memory variables;

    bool load = false;
    bool store = false;
    bool break_flag = false;
    bool continue_flag = false;
    bool return_flag = false;

    // std::vector<Statement*> instructionPointerStack;

    Terminal* operate(Terminal* a, BinaryOp::Op operation, Terminal* b);

    Debugger* debugger = nullptr;

    template<typename TStatement>
    TStatement* debug(TStatement *stmt) {
        if (debugger) debugger->onBeforeStep(stmt);
        return stmt;
    }
};

}
