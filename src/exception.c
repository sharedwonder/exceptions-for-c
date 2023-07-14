/*
 * Copyright (C) 2023 Liu Baihao (sharedwonder). All rights reserved.
 * This source code is licensed under the MIT License.
 */

#include "exception.h"

UncaughtExceptionHandler uncaughtExceptionHandler = exceptionTerminate;

ExceptionContext* exceptionContextStack = NULL;

const ExceptionType Exception = {"Exception", 0xFFFFFFFFFFFFFFFF, NULL};

void throwException(const ExceptionInstance exception) {
    if (exceptionContextStack == NULL) {
        if (uncaughtExceptionHandler != NULL) {
            uncaughtExceptionHandler(exception);   
        }
    } else {
        exceptionContextStack->exception = &exception;
        longjmp(exceptionContextStack->snapshot, exceptionContextStack->status == TryBlockInProgress ? TryBlockExceptionOccurred : TryBlockExceptionOutflow);
    }
}

ExceptionInstance newException(const ExceptionType type, const char* message) {
    const ExceptionInstance exception = {type, message};
    return exception;
}

bool exceptionInstanceOf(const ExceptionInstance* exception, ExceptionType type) {
    if (exception->type.code == type.code || type.code == Exception.code) {
        return true;
    }

    const ExceptionType* parent = exception->type.parent;
    while (parent != NULL) {
        if (parent->code == type.code) {
            return true;
        }
        parent = parent->parent;
    }
    return false;
}

void exceptionContextStackPush(ExceptionContext* exceptionContext) {
    exceptionContext->exception = NULL;
    exceptionContext->finally = false;
    exceptionContext->link = exceptionContextStack;
    exceptionContextStack = exceptionContext;
}

void exceptionContextStackPopup() {
    exceptionContextStack = exceptionContextStack->link;
}

void exceptionTerminate(ExceptionInstance exception) {
    PRINT_EXCEPTION_INFO(&exception);
    abort();
}
