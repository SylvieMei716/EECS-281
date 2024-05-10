// Project identifier: 1CAEF3A0FEDD0DEC26BA9808C69D4D22A9962768

#include "evaluate.hpp"

// #include <bits/stdint-intn.h>
#include <cctype>
#include <cstdint>
#include <stack>
#include <string>


std::int64_t evaluate(std::string const& expression) {
    // Your implementation goes here
    std::stack<char> operators;
    std::stack<std::int64_t> operands;

    for (size_t i = 0; i < expression.size(); i++) {
        if (std::isdigit(expression[i])) {
            int64_t operand = expression[i] - '0';
            operands.push(operand);
        }
        else {
            operators.push(expression[i]);
        }
        while (operands.size() >= 2 && !operators.empty()) {
            int64_t operand2 = operands.top();
            operands.pop();
            int64_t operand1 = operands.top();
            operands.pop();
            char oprt = operators.top();
            operators.pop();

            if (oprt == '+') {
                operands.push(operand1 + operand2);
            }
            if (oprt == '-') {
                operands.push(operand1 - operand2);
            }
            if (oprt == '*') {
                operands.push(operand1 * operand2);
            }
            if (oprt == '/') {
                operands.push(operand1 / operand2);
            }
        }
    }

    while (operands.size() >= 2 && !operators.empty()) {
        int64_t operand2 = operands.top();
        operands.pop();
        int64_t operand1 = operands.top();
        operands.pop();
        char oprt = operators.top();
        operators.pop();

        if (oprt == '+') {
            operands.push(operand1 + operand2);
        }
        if (oprt == '-') {
            operands.push(operand1 - operand2);
        }
        if (oprt == '*') {
            operands.push(operand1 * operand2);
        }
        if (oprt == '/') {
            operands.push(operand1 / operand2);
        }
    }

    return operands.top();
}
