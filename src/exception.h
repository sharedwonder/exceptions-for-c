/*
 * Copyright (C) 2023 Liu Baihao (sharedwonder). All rights reserved.
 * This source code is licensed under the MIT License.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <setjmp.h>
#include <memory.h>

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
            const ExceptionInstance* (variable) = _CONTEXT.exception; \
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
                longjmp(_CONTEXT.snapshot, TryBlockToReturn); \
            } \
            if (_CONTEXT.status == TryBlockExceptionOccurred || _CONTEXT.status == TryBlockExceptionOutflow) { \
                throwException(*_CONTEXT.exception); \
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
        if (setjmp(_CONTEXT.snapshot) == TryBlockToReturn) { \
            return value; \
        } \
        longjmp(snapshot, TryBlockInterrupted); \
    } \
    (void) 0

#define THROW_NEW(type, message) throwException(newException((type), (message)))

#define THROW throwException

#define NEW_EXCEPTION newException

#define PRINT_EXCEPTION_INFO(exception) fprintf(stderr, "Exception occured [%s]: %s\n", (exception)->type.name, (exception)->message);

#define EXCEPTION_INSTANCE_OF exceptionInstanceOf

#define DECLARE_EXCEPTION(name) \
    extern const ExceptionType name;

#define DEFINE_EXCEPTION(_name, _code, _parent) \
    const ExceptionType _name = {#_name, (_code), &(_parent)}

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _EXCEPTION_FOR_C_SHARED_LIBRARY
    #ifdef _WIN32
        #ifdef _EXCEPTION_FOR_C_BUILDING
            #define _EXCEPTION_FOR_C_API __declspec(dllexport)
        #else
            #define _EXCEPTION_FOR_C_API __declspec(dllimport)
        #endif
    #elif defined(__GNUC__)
        #define _EXCEPTION_FOR_C_API __attribute__((visibility("default")))
    #else
        #define _EXCEPTION_FOR_C_API
    #endif
#else
    #define _EXCEPTION_FOR_C_API
#endif

typedef struct ExceptionType {
    const char* const name;
    const uint64_t code;
    const struct ExceptionType* const parent;
} ExceptionType;

typedef struct ExceptionInstance {
    const ExceptionType type;
    const char* const message;
} ExceptionInstance;

typedef enum TryBlockStatus {
    TryBlockEnter,
    TryBlockInProgress,
    TryBlockNoException,
    TryBlockExceptionOccurred,
    TryBlockCaughtException,
    TryBlockExceptionOutflow,
    TryBlockInterrupted,
    TryBlockToReturn
} TryBlockStatus;

typedef struct ExceptionContext {
    TryBlockStatus status;
    jmp_buf snapshot;
    const ExceptionInstance* exception;
    bool finally;
    struct ExceptionContext* link;
} ExceptionContext;

typedef void (*UncaughtExceptionHandler)(ExceptionInstance);

_EXCEPTION_FOR_C_API extern UncaughtExceptionHandler uncaughtExceptionHandler;

_EXCEPTION_FOR_C_API extern ExceptionContext* exceptionContextStack;

_EXCEPTION_FOR_C_API DECLARE_EXCEPTION(Exception)

_EXCEPTION_FOR_C_API void throwException(const ExceptionInstance exception);

_EXCEPTION_FOR_C_API ExceptionInstance newException(ExceptionType type, const char* message);

_EXCEPTION_FOR_C_API bool exceptionInstanceOf(const ExceptionInstance* exception, ExceptionType type);

_EXCEPTION_FOR_C_API void exceptionContextStackPush(ExceptionContext* exceptionContext);

_EXCEPTION_FOR_C_API void exceptionContextStackPopup();

_EXCEPTION_FOR_C_API void exceptionTerminate(ExceptionInstance exception);

#undef _EXCEPTION_FOR_C_API

#ifdef __cplusplus
}
#endif
