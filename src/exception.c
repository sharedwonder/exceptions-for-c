/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org/>
 */

#include "exception.h"

#include <stdlib.h>

const ExceptionType Exception = {"Exception", NULL};

UncaughtExceptionHandler uncaughtExceptionHandler = terminateByException;

ExceptionContext* exceptionContextStack = NULL;

void throwException(ExceptionInstance exception) {
    if (exceptionContextStack == NULL) {
        if (uncaughtExceptionHandler != NULL) {
            uncaughtExceptionHandler(&exception);
        }
        abort();
    } else {
        exceptionContextStack->exception = exception;
        TryBlockStatus status = exceptionContextStack->status == TryBlockInProgress ? TryBlockExceptionOccurred : TryBlockExceptionLeak;
        longjmp(exceptionContextStack->buf, status);
    }
}

ExceptionInstance newException(const ExceptionType* type, const char* message) {
    const ExceptionInstance exception = {type, message};
    return exception;
}

bool exceptionInstanceOf(const ExceptionInstance* exception, const ExceptionType* type) {
    if (exception->type == type || type == &Exception) {
        return true;
    }

    const ExceptionType* parent = exception->type->parent;
    while (parent != NULL) {
        if (parent == type) {
            return true;
        }
        parent = parent->parent;
    }
    return false;
}

void terminateByException(const ExceptionInstance* exception) {
    PRINT_EXCEPTION_INFO(exception);
    abort();
}
