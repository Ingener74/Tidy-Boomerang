#include <iostream>
#include <algorithm>
#include <Poco/AutoPtr.h>
#include "AST.h"
#include "Utils.h"
#include "Interpreter.h"

namespace fun {

using namespace std;
using namespace Poco;

Interpreter::Interpreter(Debugger* debugger) :
        _debugger(debugger) {
    stack.push_back(new StackFrame);
}

Interpreter::~Interpreter() {
}

void Interpreter::iterateStatements(Statement *stmts) {
}

void Interpreter::visit(Break* break_stmt) {
}

void Interpreter::visit(Continue* continue_stmt) {
}

void Interpreter::visit(For* for_stmt) {
}

void Interpreter::visit(Function *function) {
    if (load || function->name) {
        operands.emplace_back(function, true);
    }

    if (function->name) {
        store = true;
        debug(function->name)->accept(this);
        store = false;
    }
}

void Interpreter::visit(Ifs *ifs_stmt) {
}

void Interpreter::visit(If* if_stmt) {
//
//    if (if_stmt->cond) {
//
//        size_t operands_size = operands.size();
//
//        load = true;
//        debug(if_stmt->cond)->accept(this);
//        load = false;
//
//        size_t operands_diff = operands.size() - operands_size;
//
//        if (operands_diff) {
//            if (operands.at(operands_size)->toBoolean()) {
//                ip = if_stmt->stmts;
//            } else {
//                ip = if_stmt->nextIf;
//            }
//
//            for (size_t i = 0; i < operands_diff; ++i)
//                operands.pop_back();
//
//        } else {
//            ip = if_stmt->nextIf;
//        }
//    } else {
//        ip = if_stmt->stmts;
//    }
}

void Interpreter::visit(Import* import_stmt) {
    auto ops = operands.size();
    fassertl(operands.size() == ops, import_stmt->loc, "operands balance broken after statement")
}

void Interpreter::visit(Print* print) {
    auto ops = operands.size();

    load = true;
    debug(print->expression)->accept(this);
    load = false;

    cout << operands.back()->toString() << endl;

    operands.pop_back();

    fassertl(operands.size() == ops, print->loc, "operands balance broken after statement")
}

void Interpreter::visit(Return *return_stmt) {
//    auto expr = return_stmt->expression;
//    while (expr) {
//        load = true;
//        expr = debug(expr)->accept(this)->nextExpression;
//        load = false;
//    }
//    for (auto ipIt = stack.rbegin(); ipIt != stack.rend(); ++ipIt)
//        if ((*ipIt)->returnIp)
//            ip = (*ipIt)->returnIp;
}

void Interpreter::visit(While* while_stmt) {
//    stack.push_back(new StackFrame);
//
//    size_t operands_size = operands.size();
//
//    load = true;
//    debug(while_stmt->cond)->accept(this);
//    load = false;
//
//    size_t operands_diff = operands.size() - operands_size;
//
//    if (operands_diff) {
//        if (operands.at(operands_size)->toBoolean()) {
//            ip = if_stmt->stmts;
//        } else {
//            ip = if_stmt->nextIf;
//        }
//    }
//
//    ip = while_stmt->stmts;
//    stack.back()->continueIp = while_stmt->stmts;
//    stack.back()->breakIp = while_stmt->nextStatement;
//    stack.back()->nextIp = while_stmt->nextStatement;
}

void Interpreter::visit(Class* class_stmt) {
}

void Interpreter::visit(Exception* exception_stmt) {
//    stack.push_back(new StackFrame);
//    ip = exception_stmt->tryStmts;
//    stack.back()->catchIp = exception_stmt->catchStmts;
}

void Interpreter::visit(Throw* throw_stmt) {
//    auto expr = throw_stmt->expression;
//    while (expr) {
//        load = true;
//        expr = debug(expr)->accept(this)->nextExpression;
//        load = false;
//    }
//    for (auto ipIt = stack.rbegin(); ipIt != stack.rend(); ++ipIt)
//        if ((*ipIt)->catchIp)
//            ip = (*ipIt)->catchIp;
}

void Interpreter::visit(Assign* assign) {
    bool load_f = load;

    auto balance = operands.size();
    auto lhs = assign->ids;
    auto rhs = assign->exprs;

    while (rhs) {
        load = true;
        rhs = debug(rhs)->accept(this)->nextExpression;
        load = false;

        auto operands_diff = operands.size() - balance;

        vector<AutoPtr<Terminal>> tmp;

        for (decltype(operands_diff) i = 0; i < operands_diff; ++i) {
            tmp.push_back(operands.at(balance + i));
        }
        for (decltype(operands_diff) i = 0; i < operands_diff; ++i) {
            operands.pop_back();
        }
        for (decltype(operands_diff) i = 0; i < operands_diff; ++i) {
            if (lhs) {
                if (assign->type == BinaryOperation::Assign) {
                    operands.push_back(tmp.at(i));

                    store = true;
                    lhs = debug(lhs)->accept(this)->nextExpression;
                    store = false;
                } else {
                    load = true;
                    lhs = debug(lhs)->accept(this);
                    load = false;

                    fassertl(!operands.empty(), assign->loc, "not enogth operands")

                    AutoPtr<Terminal> L = operands.back();
                    operands.pop_back();

                    operands.push_back(tmp.at(i));

                    fassertl(!operands.empty(), assign->loc, "not enogth operands")

                    AutoPtr<Terminal> R = operands.back();
                    operands.pop_back();

                    operands.push_back(operate(L, assign->type, R));

                    store = true;
                    lhs = debug(lhs)->accept(this)->nextExpression;
                    store = false;
                }
            }
        }
    }
    while (lhs) {
        if (assign->type == BinaryOperation::Assign) {
            operands.push_back(new Nil);

            store = true;
            lhs = debug(lhs)->accept(this)->nextExpression;
            store = false;
        } else {
            load = true;
            lhs = debug(lhs)->accept(this);
            load = false;

            fassertl(!operands.empty(), assign->loc, "not enogth operands")

            AutoPtr<Terminal> L = operands.back();
            operands.pop_back();

            operands.push_back(new Nil);

            fassertl(!operands.empty(), assign->loc, "not enogth operands")

            AutoPtr<Terminal> R = operands.back();
            operands.pop_back();

            operands.push_back(operate(L, assign->type, R));

            store = true;
            lhs = debug(lhs)->accept(this)->nextExpression;
            store = false;
        }
    }

    if (load_f) {
        auto lhs = assign->ids;
        while (lhs) {
            load = true;
            lhs = debug(lhs)->accept(this)->nextExpression;
            load = false;
        }
    } else {
        fassertl((operands.size() - balance) == 0, assign->loc, "operands balance broken after statement")
    }
}

void Interpreter::visit(BinaryOp* bin_op) {
    auto ops = operands.size();

    load = true;
    debug(bin_op->lhs)->accept(this);
    load = false;

    fassertl(!operands.empty(), bin_op->loc, "not enogth operands")

    AutoPtr<Terminal> lhs = operands.back();
    operands.pop_back();

    load = true;
    debug(bin_op->rhs)->accept(this);
    load = false;

    fassertl(!operands.empty(), bin_op->loc, "not enogth operands")

    AutoPtr<Terminal> rhs = operands.back();
    operands.pop_back();

    operands.push_back(operate(lhs, bin_op->m_operation, rhs));

    fassertl(operands.size() == (ops + 1), bin_op->loc, "operands balance broken after statement")
}

void Interpreter::visit(Dot *dot) {
}

void Interpreter::visit(Call* call) {
    load = true;
    debug(call->callable)->accept(this);
    load = false;

    /*
     * Get callable object
     */
    fassertl(operands.size() > 0, call->loc, "callable not found");
    fassertl(operands.back()->getType() == Type::Function, call->loc, "must be callable");
    auto function = operands.back().cast<Function>();
    operands.pop_back();

    /*
     * Assign passed expressions to parameters
     */
    auto expr = call->arguments;
    auto arg = function->args;
    while(expr && arg){
        load = true;
        expr = debug(expr)->accept(this)->nextExpression;
        load = false;

        store = true;
        arg = debug(arg)->accept(this)->nextId;
        store = false;
    }

    /*
     * Iterate function statements
     */
    auto stmt = function->stmts;

    while (stmt) {
        stmt = debug(stmt)->accept(this)->nextStatement;
//        if (return_flag) {
//            return_flag = false;
//            break;
//        }
    }
}

void Interpreter::visit(Dictionary* dict) {
}

void Interpreter::visit(Id* id) {
    if (load) {
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
            auto var = (*it)->variables.find(id->value);
            if (var == (*it)->variables.end()) {
                continue;
            } else {
                operands.push_back(var->second);
                fassertl(!operands.empty(), id->loc, "operands empty after expression")
                return;
            }
        }
        fassertl(false, id->loc, id->value + " undefined");
    } else if (store) {
        fassertl(operands.size() > 0, id->loc, "have no operands");

        auto rit = stack.rbegin();

        auto var = (*rit)->variables.find(id->value);

        auto val = operands.back();
        operands.pop_back();

        if (var == (*rit)->variables.end()) {
            auto it = (*rit)->variables.insert({ id->value, val });
            fassertl(it.second, id->loc, "can't store top operands value in " + id->value);
        } else {
            var->second = val;
        }
    }
}

