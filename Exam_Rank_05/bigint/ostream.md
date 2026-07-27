# what OS actually is
`std::ostream` is a class representing "a place we can write characters to." os. is a **reference to one specific such place**. It is not "the system" or "the TTY" directly - it's a stream object htat happens to be connected to a destination. 
The key realization: When we write.
std::cout << b;
the compiler rewrites it as a plain function call:
The compiler  rewrites is as a plain function call:

operator<<(std::cout, b);

so inside our function, `os` is in alias for `std::cout`. That's it. os is `std::cout`in this case -- same object, just referred to by the name `os` inside the function.

But `std::cout` isn't the only ostream. All of these are `std::ostream` s, so our **same** function works for all of them:

```cpp
std::cout << b;                 // os -> std::cout  (terminal / TTY)
myFile << b;                    // os -> ofstream   (a file)
std::ostringstream ss; ss << b  // os -> a string buffer in memory
```

That's the whole point of taking `std::ostream& os` instead of hardcoding `std::cout`: our function doesn't care where the output goes. Think of `os` as **conveyor belt** -- std::cout 's belt ends at the termina, a file's belt ends on disk. Our job is just "put the bigint's digits on whatever belt arrives."

## Why the & (reference)

two reasons:

1. **streams can't be copied**. We physically cannot pass one by value.
2. we must write to the **real**, not a copy - otherwise the output would go nowhere.

## Why it returns std::ostream&

to allow **chaining**. This line:

```cpp
std::cout <<"a= " << a <<std::endl
```

evaluates left-to-right as:

```cpp
(((std::cout << "a = ") << a) << std::endl);
```

Each << must hand back the stream so the next << has somethign to write into. `<< "a = "`  returns `std::cout`, then `<< a` writes to it and returns `std::cout` then `<< std::endl`. If our `operator<<` returned `void.`. The chain would break after the first line. That's why we return (os);

## What os << b.n does inside

`b.n` is an `int`. The standard library already has an `operator<<(std::ostream&, int). So we're not doing anything low-level - we're delegating to the built-in int-printer: "To display a bigint, display its n. " Later when n becomes a string or a vctor of digits, we'll change this one line to loop over the digits.

## One correction to our mental model
> I thought that was  ***overriding***- It's overloading. We didn't replace or modify the existing `<<`. We added a new candiate tot the specific pair. (the std::ostream&, const bigint&). All the exisitng `<<` versions (for int, char*, std::string...) still untouched. The compiler pickes the cmeatching one by the operand types. 

- Overload = add another version for difffernt parameter types (this)
- Override = repalce a virtual method in a derived class (not htis)


## In the nutshell.

Quick recacp of the mental model: os is just a name for whatever stream showed up on the left of `<<` usually `std::cout` (which pipes to our  terminal). But oit could be a file or a string bhffer. We take it by reference so we wrtie to the real one. We return it so teh next << in the chain can keep going, and os << b.n  resuses the library's existing int-printer to actually emit the characters.

Reminder for later: `operator=`needs the `bigint::` scope (it's a member), but `operator` does not -- It's a free function. Because its left operand is the stream, not our object.