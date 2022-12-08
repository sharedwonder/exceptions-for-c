/*
 * Copyright (C) 2022 Liu Baihao. All rights reserved.
 * This source code is licensed under the MIT License.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <setjmp.h>
#include <memory.h>

typedef struct ExceptionType {
    const char* name;
    uint64_t code;
    struct ExceptionType* parent;
} ExceptionType;

typedef struct ExceptionInstance {
    ExceptionType type;
    const char* message;
} ExceptionInstance;

typedef enum TryBlockStatus {
    TryBlockInto,
    TryBlockNoException,
    TryBlockExceptionOccurred,
    TryBlockExceptionCapture,
    TryBlockInterrupted,
    TryBlockFunctionReturn
} TryBlockStatus;

typedef struct ExceptionContext {
    ExceptionInstance* exception;
    jmp_buf snapshot;
    bool inFinally;
    TryBlockStatus status;
    struct ExceptionContext* link;
} ExceptionContext;

#ifdef __cplusplus
extern "C" {
#endif

void exceptionContextStackPush(ExceptionContext* exceptionContext);

void exceptionContextStackPopup();

bool exceptionContextStackIsEmpty();

ExceptionInstance exceptionNew(ExceptionType type, const char* message);

bool exceptionInstanceOf(ExceptionInstance* exception, ExceptionType type);

void exceptionThrow(ExceptionInstance exception);

#ifdef __cplusplus
}
#endif

#define TRY \
    { \
        ExceptionContext _CONTEXT; \
        exceptionContextStackPush(&_CONTEXT); \
        _CONTEXT.status = setjmp(_CONTEXT.snapshot); \
        if (_CONTEXT.status == TryBlockInto) { \
            _CONTEXT.status = TryBlockNoException;

#define CATCH(exceptionType, variable) \
        } else if (_CONTEXT.status == TryBlockExceptionOccurred && exceptionInstanceOf(_CONTEXT.exception, (exceptionType))) { \
            ExceptionInstance* (variable) = _CONTEXT.exception; \
            _CONTEXT.status = TryBlockExceptionCapture;

#define PASSED \
        } \
        _CONTEXT.inFinally = false; \
        if (_CONTEXT.status == TryBlockNoException) {

#define FINALLY \
        } \
        _CONTEXT.inFinally = true; \
        {

#define END_TRY \
        } \
            exceptionContextStackPopup(); \
            if (_CONTEXT.status == TryBlockInterrupted) { \
                longjmp(_CONTEXT.snapshot, TryBlockFunctionReturn); \
            } \
            if (_CONTEXT.status == TryBlockExceptionOccurred) { \
                exceptionThrow(*_CONTEXT.exception); \
            } \
    } \
    (void) 0

#define RETURN_IN_TRY(value) \
    { \
        if (_CONTEXT.inFinally) { \
            exceptionContextStackPopup(); \
            return value; \
        } \
        Snapshot snapshot; \
        memcpy(snapshot, _CONTEXT.snapshot, sizeof(Snapshot)); \
        if (setjmp(_CONTEXT.snapshot) == TryBlockFunctionReturn) { \
            return value; \
        } else { \
            longjmp(snapshot, TryBlockInterrupted); \
        } \
    } \
    (void) 0

#define THROW_NEW(type, message) exceptionThrow(exceptionNew(type, message))

#define THROW exceptionThrow

#define NEW_EXCEPTION exceptionNew

#define PRINT_EXCEPTION_INFO(exception) fprintf(stderr, "Exception occured [%s]: %s", exception->type.name, exception->message);

#define EXCEPTION_INSTANCE_OF exceptionInstanceOf

#define DECLARE_EXCEPTION(name) \
    extern const ExceptionType name;

#define DEFINE_EXCEPTION(_name, _code, _parent) \
    const ExceptionType _name = { \
        .name = #_name, \
        .code = _code, \
        .parent = _parent \
    }

DECLARE_EXCEPTION(Exception)