void Interpreter::visit(RoundBrackets* round_brackets) {
    if (round_brackets->expr){
        debug(round_brackets->expr)->accept(this);
        fassertl(!operands.empty(), round_brackets->loc, "operands empty after expression")
    }
}

// Terminals

void Interpreter::visit(Boolean *boolean) {
    if (load) {
        operands.emplace_back(boolean, true);
        fassertl(!operands.empty(), boolean->loc, "operands empty after expression")
    }
    fassertl(!store, boolean->loc, "you can't assign to value")
}

void Interpreter::visit(Integer *integer) {
    if (load) {
        operands.emplace_back(integer, true);
        fassertl(!operands.empty(), integer->loc, "operands empty after expression")
    }
    fassertl(!store, integer->loc, "you can't assign to value")
}

void Interpreter::visit(Nil *nil) {
    if (load) {
        operands.emplace_back(nil, true);
        fassertl(!operands.empty(), nil->loc, "operands empty after expression")
    }
    fassertl(!store, nil->loc, "you can't assign to value")
}

void Interpreter::visit(Real *real) {
    if (load) {
        operands.emplace_back(real, true);
        fassertl(!operands.empty(), real->loc, "operands empty after expression")
    }
    fassertl(!store, real->loc, "you can't assign to value")
}

void Interpreter::visit(String *str) {
    if (load) {
        operands.emplace_back(str, true);
        fassertl(!operands.empty(), str->loc, "operands empty after expression")
    }
    fassertl(!store, str->loc, "you can't assign to value")
}

