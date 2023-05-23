# Exceptions for C

Licensed under the [MIT License](LICENSE).

This repository provides an implementation for exceptions in the C language.

## All Identifiers

- Note: The 'TRY' block refers to the entire block from 'TRY' to 'END_TRY', 'TRY' only means the code block after 'TRY'.

### 'TRY' Block Macros

| Identifier | Parameters | Description |
| - | - | - |
| TRY | - | Begin of the 'TRY' block. |
| CATCH | (type, variable) | Use to catch an exception of specific type and its its child exceptions, match once. |
| PASSED | - | Execute when no exception was thrown. |
| FINALLY | - | Always executes, even executed 'RETURN_IN_TRY(value)'. |
| END_TRY | - | End of the 'TRY' block. |

### Functionality Macros

| Identifier | Parameters | Description |
| - | - | - |
| RETURN_IN_TRY| (value) | The right way to return in the 'TRY' block. **DO NOT use the keyword 'return' in the 'TRY' block, it will break the exception context stack.** |
| THROW_NEW| (type, message) | Throw a new exception. |
| THROW| (exception) | Throw an existing exception instance. |
| NEW_EXCEPTION| (type, message) | Create a new exception instance. |
| PRINT_EXCEPTION_INFO | (exception) | Print the exception information. |
| EXCEPTION_INSTANCE_OF | (exception, type) | Determine the exception instance is an instance of the specific exception type. |
| DECLARE_EXCEPTION | (name) | Declare a new exception type in the header file. |
| DEFINE_EXCEPTION | (name, code, parent) | Define a declared exception type in the source file. |

### Exception Types

| Identifier | Description |
| - | - |
| Exception | Top level exception type, the parent of all exceptions type |

### Functions

- Not recommended to use these functions directly, instead use the macros.

| Identifier | Signature | Description |
| - | - | - |
| exceptionContextStackPush | void (ExceptionContext* exceptionContext) | Push an exception context into the exception context stack. |
| exceptionContextStackPop | void () | Pop the exception context at the top of the exception context stack. |
| exceptionNew | ExceptionInstance (ExceptionType\* type, const char\* message) | See functionality macro 'NEW_EXCEPTION'. |
| exceptionInstanceOf | bool (ExceptionInstance\* exception, ExceptionType type) | See functionality macro 'EXCEPTION_INSTANCE_OF'. |
| exceptionThrow | void (ExceptionInstance exception) | See functionality macro 'EXCEPTION_THROW'. |

### Global Variables

| Identifier | Type | Description |
| - | - | - |
| exceptionContextStack | ExceptionContext* | The exception context stack |

### Structures

#### ExceptionType

- Structure of all exception types.

| Identifier | Type | Description |
| - | - | - |
| name | const char\* | The name of exception. |
| code | uint64_t | The code of exception. |
| parent | ExceptionType\* | The parent exception type. |

#### ExceptionInstance

- The exception instance type.

| Identifier | Type | Description |
| - | - | - |
| type | ExceptionType | The exception type. |
| message | const char\* | The message provided about the exception. |

#### ExceptionContext

- The exception context type.

| Identifier | Type | Description |
| - | - | - |
| status | TryBlockStatus | The status of the 'TRY' block. |
| snapshot | jmp_buf | Save information for code jumps. |
| exception | ExceptionInstance\* | The exception instance if there is an exception occurred. |
| finally | bool | Whether the code in 'FINALLY' has been executed. |
| link | ExceptionContext\* | The previous exception context in the exception context stack. |

### Enums

#### TryBlockStatus

- Defines all the status of the 'TRY' block.

| Identifier | Description |
| - | - |
| TryBlockEnter | Enter the 'TRY' block. |
| TryBlockInProgress | Executing the code in 'TRY'. |
| TryBlockNoException | No exception occurred. |
| TryBlockExceptionOccurred | An exception occurred in 'TRY'. |
| TryBlockUncaughtException | An exception occurred out of 'TRY'. |
| TryBlockCaughtException | The exception has been caught. |
| TryBlockInterrupted | Interrupted the 'TRY' block by RETURN_IN_TRY, indicates that the function will be returned. |

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
