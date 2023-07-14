# Exceptions for C

Licensed under the [MIT License](LICENSE).

This repository provides an simple implementation for exceptions in the C language.

## Notices

- Variables that are modified in a 'TRY-CATCH' block may need to mark `volatile` because of compiler optimizations.
- DO NOT use the keyword `return` in the 'TRY-CATCH' block, it will break the exception context stack. Use the macro `RETURN_IN_TRY` instead.
- All structure types and enumeration types has typedefs, so prefixes (`struct` or `enum`) can be omitted when using them.

## All Identifiers

### 'TRY-CATCH' Block Macros

| Identifier   | Parameters         | Description                                                                          |
| ------------ | ------------------ | ------------------------------------------------------------------------------------ |
| `TRY`        | -                  | Begin of the 'TRY-CATCH' block.                                                      |
| `CATCH`      | `(type, variable)` | Use to catch an exception of specific type and its its child exceptions, match once. |
| `PASSED`     | -                  | Execute when no exception was thrown.                                                |
| `FINALLY`    | -                  | Always executes, even executed `RETURN_IN_TRY` in other branches.                    |
| `END_TRY`    | -                  | End of the 'TRY-CATCH' block.                                                        |

### Functionality Macros

| Identifier              | Parameters             | Description                                                                     |
| ----------------------- | ---------------------- | ------------------------------------------------------------------------------- |
| `RETURN_IN_TRY`         | `(value)`              | The right way to return in the 'TRY-CATCH' block.                               |
| `THROW_NEW`             | `(type, message)`      | Throw a new exception.                                                          |
| `THROW`                 | `(exception)`          | Throw an existing exception instance.                                           |
| `NEW_EXCEPTION`         | `(type, message)`      | Create a new exception instance.                                                |
| `PRINT_EXCEPTION_INFO`  | `(exception)`          | Print the exception information.                                                |
| `EXCEPTION_INSTANCE_OF` | `(exception, type)`    | Determine the exception instance is an instance of the specific exception type. |
| `DECLARE_EXCEPTION`     | `(name)`               | Declare a new exception type in the header file.                                |
| `DEFINE_EXCEPTION`      | `(name, code, parent)` | Define a declared exception type in the source file.                            |

### Exception Types

- Actually global constants.

| Identifier   | Code                 | Parent | Description                                                 |
| ------------ | -------------------- | ------ | ----------------------------------------------------------- |
| `Exception`  | `0xFFFFFFFFFFFFFFFF` | -      | Top level exception type, the parent of all exceptions type |

### Structure types

#### ExceptionType

- Structure of all exception types.

| Member       | Type                          | Description                |
| ------------ | ----------------------------- | -------------------------- |
| `name`       | `const char* const`           | The name of exception.     |
| `code`       | `const uint64_t`              | The code of exception.     |
| `parent`     | `const ExceptionType* const`  | The parent exception type. |

#### ExceptionInstance

- The exception instance type.

| Member       | Type                  | Description                               |
| ------------ | --------------------- | ----------------------------------------- |
| `type`       | `const ExceptionType` | The exception type.                       |
| `message`    | `const char* const`   | The message provided about the exception. |

#### ExceptionContext

- The exception context type.

| Member       | Type                        | Description                                                    |
| ------------ | --------------------------- | -------------------------------------------------------------- |
| `status`     | `TryBlockStatus`            | The status of the 'TRY-CATCH' block.                           |
| `snapshot`   | `jmp_buf`                   | Save information for code jumps.                               |
| `exception`  | `const ExceptionInstance*`  | The exception instance if there is an exception occurred.      |
| `finally`    | `bool`                      | Whether the code in the `FINALLY` block has been executed.     |
| `link`       | `ExceptionContext*`         | The previous exception context in the exception context stack. |

### Enumeration types

#### TryBlockStatus

- Defines all the status of the 'TRY-CATCH' block.

| Constant                    | Description                                                                 |
| --------------------------- | --------------------------------------------------------------------------- |
| `TryBlockEnter`             | Enter the 'TRY-CATCH' block.                                                |
| `TryBlockInProgress`        | Executing the code in the 'TRY' branch.                                     |
| `TryBlockNoException`       | No exception occurred.                                                      |
| `TryBlockExceptionOccurred` | An exception occurred in the 'TRY' branch.                                  |
| `TryBlockCaughtException`   | The exception has been caught.                                              |
| `TryBlockExceptionOutflow`  | An exception occurred in the 'TRY-CATCH' block but not in the `TRY` branch. |
| `TryBlockInterrupted`       | Interrupted the 'TRY-CATCH' block by `RETURN_IN_TRY`.                       |
| `TryBlockToReturn`          | Indicates that the function will be returned.                               |

### Typedefs

| Identifier                 | Type                          | Description                             |
| -------------------------- | ----------------------------- | --------------------------------------- |
| `UncaughtExceptionHandler` | `void (*)(ExceptionInstance)` | Type of the uncaught exception handler. |

### Functions

| Identifier                  | Signature                                                        | Description                                                             |
| --------------------------- | ---------------------------------------------------------------- | ----------------------------------------------------------------------- |
| `throwException`            | `void (ExceptionInstance exception)`                             | See `EXCEPTION_THROW`.                                                  |
| `newException`              | `ExceptionInstance (ExceptionType* type, const char* message)`   | See `NEW_EXCEPTION`.                                                    |
| `exceptionInstanceOf`       | `bool (ExceptionInstance* exception, ExceptionType type)`        | See `EXCEPTION_INSTANCE_OF`.                                            |
| `exceptionContextStackPush` | `void (ExceptionContext* exceptionContext)`                      | Push an exception context into the exception context stack.             |
| `exceptionContextStackPop`  | `void ()`                                                        | Pop-up the exception context at the top of the exception context stack. |
| `exceptionTerminate`        | `void (ExceptionInstance exception)`                             | Default uncaught exception handler.                                     |

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
    const char* volatile str = "hello";
    printf("%s\n", str); // Output 'hello'.

    TRY { // Begin of the 'TRY-CATCH' block.
        str = "ok";
        THROW_NEW(Exception, "exception");
        str = ""; // Never reaching here.
    } CATCH (Exception, exception) {
        // Output the exception information.
        PRINT_EXCEPTION_INFO(exception);
    } PASSED {
        printf("passed the 'TRY' branch\n");
    } FINALLY {
        printf("finally\n");
    } END_TRY; // End of the 'TRY-CATCH' block.

    printf("%s\n", str); // Output 'ok'.

    return 0;
}
```

Output:

```
hello
Exception occured [Exception]: exception
finally
ok
```
