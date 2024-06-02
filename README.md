# Exceptions for C

This repository provides an simple implementation of exception handling in the C language.

Released into the public domain, see [LICENSE](LICENSE) for more information.

## Notices

- Variables that are modified in a 'TRY' block may need to mark `volatile` because of compiler optimizations.
- DO NOT use the keyword `return` in 'TRY' blocks, it will break the exception context stack. Use the macro `RETURN_IN_TRY` instead.
- An 'TRY' block can only have one `FINALLY` branch.
- All structure types and enumeration types have a typedef with the same name.

## All Identifiers

### 'TRY' Block Macros

| Identifier   | Parameters         | Description                                                                                                 |
| ------------ | ------------------ | ----------------------------------------------------------------------------------------------------------- |
| `TRY`        | -                  | The beginning of 'TRY' blocks.                                                                              |
| `CATCH`      | `(type, variable)` | Used to catch an exception of specific type and its child exception types, matching one from top to bottom. |
| `PASSED`     | -                  | Executes when no exception was thrown.                                                                      |
| `FINALLY`    | -                  | Always executes, even executed `RETURN_IN_TRY` in other branches.                                           |
| `END_TRY`    | -                  | The end of 'TRY' blocks.                                                                                    |

### Functionality Macros

| Identifier              | Parameters             | Description                                                                      |
| ----------------------- | ---------------------- | -------------------------------------------------------------------------------- |
| `RETURN_IN_TRY`         | `(value)`              | The correct way to return in 'TRY' blocks.                                       |
| `THROW_NEW`             | `(type, message)`      | Throws a new exception.                                                          |
| `THROW`                 | `(exception)`          | Throws an existing exception instance.                                           |
| `NEW_EXCEPTION`         | `(type, message)`      | Creates a new exception instance.                                                |
| `PRINT_EXCEPTION_INFO`  | `(exception)`          | Prints the exception information to stderr.                                      |
| `EXCEPTION_INSTANCE_OF` | `(exception, type)`    | Determines the exception instance is an instance of the specific exception type. |
| `DECLARE_EXCEPTION`     | `(name, parent)`       | Declares a new exception type (usually in a header file).                        |
| `DEFINE_EXCEPTION`      | `(name, parent)`       | Defines a declared exception type (usually in a source file).                    |

### Exception Types

- Actually global constants of the type `ExceptionType`.

| Identifier   | Parent | Description                                                        |
| ------------ | ------ | ------------------------------------------------------------------ |
| `Exception`  | -      | Top level exception type, the parent of all other exceptions types |

### Structure Types

#### ExceptionType

- The type of exception types.

| Member       | Type                          | Description                |
| ------------ | ----------------------------- | -------------------------- |
| `name`       | `const char* const`           | The name of exception.     |
| `parent`     | `const ExceptionType* const`  | The parent exception type. |

#### ExceptionInstance

- The exception instance type.

| Member       | Type                   | Description                               |
| ------------ | ---------------------- | ----------------------------------------- |
| `type`       | `const ExceptionType*` | The exception type.                       |
| `message`    | `const char*`          | The message provided about the exception. |

#### ExceptionContext

- The exception context type.

| Member       | Type                        | Description                                                    |
| ------------ | --------------------------- | -------------------------------------------------------------- |
| `link`       | `ExceptionContext*`         | The previous exception context in the exception context stack. |
| `status`     | `TryBlockStatus`            | The status of the 'TRY' block.                                 |
| `finally`    | `bool`                      | Whether the code in the `FINALLY` branch has been executed.    |
| `exception`  | `ExceptionInstance`         | The exception instance if there is an exception occurred.      |
| `buf`        | `jmp_buf`                   | Saves information for code jumping.                            |

### Enumeration Types

#### TryBlockStatus

- Defines all the status of 'TRY' blocks.

| Constant                    | Description                                                           |
| --------------------------- | --------------------------------------------------------------------- |
| `TryBlockEntered`           | Entered the 'TRY' block.                                              |
| `TryBlockInProgress`        | Executing the code in the `TRY` branch.                               |
| `TryBlockNoException`       | No exception occurred after executing the `TRY` branch.               |
| `TryBlockExceptionOccurred` | An exception occurred in the `TRY` branch.                            |
| `TryBlockCaughtException`   | The exception has been caught.                                        |
| `TryBlockExceptionLeak`     | An exception occurred in the 'TRY' block but not in the `TRY` branch. |
| `TryBlockInterrupted`       | Interrupted the 'TRY' block by `RETURN_IN_TRY`.                       |
| `TryBlockToReturn`          | Instructs to return after code jumping.                               |

### Typedefs

| Identifier                 | Type                                 | Description                             |
| -------------------------- | ------------------------------------ | --------------------------------------- |
| `UncaughtExceptionHandler` | `void (*)(const ExceptionInstance*)` | Type of the uncaught exception handler. |

### Functions

| Identifier             | Signature                                                              | Description                                                                      |
| ---------------------- | ---------------------------------------------------------------------- | -------------------------------------------------------------------------------- |
| `throwException`       | `void (ExceptionInstance exception)`                                   | Throws an exception instance.                                                    |
| `newException`         | `ExceptionInstance (const ExceptionType* type, const char* message)`   | Creates a new exception instance.                                                |
| `exceptionInstanceOf`  | `bool (const ExceptionInstance* exception, const ExceptionType* type)` | Determines the exception instance is an instance of the specific exception type. |
| `terminateByException` | `void (const ExceptionInstance* exception)`                            | Default uncaught exception handler.                                              |

### Global Variables

| Identifier                 | Type                       | Description                     |
| -------------------------- | -------------------------- | ------------------------------- |
| `uncaughtExceptionHandler` | `UncaughtExceptionHandler` | The uncaught exception handler. |
| `exceptionContextStack`    | `ExceptionContext*`        | The exception context stack.    |

## Example

Source code:

```c
#include "exception.h"

int main() {
    const char* volatile str = "Hello";
    printf("%s\n", str); // Outputs 'Hello'.

    TRY {
        str = "OK";
        THROW_NEW(Exception, "this is an exeption");
        str = ""; // Never reaches here.
    } CATCH (Exception, exception) {
        // Outputs the exception information.
        PRINT_EXCEPTION_INFO(exception);
    } PASSED {
        printf("Passed the 'TRY' branch\n");
    } FINALLY {
        printf("Finally\n");
    } END_TRY;

    printf("%s\n", str); // Outputs 'OK'.

    return 0;
}
```

Output:

```
Hello
Exception: this is an exeption
Finally
OK
```
