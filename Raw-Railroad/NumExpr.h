#pragma once

#include "Expr.h"

class NumExpr: public Expr {
public:
    NumExpr(int);
    virtual ~NumExpr();

private:
    int m_num;
};
