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

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #if __STDC_VERSION__ < 202311L
        #include <stdbool.h>
    #endif
#elif !defined(__cplusplus)
    #define bool unsigned char
    #define true 1
    #define false 0
#endif

#ifdef _EXCEPTIONS_FOR_C_SHARED_LIBRARY
    #if defined(_WIN32) || defined(__CYGWIN__)
        #ifdef _EXCEPTIONS_FOR_C_BUILDING
            #define _EXCEPTIONS_FOR_C_API __declspec(dllexport)
        #else
            #define _EXCEPTIONS_FOR_C_API __declspec(dllimport)
        #endif
    #elif defined(__GNUC__) && __GNUC__ >= 4
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
    #else
        #define _NORETURN
    #endif
#elif defined(__STDC_VERSION__)
    #if __STDC_VERSION__ >= 202311L
        #define _NORETURN [[noreturn]]
    #elif __STDC_VERSION__ >= 201112L
        #define _NORETURN _Noreturn
    #else
        #define _NORETURN
    #endif
#else
    #define _NORETURN
#endif

#define TRY \
    { \
        ExceptionContext _context; \
        _context.link = exceptionContextStack; \
        exceptionContextStack = &_context; \
        _context.finally = false; \
        _context.state = setjmp(_context.buffer); \
        if (_context.state == TryStateBeginning) { \
            _context.state = TryStateTrying;

#define CATCH(type, variable) \
        } \
        if (_context.state == TryStateTrying) { \
            _context.state = TryStateNoException; \
        } else if (_context.state == TryStateExceptionOccurred && exceptionInstanceOf(&_context.exception, &(type))) { \
            _context.state = TryStateCaughtException; \
            const ExceptionInstance* const (variable) = &_context.exception;

#define PASSED \
        } \
        if (_context.state == TryStateTrying) { \
            _context.state = TryStateNoException; \
        } \
        if (_context.state == TryStateNoException) {

#define FINALLY \
        } \
        if (_context.state == TryStateTrying) { \
            _context.state = TryStateNoException; \
        } \
        if (!_context.finally) { \
            _context.finally = true;

#define END_TRY \
        } \
        exceptionContextStack = exceptionContextStack->link; \
        if (_context.state == TryStateInterrupted) { \
            longjmp(_context.buffer, TryStateFunctionReturn); \
        } \
        if (_context.state == TryStateExceptionOccurred || _context.state == TryStateExceptionRaised) { \
            throwException(&_context.exception); \
        } \
    } \
    (void) 0

#define RETURN_IN_TRY(value) \
    { \
        if (_context.finally) { \
            exceptionContextStack = exceptionContextStack->link; \
            return (value); \
        } \
        jmp_buf buffer; \
        memcpy(buffer, _context.buffer, sizeof(jmp_buf)); \
        if (setjmp(_context.buffer) == TryStateFunctionReturn) { \
            return (value); \
        } \
        longjmp(buffer, TryStateInterrupted); \
    } \
    (void) 0

#define THROW_NEW(type, message) \
    { \
        CREATE_EXCEPTION(type, _exception, message); \
        throwException(&_exception); \
    } \
    (void) 0

#define THROW(exception) \
    throwException(exception)

#define CREATE_EXCEPTION(type, variable, message) \
    const ExceptionInstance (variable) = {&(type), message, __FILE__, __LINE__}

#define PRINT_EXCEPTION_INFO(exception) \
    PRINT_EXCEPTION_INFO_TO(exception, stderr)

#define PRINT_EXCEPTION_INFO_TO(exception, stream) \
    printExceptionInfo(exception, stream)

#define EXCEPTION_INSTANCE_OF(exception, type) \
    exceptionInstanceOf(exception, &(type))

#define DECLARE_EXCEPTION(name, parnet) \
    extern const ExceptionType (name)

#define DEFINE_EXCEPTION(name, parent) \
    const ExceptionType (name) = {#name, &(parent)}

#define NO_FINALLY_CLAUSE \
    _context.finally = true

typedef struct ExceptionType {
    const char* const name;
    const struct ExceptionType* const parent;
} ExceptionType;

typedef struct ExceptionInstance {
    const ExceptionType* type;
    const char* message;
    const char* file;
    int line;
} ExceptionInstance;

typedef enum TryState {
    TryStateBeginning,
    TryStateTrying,
    TryStateNoException,
    TryStateExceptionOccurred,
    TryStateCaughtException,
    TryStateExceptionRaised,
    TryStateInterrupted,
    TryStateFunctionReturn
} TryState;

typedef struct ExceptionContext {
    TryState state;
    bool finally;
    ExceptionInstance exception;
    jmp_buf buffer;
    struct ExceptionContext* link;
} ExceptionContext;

typedef void UncaughtExceptionHandler(const ExceptionInstance*);

#ifdef __cplusplus
extern "C" {
#endif

_EXCEPTIONS_FOR_C_API
extern const ExceptionType Exception;

_EXCEPTIONS_FOR_C_API
extern ExceptionContext* exceptionContextStack;

_EXCEPTIONS_FOR_C_API
extern UncaughtExceptionHandler* uncaughtExceptionHandler;

_NORETURN _EXCEPTIONS_FOR_C_API 
void throwException(const ExceptionInstance* exception);

_EXCEPTIONS_FOR_C_API
void printExceptionInfo(const ExceptionInstance* exception, FILE* stream);

_EXCEPTIONS_FOR_C_API
bool exceptionInstanceOf(const ExceptionInstance* exception, const ExceptionType* type);

#ifdef __cplusplus
}
#endif

#undef _EXCEPTIONS_FOR_C_API
#undef _NORETURN

#endif
