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

ExceptionContext* exceptionContextStack = NULL;

static UncaughtExceptionHandler defaultUncaughtExceptionHandler;

UncaughtExceptionHandler* uncaughtExceptionHandler = defaultUncaughtExceptionHandler;

void throwException(const ExceptionInstance* exception) {
    if (exceptionContextStack == NULL) {
        if (uncaughtExceptionHandler != NULL) {
            uncaughtExceptionHandler(exception);
        }
        abort();
    } else {
        exceptionContextStack->exception = *exception;
        TryState state = exceptionContextStack->state == TryStateTrying ? TryStateExceptionOccurred : TryStateExceptionRaised;
        longjmp(exceptionContextStack->buffer, state);
    }
}

void printExceptionInfo(const ExceptionInstance* exception, FILE* stream) {
    if (exception->file == NULL) {
        fprintf(stream, "%s (unknown source)", exception->type->name);
    } else if (exception->line <= 0) {
        fprintf(stream, "%s (file \"%s\")", exception->type->name, exception->file);
    } else {
        fprintf(stream, "%s (file \"%s\", line %d)", exception->type->name, exception->file, exception->line);
    }
    if (exception->message != NULL) {
        fputs(": ", stream);
        fputs(exception->message, stream);
        fputc('\n', stream);
    }
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

static void defaultUncaughtExceptionHandler(const ExceptionInstance* exception) {
    printExceptionInfo(exception, stderr);
}
