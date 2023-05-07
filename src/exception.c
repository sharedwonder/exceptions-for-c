/*
 * Copyright (C) 2022 Liu Baihao. All rights reserved.
 * This source code is licensed under the MIT License.
 */

#include "exception.h"

ExceptionContext* exceptionContextStack = NULL;

DEFINE_EXCEPTION(Exception, 0xFFFFFFFFFFFFFFFF, NULL);

ExceptionInstance exceptionNew(const ExceptionType type, const char* message) {
    ExceptionInstance exception = {
        .type = type,
        .message = message
    };
    return exception;
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

bool exceptionInstanceOf(ExceptionInstance* exception, ExceptionType type) {
    if (exception->type.code == type.code || type.code == Exception.code) {
        return true;
    }

    ExceptionType* parent = exception->type.parent;
    while (parent != NULL) {
        if (parent->code == type.code) {
            return true;
        }
        parent = parent->parent;
    }
    return false;
}

void exceptionThrow(ExceptionInstance exception) {
    if (exceptionContextStack == NULL) {
        fprintf(stderr, "Terminate cause by the exception [%s]: %s\n", exception.type.name, exception.message);
        abort();
    } else {
        exceptionContextStack->exception = &exception;
        longjmp(exceptionContextStack->snapshot, exceptionContextStack->status == TryBlockInProgress ? TryBlockExceptionOccurred : TryBlockUncaughtException);
    }
}