Terminal* Interpreter::operate(Terminal* a, BinaryOperation op, Terminal* b) {
    auto seniorType = Terminal::getSeniorBinaryOpType(a, b);
    switch (seniorType) {
    case Type::Integer: {

        auto lhs = a->toInteger();
        auto rhs = b->toInteger();

        switch (op) {
        case BinaryOperation::Add: { return new Integer(lhs + rhs); }
        case BinaryOperation::Sub: { return new Integer(lhs - rhs); }
        case BinaryOperation::Mul: { return new Integer(lhs * rhs); }
        case BinaryOperation::Div: { fassert(rhs != 0, "divide by zero"); return new Integer(lhs / rhs); }
        case BinaryOperation::Mod: { fassert(rhs != 0, "divide by zero"); return new Integer(lhs % rhs); }

        case BinaryOperation::LShift:     { return new Integer(lhs << rhs); }
        case BinaryOperation::RShift:     { return new Integer(lhs >> rhs); }
        case BinaryOperation::BinaryOr:  { return new Integer(lhs | rhs); }
        case BinaryOperation::BinaryAnd: { return new Integer(lhs & rhs); }
        case BinaryOperation::BinaryXor: { return new Integer(lhs ^ rhs); }

        case BinaryOperation::LogicOr:  { return new Boolean(lhs || rhs); }
        case BinaryOperation::LogicAnd: { return new Boolean(lhs && rhs); }

        case BinaryOperation::Equal:      { return new Boolean(lhs == rhs); }
        case BinaryOperation::NotEqual:  { return new Boolean(lhs != rhs); }
        case BinaryOperation::More:       { return new Boolean(lhs >  rhs); }
        case BinaryOperation::MoreEqual: { return new Boolean(lhs >= rhs); }
        case BinaryOperation::Less:       { return new Boolean(lhs <  rhs); }
        case BinaryOperation::LessEqual: { return new Boolean(lhs <= rhs); }
        default:
            fassertl(false, a->loc + b->loc, "unsupported operation");
        }
        break;
    }
    case Type::Real: {
        auto lhs = a->toReal();
        auto rhs = b->toReal();

        switch (op) {
        case BinaryOperation::Add: { return new Real(lhs + rhs); }
        case BinaryOperation::Sub: { return new Real(lhs - rhs); }
        case BinaryOperation::Mul: { return new Real(lhs * rhs); }
        case BinaryOperation::Div: { fassert(rhs != 0.0, "devide by zero"); return new Real(lhs / rhs); }

        case BinaryOperation::LogicOr:  { return new Boolean(Real::isTrue(lhs) || Real::isTrue(rhs)); }
        case BinaryOperation::LogicAnd: { return new Boolean(Real::isTrue(lhs) && Real::isTrue(rhs)); }

        case BinaryOperation::Equal:      { return new Boolean(lhs == rhs); }
        case BinaryOperation::NotEqual:  { return new Boolean(lhs != rhs); }
        case BinaryOperation::More:       { return new Boolean(lhs >  rhs); }
        case BinaryOperation::MoreEqual: { return new Boolean(lhs >= rhs); }
        case BinaryOperation::Less:       { return new Boolean(lhs <  rhs); }
        case BinaryOperation::LessEqual: { return new Boolean(lhs <= rhs); }
        default:
            fassertl(false, a->loc + b->loc, "unsupported operation");
        }
        break;
    }
    case Type::String: {
        auto lhs = a->toString();
        auto rhs = b->toString();

        switch (op) {
        case BinaryOperation::Add: { return new String(lhs + rhs); }

        case BinaryOperation::LogicOr:  { return new Boolean(String::isTrue(lhs) || String::isTrue(rhs)); }
        case BinaryOperation::LogicAnd: { return new Boolean(String::isTrue(lhs) && String::isTrue(rhs)); }

        case BinaryOperation::LShift:     { return new String(lhs + rhs); }
        case BinaryOperation::RShift:     { return new String(lhs + rhs); }

        case BinaryOperation::More:       { return new Boolean(lhs.size() >  rhs.size()); }
        case BinaryOperation::MoreEqual: { return new Boolean(lhs.size() >= rhs.size()); }
        case BinaryOperation::Less:       { return new Boolean(lhs.size() <  rhs.size()); }
        case BinaryOperation::LessEqual: { return new Boolean(lhs.size() <= rhs.size()); }

        case BinaryOperation::Equal:      { return new Boolean(lhs == rhs); }
        case BinaryOperation::NotEqual:  { return new Boolean(lhs != rhs); }
        default:
            fassertl(false, a->loc + b->loc, "unsupported operation");
        }
        break;
    }
    case Type::Boolean:{
        auto lhs = a->toBoolean();
        auto rhs = b->toBoolean();

        switch (op) {
        case BinaryOperation::More:       { return new Boolean(lhs >  rhs); }
        case BinaryOperation::MoreEqual: { return new Boolean(lhs >= rhs); }
        case BinaryOperation::Less:       { return new Boolean(lhs <  rhs); }
        case BinaryOperation::LessEqual: { return new Boolean(lhs <= rhs); }

        case BinaryOperation::Equal:      { return new Boolean(lhs == rhs); }
        case BinaryOperation::NotEqual:  { return new Boolean(lhs != rhs); }

        case BinaryOperation::LogicOr:  { return new Boolean(lhs || rhs); }
        case BinaryOperation::LogicAnd: { return new Boolean(lhs && rhs); }

        default:
            fassertl(false, a->loc + b->loc, "unsupported operation");
        }
        break;
    }
    case Type::Nil: {
        switch (op) {
        case BinaryOperation::LogicAnd: { return new Boolean(false); }
        case BinaryOperation::LogicOr: { return new Boolean(false); }

        case BinaryOperation::Equal:      { return new Boolean(true); }
        case BinaryOperation::NotEqual:  { return new Boolean(false); }

        default:
            fassertl(false, a->loc + b->loc, "unsupported operation");
        }
        break;
    }
    default:
        fassertl(false, a->loc + b->loc, "unsupported operation");
    }
    return nullptr;
}

