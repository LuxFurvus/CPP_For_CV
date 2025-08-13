// SQL_ExecutorInterface.h

#pragma once
#ifndef SQL_EXECUTOR_H
#define SQL_EXECUTOR_H

#include <string>

struct SQL_TableValues2D;

struct SQL_ExecutorInterface
{
    virtual ~SQL_ExecutorInterface() = default;
    
    virtual SQL_TableValues2D ExecuteQuery(
        const std::string& DbPath, const std::string& Query) = 0;
};



#endif // SQL_EXECUTOR_H