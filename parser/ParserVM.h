//
// Created by BurNingLi on 2025/9/19.
//

#ifndef BXSCRIPT_PARSERVM_H
#define BXSCRIPT_PARSERVM_H
#include <vector>

#include "Expression.h"


class ParserVM {
public:
    ParserVM *OuterVM;
    bool InFunc = false, InFor = false;
    std::vector<std::unique_ptr<ImportStatement>> imports{};
};

#endif //BXSCRIPT_PARSERVM_H