size_t Interpreter::count() const {
    return operands.size();
}

Type Interpreter::type(size_t operand) const {
    return operands.at(operand)->getType();
}

AutoPtr<Terminal> Interpreter::operand(size_t operand) const {
    return operands.at(operand);
}

bool Interpreter::boolean(size_t operand) const {
    return operands.at(operand)->toBoolean();
}

long long int Interpreter::integer(size_t operand) const {
    return operands.at(operand)->toInteger();
}

double Interpreter::real(size_t operand) const {
    return operands.at(operand)->toReal();
}

string Interpreter::str(size_t operand) const {
    return operands.at(operand)->toString();
}

size_t Interpreter::levelCount() const {
    return stack.size();
}

size_t Interpreter::count(size_t memoryLevel) const {
    return stack.at(memoryLevel)->variables.size();
}

bool Interpreter::has(const string& name) const {
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        if ((*it)->variables.find(name) != (*it)->variables.end()) {
            return true;
        }
    }
    return false;
}

bool Interpreter::has(size_t memoryLevel, const string& name) const {
    if (stack.at(memoryLevel)->variables.find(name) != stack.at(memoryLevel)->variables.end()) {
        return true;
    }
    return false;
}

Type Interpreter::type(const string& name) const {
    auto var = variable(name);
    if (var.isNull())
        return Type::Count;
    return var->getType();
}

