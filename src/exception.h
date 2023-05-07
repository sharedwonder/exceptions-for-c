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
    TryBlockEnter,
    TryBlockInProgress,
    TryBlockNoException,
    TryBlockExceptionOccurred,
    TryBlockUncaughtException,
    TryBlockCaughtException,
    TryBlockInterrupted
} TryBlockStatus;

typedef struct ExceptionContext {
    TryBlockStatus status;
    jmp_buf snapshot;
    ExceptionInstance* exception;
    bool finally;
    struct ExceptionContext* link;
} ExceptionContext;

#ifdef __cplusplus
extern "C" {
#endif

extern ExceptionContext* exceptionContextStack;

void exceptionContextStackPush(ExceptionContext* exceptionContext);

void exceptionContextStackPopup();

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
        if (_CONTEXT.status == TryBlockEnter) { \
            _CONTEXT.status = TryBlockInProgress;

#define CATCH(exceptionType, variable) \
        } \
        if (_CONTEXT.status == TryBlockInProgress) _CONTEXT.status = TryBlockNoException; \
        else if (_CONTEXT.status == TryBlockExceptionOccurred && exceptionInstanceOf(_CONTEXT.exception, (exceptionType))) { \
            ExceptionInstance* (variable) = _CONTEXT.exception; \
            _CONTEXT.status = TryBlockCaughtException;

#define PASSED \
        } \
        if (_CONTEXT.status == TryBlockInProgress) _CONTEXT.status = TryBlockNoException; \
        if (_CONTEXT.status == TryBlockNoException) {

#define FINALLY \
        } \
        if (_CONTEXT.status == TryBlockInProgress) _CONTEXT.status = TryBlockNoException; \
        if (!_CONTEXT.finally) { \
            _CONTEXT.finally = true; \

#define END_TRY \
        } \
            exceptionContextStackPopup(); \
            if (_CONTEXT.status == TryBlockInterrupted) { \
                longjmp(_CONTEXT.snapshot, -1); \
            } \
            if (_CONTEXT.status == TryBlockExceptionOccurred || _CONTEXT.status == TryBlockUncaughtException) { \
                exceptionThrow(*_CONTEXT.exception); \
            } \
    } \
    (void) 0

#define RETURN_IN_TRY(value) \
    { \
        if (_CONTEXT.finally) { \
            exceptionContextStackPopup(); \
            return value; \
        } \
        jmp_buf snapshot; \
        memcpy(snapshot, _CONTEXT.snapshot, sizeof(jmp_buf)); \
        if (setjmp(_CONTEXT.snapshot) == -1) { \
            return value; \
        } else { \
            longjmp(snapshot, TryBlockInterrupted); \
        } \
    } \
    (void) 0

#define THROW_NEW(type, message) exceptionThrow(exceptionNew(type, message))

#define THROW exceptionThrow

#define NEW_EXCEPTION exceptionNew

#define PRINT_EXCEPTION_INFO(exception) fprintf(stderr, "Exception occured [%s]: %s\n", exception->type.name, exception->message);

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
