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

#ifndef _EXCEPTION_FOR_C
#define _EXCEPTION_FOR_C

#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ <= 201710L
    #include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TRY \
    { \
        ExceptionContext _CONTEXT; \
        _CONTEXT.link = exceptionContextStack; \
        exceptionContextStack = &_CONTEXT; \
        _CONTEXT.finally = false; \
        _CONTEXT.status = setjmp(_CONTEXT.buf); \
        if (_CONTEXT.status == TryBlockEntered) { \
            _CONTEXT.status = TryBlockInProgress;

#define CATCH(exceptionType, variable) \
        } \
        if (_CONTEXT.status == TryBlockInProgress) { \
            _CONTEXT.status = TryBlockNoException; \
        } else if (_CONTEXT.status == TryBlockExceptionOccurred && exceptionInstanceOf(&_CONTEXT.exception, &(exceptionType))) { \
            _CONTEXT.status = TryBlockCaughtException; \
            const ExceptionInstance* const (variable) = &_CONTEXT.exception;

#define PASSED \
        } \
        if (_CONTEXT.status == TryBlockInProgress) { \
            _CONTEXT.status = TryBlockNoException; \
        } \
        if (_CONTEXT.status == TryBlockNoException) {

#define FINALLY \
        } \
        if (_CONTEXT.status == TryBlockInProgress) { \
            _CONTEXT.status = TryBlockNoException; \
        } \
        if (!_CONTEXT.finally) { \
            _CONTEXT.finally = true; \

#define END_TRY \
        } \
        exceptionContextStack = exceptionContextStack->link; \
        if (_CONTEXT.status == TryBlockInterrupted) { \
            longjmp(_CONTEXT.buf, TryBlockToReturn); \
        } \
        if (_CONTEXT.status == TryBlockExceptionOccurred || _CONTEXT.status == TryBlockExceptionLeak) { \
            throwException(_CONTEXT.exception); \
        } \
    } \
    (void) 0

#define RETURN_IN_TRY(value) \
    { \
        if (_CONTEXT.finally) { \
            exceptionContextStack = exceptionContextStack->link; \
            return value; \
        } \
        jmp_buf buf; \
        memcpy(buf, _CONTEXT.buf, sizeof(jmp_buf)); \
        if (setjmp(_CONTEXT.buf) == TryBlockToReturn) { \
            return value; \
        } \
        longjmp(buf, TryBlockInterrupted); \
    } \
    (void) 0

#define THROW_NEW(type, message) \
    throwException(newException(&(type), message))

#define THROW(exception) \
    throwException(exception)

#define NEW_EXCEPTION(type, message) \
    newException(&(type), message)

#define PRINT_EXCEPTION_INFO(exception) \
    fprintf(stderr, "%s: %s\n", (exception)->type->name, (exception)->message)

#define EXCEPTION_INSTANCE_OF(exception, type) \
    exceptionInstanceOf(exception, &(type))

#define DECLARE_EXCEPTION(name, parnet) \
    extern const ExceptionType (name)

#define DEFINE_EXCEPTION(name, parent) \
    const ExceptionType (name) = {#name, &(parent)}

typedef struct ExceptionType {
    const char* const name;
    const struct ExceptionType* const parent;
} ExceptionType;

typedef struct ExceptionInstance {
    const ExceptionType* type;
    const char* message;
} ExceptionInstance;

typedef enum TryBlockStatus {
    TryBlockEntered,
    TryBlockInProgress,
    TryBlockNoException,
    TryBlockExceptionOccurred,
    TryBlockCaughtException,
    TryBlockExceptionLeak,
    TryBlockInterrupted,
    TryBlockToReturn
} TryBlockStatus;

typedef struct ExceptionContext {
    struct ExceptionContext* link;
    TryBlockStatus status;
    bool finally;
    ExceptionInstance exception;
    jmp_buf buf;
} ExceptionContext;

typedef void (*UncaughtExceptionHandler)(const ExceptionInstance*);

#ifdef _EXCEPTIONS_FOR_C_SHARED_LIBRARY
    #ifdef _WIN32
        #ifdef _EXCEPTIONS_FOR_C_BUILDING
            #define _EXCEPTIONS_FOR_C_API __declspec(dllexport)
        #else
            #define _EXCEPTIONS_FOR_C_API __declspec(dllimport)
        #endif
    #elif defined(__GNUC__)
        #define _EXCEPTIONS_FOR_C_API __attribute__((visibility("default")))
    #else
        #define _EXCEPTIONS_FOR_C_API
    #endif
#else
    #define _EXCEPTIONS_FOR_C_API
#endif

#ifdef __cplusplus
    #if defined(_MSVC_LANG) && _MSVC_LANG >= 201103L
        #define _NORETURN [[noreturn]]
    #elif __cplusplus >= 201103L
        #define _NORETURN [[noreturn]]
    #endif
#else
    #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L || (defined(__clang__) && __STDC_VERSION__ >= 201112L))
        #define _NORETURN [[noreturn]]
    #elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        #define _NORETURN [[_Noreturn]]
    #endif
#endif

#ifndef _NORETURN
    #if defined(__GNUC__)
        #define _NORETURN __attribute__((noreturn))
    #elif defined(_MSC_VER)
        #define _NORETURN __declspec(noreturn)
    #else
        #define _NORETURN
    #endif
#endif

_EXCEPTIONS_FOR_C_API
extern const ExceptionType Exception;

_EXCEPTIONS_FOR_C_API
extern UncaughtExceptionHandler uncaughtExceptionHandler;

_EXCEPTIONS_FOR_C_API
extern ExceptionContext* exceptionContextStack;

_NORETURN _EXCEPTIONS_FOR_C_API 
void throwException(ExceptionInstance exception);

_EXCEPTIONS_FOR_C_API
ExceptionInstance newException(const ExceptionType* type, const char* message);

_EXCEPTIONS_FOR_C_API
bool exceptionInstanceOf(const ExceptionInstance* exception, const ExceptionType* type);

_NORETURN _EXCEPTIONS_FOR_C_API
void terminateByException(const ExceptionInstance* exception);

#undef _EXCEPTIONS_FOR_C_API
#undef _NORETURN

#ifdef __cplusplus
}
#endif

#endif