Type Interpreter::type(size_t memoryLevel, const string& name) const {
    auto var = variable(memoryLevel, name);
    if (var.isNull())
        return Type::Count;
    return var->getType();
}

AutoPtr<Terminal> Interpreter::variable(const string& name) const {
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        auto varIt = (*it)->variables.find(name);
        if (varIt != (*it)->variables.end()) {
            return varIt->second;
        }
    }
    return {};
}

AutoPtr<Terminal> Interpreter::variable(size_t memoryLevel, const string& name) const {
    auto varIt = stack.at(memoryLevel)->variables.find(name);
    if (varIt != stack.at(memoryLevel)->variables.end()) {
        return varIt->second;
    }
    return {};
}

bool Interpreter::boolean(const string& name) const {
    auto var = variable(name);
    if (var.isNull())
        return false;
    return var->toBoolean();
}

bool Interpreter::boolean(size_t memoryLevel, const string& name) const {
    auto var = variable(memoryLevel, name);
    if (var.isNull())
        return false;
    return var->toBoolean();
}

long long int Interpreter::integer(const string& name) const {
    auto var = variable(name);
    if (var.isNull())
        return false;
    return var->toInteger();
}

long long int Interpreter::integer(size_t memoryLevel, const string& name) const {
    auto var = variable(memoryLevel, name);
    if (var.isNull())
        return false;
    return var->toInteger();
}

double Interpreter::real(const string& name) const {
    auto var = variable(name);
    if (var.isNull())
        return false;
    return var->toReal();
}

double Interpreter::real(size_t memoryLevel, const string& name) const {
    auto var = variable(memoryLevel, name);
    if (var.isNull())
        return false;
    return var->toReal();
}

string Interpreter::str(const string& name) const {
    auto var = variable(name);
    if (var.isNull())
        return {};
    return var->toString();
}

string Interpreter::str(size_t memoryLevel, const string& name) const {
    auto var = variable(memoryLevel, name);
    if (var.isNull())
        return {};
    return var->toString();
}

//void Interpreter::iterate() {
//    while (ip)
//        ip = next(debug(ip)->accept(this));
//}
//
//Statement* Interpreter::next(Statement* stmt) {
//    if (ip == stmt) {
//        if (stmt && stmt->nextStatement) {
//            return stmt->nextStatement;
//        } else {
//            if (stack.size() > 1)
//                stack.pop_back();
//            return stack.back()->nextIp;
//        }
//    } else {
//        return ip;
//    }
//}

}
