#include <sstream>
#include <cmath>
#include "Visitor.h"
#include "Utils.h"
#include "AST.h"

#define ACCEPT(CLASS, BODY)        \
CLASS* CLASS::accept(Visitor* v) { \
    fassert(v, "Visitor is null"); \
    BODY                           \
    v->visit(this);                \
    return this;                   \
}

namespace fun {

using namespace std;

void Pot::setRoot(Statement* root) {
    _root.assign(root, true);
}

Pot::~Pot() {
}

void Pot::accept(Visitor* v) {
    v->iterateStatements(_root);
}

int Statement::stmtCounter = 0;

Statement::Statement(const location& loc) :
        loc(loc) {
    stmtCounter++;
}

Statement::~Statement(){
    stmtCounter--;
    removeRefs(nextStatement);
}

int Statement::counter() {
    return stmtCounter;
}

void Statement::resetCounter() {
    stmtCounter = 0;
}

ACCEPT(Break, )

ACCEPT(Class, )

Class::~Class() {
    removeRefs(name, derived, stmts);
}

ACCEPT(Continue, )

ACCEPT(Exception, )

Exception::~Exception(){
    removeRefs(tryStmts, errorClasses, errorObject, catchStmts);
}

ACCEPT(For, )

For::~For(){
    removeRefs(initial, condition, increment, stmts);
}

ACCEPT(Function, )

Function::~Function(){
    removeRefs(name, args, stmts, nextFunction);
}

ACCEPT(Ifs, )

Ifs::~Ifs() {
    removeRefs(if_stmts);
}

ACCEPT(If, )

If::~If(){
    removeRefs(cond, stmts, nextIf);
}

ACCEPT(Import, {
    fassert(id, "Import must have an id");
})

Import::~Import(){
    removeRefs(id);
}

ACCEPT(Print, {
    fassert(expression, "Print must have the expressions")
})

Print::~Print(){
    removeRefs(expression);
}

ACCEPT(Return, )

Return::~Return(){
    removeRefs(expression);
}

ACCEPT(Throw, )

Throw::~Throw(){
    removeRefs(expression);
}

ACCEPT(While, {
    fassert(cond, "While must have the condition expression");
})

While::~While(){
    removeRefs(cond, stmts);
}

void Expression::apply(Expression* expression, Visitor* v) {
    while (expression)
        expression = expression->accept(v)->nextExpression;
}

Expression::~Expression(){
    removeRefs(nextExpression);
}

ACCEPT(Assign, {
    fassert(ids, "Assign must have name")
    fassert(exprs, "Assign must have value")
})

Assign::~Assign(){
    removeRefs(ids, exprs, nextAssign);
}

void Assign::apply(Assign* assign, Visitor* v) {
    while (assign)
        assign = assign->accept(v)->nextAssign;
}

ACCEPT(BinaryOp, {
    fassert(lhs, "Binary operation must have left side expression")
    fassert(rhs, "Binary operation must have right side expression")
})

BinaryOp::~BinaryOp(){
    removeRefs(lhs, rhs);
}

ACCEPT(Dot, )

Dot::~Dot() {
    removeRefs(lhs, rhs);
}

ACCEPT(Call, {
    fassert(callable, "Call expression must have name")
})

Call::~Call(){
    removeRefs(callable, arguments);
}

ACCEPT(Dictionary, )

Dictionary::~Dictionary(){
    removeRefs(assign);
}

ACCEPT(Id, )

Id::~Id(){
    removeRefs(nextId);
}

ACCEPT(Index, {
    fassert(indexable, "Index expression must have name")
})

Index::~Index(){
    removeRefs(indexable, arg);
}

//ACCEPT(ForExpression, {
//})

ACCEPT(RoundBrackets, )

RoundBrackets::~RoundBrackets(){
    removeRefs(expr);
}

Terminal::Type Terminal::getSeniorBinaryOpType(Terminal::Type lhs, Terminal::Type rhs) {
    fassert(lhs == Nil || lhs == Boolean || lhs == Integer || lhs == Real || lhs == String, "unsupported type");
    fassert(rhs == Nil || rhs == Boolean || rhs == Integer || rhs == Real || rhs == String, "unsupported type");
    return std::max<Type>(lhs, rhs);
}

Terminal::Type Terminal::getSeniorBinaryOpType(Terminal* lhs, Terminal* rhs) {
    return getSeniorBinaryOpType(lhs->getType(), rhs->getType());
}

ACCEPT(Boolean, )

ACCEPT(Integer, )

ACCEPT(Nil, )

ACCEPT(Real, )

ACCEPT(String, )

string Integer::toString() const {
    stringstream ss;
    ss << value;
    return ss.str();
}

string Real::toString() const {
    stringstream ss;
    ss << value;
    return ss.str();
}

bool Real::toBoolean() const {
    return isTrue(value);
}

const double Epsilon = 1e-8;
bool Real::isTrue(double val) {
    return fabs(val) > Epsilon;
}

bool String::isTrue(const std::string& str) {
    return str != "false" && str != "nil" && !str.empty();
}

}
