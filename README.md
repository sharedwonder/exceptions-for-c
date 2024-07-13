# Exceptions for C

This repository provides an simple implementation of exception handling in the C language.

Released into the public domain, see [LICENSE](LICENSE) for more information.

## Limitations

- Variables that are modified in a 'TRY' statement may need to qualify with `volatile` because of compiler optimizations.
- DO NOT use the keyword `return` in 'TRY' statements, it will break the exception context stack. Use the macro `RETURN_IN_TRY` instead.
- An 'TRY' statement can only have one `FINALLY` clause.
- Multithreading is not supported.

## All Identifiers

### 'TRY' Statement Macros

| Identifier   | Parameters         | Description                                                                                               |
| ------------ | ------------------ | --------------------------------------------------------------------------------------------------------- |
| `TRY`        | -                  | The beginning of 'TRY' statements.                                                                        |
| `CATCH`      | `(type, variable)` | Used to catch an exception of the specific type and its subtypes, matching only one in a 'TRY' statement. |
| `PASSED`     | -                  | Executes when no exception was thrown.                                                                    |
| `FINALLY`    | -                  | Always executes, even executed `RETURN_IN_TRY` in other clauses.                                          |
| `END_TRY`    | -                  | The end of 'TRY' statements.                                                                              |

### Other Macros

| Identifier                | Parameters                  | Description                                                                                                                                     |
| ------------------------- | --------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| `RETURN_IN_TRY`           | `(value)`                   | The correct way to return in 'TRY' statements.                                                                                                  |
| `THROW_NEW`               | `(type, message)`           | Creates a new exception instance and throws it.                                                                                                 |
| `THROW`                   | `(exception)`               | Throws an exception instance.                                                                                                                   |
| `CREATE_EXCEPTION`        | `(type, variable, message)` | Defines a variable that holds a new exception instance.                                                                                         |
| `PRINT_EXCEPTION_INFO`    | `(exception)`               | Prints the exception information to stderr.                                                                                                     |
| `PRINT_EXCEPTION_INFO_TO` | `(exception, stream)`       | Prints the exception information to the specified stream.                                                                                       |
| `EXCEPTION_INSTANCE_OF`   | `(exception, type)`         | Determines the exception instance is an instance of the specific exception type.                                                                |
| `DECLARE_EXCEPTION`       | `(name, parent)`            | Declares a new exception type (usually used in a header file).                                                                                  |
| `DEFINE_EXCEPTION`        | `(name, parent)`            | Defines a declared exception type (usually used in a source file).                                                                              |
| `NO_FINALLY_CLAUSE`       | -                           | Using this at the beginning of the `TRY` clause, indicates that there is no `FINALLY` clause, so using `RETURN_IN_TRY` will return immediately. |

### Exception Types

- Actually global constants of the type `ExceptionType`.

| Identifier   | Parent | Description                                                         |
| ------------ | ------ | ------------------------------------------------------------------- |
| `Exception`  | -      | Top level exception type, the parent of all other exceptions types. |

### Structure Types

- All structure types have a typedef with the same name.

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
| `message`    | `const char*`          | The message of the exception.             |
| `file`       | `const char*`          | The file where the exception created.     |
| `line`       | `int`                  | The line where the exception created.     |

#### ExceptionContext

- The exception context type.

| Member       | Type                        | Description                                                                                      |
| ------------ | --------------------------- | ------------------------------------------------------------------------------------------------ |
| `state`      | `TryState`                  | The state of the 'TRY' statement.                                                                |
| `finally`    | `bool`                      | Whether the `FINALLY` clause has been executed (or the 'TRY' statement has no `FINALLY` clause). |
| `exception`  | `ExceptionInstance`         | The exception instance if there is an exception occurred.                                        |
| `buffer`     | `jmp_buf`                   | Stores information for code jumping.                                                             |
| `link`       | `ExceptionContext*`         | The previous exception context in the exception context stack.                                   |

### Enumeration Types

- All enumeration types have a typedef with the same name.

#### TryState

- Defines all the status of 'TRY' statements.

| Constant                    | Description                                                               |
| --------------------------- | ------------------------------------------------------------------------- |
| `TryStateBeginning`         | Started executing a 'TRY' statement.                                      |
| `TryStateTrying`            | Executing the `TRY` clause.                                               |
| `TryStateNoException`       | No exception occurred after executing the `TRY` clause.                   |
| `TryStateExceptionOccurred` | An exception occurred in the `TRY` clause.                                |
| `TryStateCaughtException`   | The exception has been caught.                                            |
| `TryStateExceptionRaised`   | An exception occurred in the 'TRY' statement but not in the `TRY` clause. |
| `TryStateInterrupted`       | Interrupted the 'TRY' statement by `RETURN_IN_TRY`.                       |
| `TryStateFunctionReturn`    | Instructs to return after code jumping.                                   |

### Typedefs

| Identifier                 | Type                              | Description                              |
| -------------------------- | --------------------------------- | ---------------------------------------- |
| `UncaughtExceptionHandler` | `void (const ExceptionInstance*)` | The type of uncaught exception handlers. |

### Functions

| Identifier             | Signature                                                              | Description                                                                                |
| ---------------------- | ---------------------------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| `throwException`       | `void (const ExceptionInstance* exception)`                            | Throws an exception instance.                                                              |
| `printExceptionInfo`   | `void (const ExceptionInstance* exception, FILE* stream)`              | Prints the exception information to the specified stream.                                  |
| `exceptionInstanceOf`  | `bool (const ExceptionInstance* exception, const ExceptionType* type)` | Determines the exception instance is an instance of the specific exception type.           |

### Global Variables

| Identifier                 | Type                        | Description                                                                             |
| -------------------------- | --------------------------- | --------------------------------------------------------------------------------------- |
| `exceptionContextStack`    | `ExceptionContext*`         | The exception context stack.                                                            |
| `uncaughtExceptionHandler` | `UncaughtExceptionHandler*` | The uncaught exception handler (defaults to print the exception information to stderr). |

## Example

Source code:

```c
#include <stdio.h>
#include <exception.h>

int main() {
    volatile int number = 1;
    printf("Number: %d\n", number);

    TRY {
        printf("TRY\n");
        number = 2;
        THROW_NEW(Exception, "exeption message");
        number = 3;
    } CATCH (Exception, exception) {
        printf("CATCH\n");
        PRINT_EXCEPTION_INFO(exception);
    } PASSED {
        printf("PASSED\n");
    } FINALLY {
        printf("FINALLY\n");
    } END_TRY;

    printf("Number: %d\n", number);

    return 0;
}
```

Output:

```
Number: 1
TRY
CATCH
Exception (file "example.c", line 11): exeption message
FINALLY
Number: 2
```
