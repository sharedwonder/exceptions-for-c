# Exceptions for C

Licensed under the [MIT License](LICENSE).

This repository provides an implementation for exceptions in the C language.

## All Identifiers

### 'TRY' Block Macros

| Identifier | Description |
| - | - |
| TRY | Begin of the 'TRY' block. |
| CATCH(type, variable) | Use to catch an exception of specific type, match once. |
| PASSED | Execute when no exception was thrown. |
| FINALLY | Always executes, even executed 'RETURN_IN_TRY(value)'. |
| END_TRY | End of the 'TRY' block. |

### Functionality Macros

| Identifier | Description |
| - | - |
| RETURN_IN_TRY(value) | The right way to return in the 'TRY' block. **DO NOT use the keyword 'return' in the 'TRY' block, it will break the exception context stack.** |
| THROW_NEW(type, message) | Throw a new exception. |
| THROW(exception) | Throw an existing exception instance. |
| NEW_EXCEPTION(type, message) | Create a new exception instance. |
| PRINT_EXCEPTION_INFO(exception) | Print the exception information. |
| EXCEPTION_INSTANCE_OF(exception, type) | Determine the exception instance is an instance of the specific exception type. |
| DECLARE_EXCEPTION(name) | Declare a new exception type in the header file. |
| DEFINE_EXCEPTION(name, code, parent) | Define a declared exception type in the source file. |

### Exception Types

| Identifier | Description |
| - | - |
| Exception | Top level exception type, the parent of all exceptions type |

### Functions

- Not recommended to use these functions directly, instead use the macros.

| Identifier | Description |
| - | - |
| exceptionContextStackPush(exceptionContext) | Push an exception context into the stack. |
| exceptionContextStackPop | Pop up an exception context from the stack. |
| exceptionContextStackIsEmpty | Determine the exception context stack is empty. |
| exceptionNew(type, message) | See functionality macro 'NEW_EXCEPTION'. |
| exceptionInstanceOf(exception, type) | See functionality macro 'EXCEPTION_INSTANCE_OF'. |
| exceptionThrow(exception) | See functionality macro 'EXCEPTION_THROW'. |

### Structures

| Identifier | Description |
| - | - |
| ExceptionType | Structure of all exception types. |
| ExceptionInstance | The exception instance type. |
| ExceptionContext | The exception context type. |

### Enums

| Identifier | Description |
| - | - |
| TryBlockStatus | Defines all the status of the 'TRY' block. |

## Example

```c
#include "exception.h"

int main() {
    char* str = "hello";
    printf("%s\n", str); // Output 'hello'.

    TRY { // Begin of the 'TRY' block.
        str = "ok";
        THROW_NEW(Exception, "exception");
        str = ""; // Never reaching here.
    } CATCH (Exception, exception) {
        PRINT_EXCEPTION_INFO(exception); // Output the exception information.
    } PASSED {
        printf("passed the 'TRY' block\n");
    } FINALLY {
        printf("finally\n");
    } END_TRY; // End of the 'TRY' block.

    printf("%s\n", str); // Output 'ok'.

    return 0;
}
```
