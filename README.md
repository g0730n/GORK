# GORK Programming Language

## General Optimized Reusable Keywords

A WIP Interpreted Language in C. The idea is it is supposed to be simple, but also powerful.

This is the 4th or 5th version of GORK. So far this one has been the best. I am learning C by writing this interpreter.

Below are features that will be implimented, and their progress:

- [x] Allocate memory for PROGRAM MEMORY, VARIABLE TABLE, and STACK
- [x] Function to visualize memory state
- [x] INTERPRET PROGRAM MEMORY. Add variables to STACK, and or VARIABLE TABLE
- [x] Determine whether or not a variable is a BUILTIN function, if so, do not add it to the variable table.
- [x] Determine whether or not variable on the STACK is already stored in VARIABLE TABLE, if so, do not re-add it.
- [ ] If variable on STACK is already in VARIABLE TABLE, update it's value, shrinking or growing VARIABLE TABLE if type/length of variable data is different than before.
- [ ] Functions to evaluate whole expressions
- [ ] Functions to parse functions when called
- [x] Functions to convert from any number type to another.
- [x] When storing a FUNCTION in the variable table, store the address it's function code starts in PROGRAM MEMORY.
- [ ] Use of SYMBOLS/WHITESPACE for flexible code style.
- [ ] Impliment storing of LISTS in VARIABLE TABLE
- [ ] STACK operations: Doing math on numbers, concationation of strings.
- [ ] INTERPRETER command line.
- [ ] Ability to EDIT or RUN PROGRAM in PROGRAM MEMORY from command line
- [ ] Ability to write programs in seperate text file then run them from commandline. eg "run myfile.txt"

### Example Program Code:

```
var 0 out var
func#
    out 0p
    0p+1
    con 0p<10 lpf
    ret 0p
    #
var func,var
out var

```

The output of this code would be:
```
>run
0
0
1
2
3
4
5
6
7
8
9
10
DONE!
>
```

Below examples of the same code, but with a different stype displaying the flexibility to substitute symbols for whitespace. In GORK, multiple whitespace in any form is condensed down to 1 whitepsace value upon interpretation.
A possible style:

```
var=0
out(var)
func#
    out(0p)
    0p=0p+1
    con(0p<10){lpf}
    ret(0p)
    #
var=func,var;
out(var)
```

Another Possible style:

```
var:0;
out:var;
func#{
  out:0p;
  0p+1;
  con:0p<10(
    lpf;
  )
  ret:0p;
  #
var:func,var;
out:var;
```

Most Minimal style:

```
var0out var func#out0p 0p+1 con0p<10 lpf ret0p#var func,var out var
```

Same code, but using brackets to display exactly what are seperate expressions. []brackets denote expressions,{}brackets denote function code:

```
[var 0]
[out var]
[func]#{
    [out 0p]
    [0p + 1]
    [con 0p < 10] [lpf]
    [ret 0p]
    }#
[var func,var]
[out var]
```

## RULES FOR INTERPRETING GORK

1. ### Whitespace Expression Seperation

Variable name has been followed by a value, then whitepsace(space or any other non used symbols, tabs, newlines, etc.

operators (+ with str or nums), -, /, *, % continue the expression. Outside of a CONDITIONAL(con), =, <, >, and ! can also be used as whitepace seperators.

```
  [var 4]                  var=4
  
  [firstvar secondvar]     firstvar=secondvar
  
  [var+2]                  var=value of var + 2
  
  [var 2+2]                var=2+2
  
  [var"hello "+"world!"]   var="hello "+"world!" (+ operator only can be used on str)
  
  [var2+"hi"]              var="2hi" (a string in expression always results in str)
  
  [var 32+var]             same as [var+32]
```

2. ### Rules for BUILTIN FUNCTIONS and VARIABLES

BUILTIN FUNCTIONS are "out", "ret", "lpf", and "con". More will be added later...

```
  [out2]                  prints "2"
  
  [outvar] [x+2]          creates var "outvar=x+2"
                          variables cannot be lumped together for example "out" and "var"
                          need whitepsace between them to work "out var". Otherwise instead
                          of assigning the value of "var" to "out" it will create a variable
                          named "outvar" then try to assign the next expression to it. In this
                          example it created a variable named "outvar" then read the next
                          expression "x+2". now "outvar=x+2" and "x" has been unchanged.
  
  [out var]               prints var
  
  [out"hello "+"world!"]
  [out"hello ","world!"]  prints "hello world!"
                          If a variable name is followed by a non alpha character, (whitespace,
                          symbol, or number) no seperator is needed.
  
  
  [out2+var/3]            prints var/3+2
  
  [func #]
or
  [func#]                 defines a function. stores func name and addr in variable
                          table. Function code block is skipped until function is called.
                          
  [func]                  a func call is always an expression to itself
  
  [func,2,3,2]            unless parameters are used.
  
  [func,"hello "]         a string parameter.
  
  [x func]                storing function return value in x
                          functions can never be redifined. Doing [func3] will never
                          create a new variable of "func" with value of 3, but consider
                          3 to be a parameter. if no parameters are used in function,
                          these values are simply discarded from stack when function has
                          finished running.
                          
  [x func,0]              sends "0" parameter to func, stores result in x
  
  [ret x]                 "ret" must always be followed by a value or variable
  [ret"DONE!"]
  [ret24]                 
  
  [lpf]                   "lpf" if called, starts function over again from beginning
  
  [con 3 < 10]            "con must always be followed by (value)(operator)(value)
                          Following the rules of using data types as seperators, the
                          previous expression can be written like this:
  [con3<10]
  
  [con x<100]             if x is less than 100. Here "x" needs whitespace after "con",
  [con100>x]              we can save one byte in program memory by reversing the
                          conditional.
  
  [con x="hello"]         if x equals "hello"
  
  [con100!x]              if 100 does not equal x
  
  [con100=x]              if 100 equals x
  
  [con1=1]                if followed by "lpf" this creates an infininte loop with no way
                          to break out of...
                          instead do:
  [con x=1]
or
  [con1=x]                then x can be changed to other than 1 and loop breaks
                          "con" must always be followed by (value)(operator)(value), if
                          it returns true(uint8_t '1'), then the next expression is evaluated.
                          if it returns false(uint8_t '0'), then the next expression is skipped.
                          
                          a function can be used as the next expression, eg.
  [x0]
  [con100>x][x myfunc,x]  this is really considered ONE expression as the conditional
                          will skip the function expression if it doesn't evalueate to
                          true. if it evaluates to true, it executes the function.
                          Below is the code for the whole function.
  [myfunc]#
          [ret 0p+1]
        or
          [ret1+0p]       parameters are indexed by their position (0p,1p,2p,3p...)
          #               in this case we sent the value of x:(0) as 0p, then add 1
                          to it and return it, which basically does "x=1". the condtion
                          checks that 100>1, and it's true, so it runs [x myfunc0] again.
                          another way of doing this is simpler:
  [myfunc]#[0p+1]
           [out0p]
           [con0p<100]
           [lpf]
           [ret0p]
          #
  [x myfunc,0]            this does the same thing basically, but prints out 1-100,
                          then at the end stores 100 in x. we can do the same thing,
                          by passing two parameters:
    [myfunc]#
           [0p+1]
           [out0p]
           [con0p<1p]
           [lpf]
          #
  [myfunc,0,100]        here we pass, 0 and 100 as the parameters. and instead of
                        storing the value at the end, we simply print 1-100.
```

3. ### Rules for LISTS and CONDITIONAL comparisons

```
  [mylist,100,200,300,400,500,600,700,800] if a variables name is followed by a 
                                          ',' then it is created as a list.
                                          lists can be useful as they only store one 
                                          variable name for multiple values in variable table.
                                          we will dynamically asign any data type
                                          to any value of the list.
                                          in memory, here is how a list is stored in the
                                          variable table:
  
  (TYPE,VARIABLE_NAME,ELEMENTS)           the list header
   LST   mylist        8...
   
  (0ELEMENT_TYPE,0ELEMENT_VALUE)          each element only uses two spots in memory
   STR            hello world!             here we store type 2 (STR) with value of
                                          "hello world!"
                                          
                                          if the list has already been created, then accessing
                                          an element is done like this:
  [out mylist,0]                          prints first element of "mylist"
  
  [mylist,0 "Hello GORK!"]                sets first element of "mylist" to value 
                                          "Hello GORK!"
                                          
  [mylist,7 3]                            assigns value of (3) to element 7 in list
  
  [mylist,7+23]                           adds 23 to element 7 in list.
  
  [mylist,20 42]                          Here we try to assign "42" to element 20 of "mylist"
                                          if "mylist" only has 8 elements. Should we grow the
                                          list by 12 elements? Or just return an INDEX OUT OF
                                          RANGE error???
                                          One option is to change the format of how we define
                                          how many elements we have in the list. By creating an
                                          unordered list...
                                          
  (TYPE, VARIABLE_NAME, ELEMENTS)         new format for the list header.
   LST   mylist         8...  
                                          when a list is initialized, eg:
  [mylist,24,"hi",123.432,...]            it gives an ID, starting at 0 to each element.
   
  (E_TYPE, E_ID, E_VALUE)                 each element uses 3 spots in memory.
   UINT    20    42                       here we store type UINT with value of
                                          42, and ID of 20.
  
  [findname]#                             function to find string in list
              [con mylist,0p=1p]          checks if mylist[0p value]=1p(STR)
              [ret 0p]                    if so, return 0p value, which is index num
              [0p+1]                      else, add 1 to op
              [con 0p<mylist]             if 0p is less than length of 'mylist'
              [lpf]                       loop function
            #
  [out findname,0,"hello"]                here we call findname sending 0p=0,1p="hello"
                                          out will print the index of "hello" if it exists.
                                          when comparing variables, if their types do not match,
                                          they are skipped. which makes searching much faster. eg.
  [list,"hello",2,3.321,"x",-234]
  
  [x findname,0,-234]                     elements are STR,UINT,DOUBLE,CHAR,LONG.
                                         -234 is LONG, so it skips first 4 elements immediately.
  [x321] [y83.13]                        x=LONG, y=DOUBLE
  [con x=y out x]                        immediately con will return false upon
                                         comparing their types. more examples of immediate failures:
  [x"a"] [y"hello"]                      comparing CHAR to STR
  [x24]  [y"a"]                          comparing UINT to CHAR
  
  [x"a"] [z"ask"]
  [con x=z,0]                            this will return TRUE, as strings can be
                                         treated as a list, and indexed like (strvar,x), to access
                                         their elements. when an elment is accessed individually,
                                         it returns a CHAR value.
  [x3421.234] [y32432.322]
  [con x=y]
                                        these expressions will be tested, but obvioulsy found
                                        to be false, but because the types are the same, 
                                        they will be tested.
                                        
  [x32][y33.1][con x<y]                 even though their types are different, they
                                        will be tested as they are both numbers. When number types
                                        are different, they will automatically return false or true
                                        when using "=" or "!" comparisons. 
                                        but when using "<" or ">" comparisons, they will be tested.
```
## Data Types

Data types are assigned automatically by the GORK interpreter. There are three basic types.

1. NUMBER TYPE
  All numbers are converted to a DOUBLE initially on when parsed. Then the interpreter will determine the size and type of the number and store it in the stack and variable table as such. Why not just store all numbers as doubles? GORK focuses on memory optimization over processing speed.
  
  Number types:
  - uint8_t
  - int8_t
  - int16_t
  - int32_t
  - double
  
  All floating point numbers are stored as doubles.
  
2. CHAR TYPE
  If a variable happens to be a string of 1 character eg. "a", it is saved as a CHAR type, which saves one byte of memory, excluding the trailing '\0' which the string has.
  
3. STRING TYPE
  A string is any number of numbers, characters, or symbols enclosed in double quotes "".
  
  
## Inserting data into var table

If the variable "x" is a string with value of "Hello", it will look like this in memory (2x'\0'hello'\0'). Let's say we want to change x to "Hello World!", we do:
```
x"Hello World!"
```
When GORK looks in the var table for variable "x", it finds that we have a string stored there with the name "x", but the only problem is it's length is 6 bytes long (including NULL terminator), whereas the new string we want to save there is 12 bytes long.

What do?

1. We need to subtract the length of "x" that is stored in var table (6) from the new length of "x" (12).
2. That means we need to shift the entire var table to the right 6 times, starting at the next variable. Using memmove() will work for this. Code for C function that will do this:

```c

void shift_var_table(int pos, int old_size, int shift_size){
  int shift_point=pos+old_size;
  int shift_to_point=pos+shift_size;
  int data_size=var_end-shift_point;
  
  //void * memmove ( void * destination, const void * source, size_t num );
  memmove(mem + shift_to_point, mem + shift_point, data_size); 
}

void update_var_in_table(int pos){
  /*
  pos is the var location we are updating. we have already
  gotten it by calling get_var();
  */
  //we get the size of var previously stored there
  int old_size=get_var_size(pos);

  //we get the size of var on stack we are pushing to table
  //(stack_pos) is a global variable
  int new_size=get_stack_var_size(stack_pos);
  //prototype integer for size to grow
  
  if(new_size!=old_size){
    /* examples 1: old_size: 3, new_size: 6
                   (6-3 = 3) grow table by 3
       example 2:  old_size: 5, new_size 3
                   (3-5= -2) shrink table by 2
    */          
    int shift_size = new_size-old_size;
    shift_var_table(pos, , old_size, shift_size);
  }
  

}
```

## Valid Whitespace Seperators:

Depending on what state GORK is in, different symbols can be used as whitespace seperators:

1. ### Regular Interpretation
In this state, GORK is not in a STRING, CONDITIONAL, or MATH expression.
The following can be used as whitespace seperators:
```
'~', '@', '$', '(', ')', '_', '{', '}',
'[', ']', ':', ';', '|', ' '
```

In almost any state, GORK can and will also use changes from data types as a seperator. All variable and function names MUST be alpha only. They cannot contain a number. This allows GORK to know if it's not in a string, and it's reading a letter (aA-zZ) then it is reading a variable or function name! 

Below are some examples:
```
a 24           [a=24] Here we use a space for a seperator...
a24            [a=24] Because in the change of character type,
                      GORK knows that 'a' is the variable name,
                      and 24 is the value.
a(24)
a:24
a@24
a|24           All mean the same thing. It is up to the programmer
               what style they want to use. Let's look at some more
               switches between data types.

a24b"Hello"c24.23out a+b+c
               The above code may look a little confusing, but
               This is what it actually contains:
a 24
b "Hello"      The double quotes (") starting and ending a string
c 24.23        are a change in data type, so they can be used as a
out a+b+c      whitespace seperator as long as they are encasing a
               string.
               
               The result here would be (24Hello24.23);

outa+b+c       An example that would not work as expected:
               [outa=b+c] creates a new variable and sets value
               to value of b+c...
                         
a: 24;
b: "Hello";
c: 24.23;
out(a+b+c)   Although the "Whitespace" symbols are unessesary in
             a lot of spaces, a programmer using GORK can take the
             liberty to use them and in doing so sacrifice a small
             amount of program memory in exchange for better read
             ability and be less likely to run into an error from
             mashing types.
```
2. ### MATH Interpretation
Below is an example of some MATH expressions:
The only acceptable whitespace in a math expression is a space ' ',
brackets of any kind "(){}[]" can be used in math expressions, and in a MATH expression will be counted as expression seperators, not whitespace!
```
a+b       [a=a+b]
a + b     [a=a+b] All whitespace is stripped from expression
                  when GORK is in a MATH expression..
                  
a(b)      [a=b] Expresssion has no math symbols,
                so brackets are treated as whitespace...
a b       [a=b]
a:b       [a=b]
a b+24    [a=b+24]
a=b       [ERROR, not in CONDITIONAL]
a24/(2+1) [a=24/2+1] Brackets are not used as multipliers
                       as with some math formats...
```
Symbols that are only used for MATH expressions:
(note the '+' symbol can also be used to concationate strings.
```
add:      '+'
  var+1                   [var=var+1]
  a"Hello"                [a="Hello"]
  a+" World!"             [a=a+" World!"] a="Hello World!"
  
  z 21 + " Jump Street"   [z="21 Jump Street"] Whenever a
                          number is added to a string, it always
                          results in a string...
subtract: '-'
  x-1        [x=x-1] With negative numbers, the spacing is
                     important! If there is no space between
                     the variable name and the number, it will
                     be treated as subtraction. Otherwise, a space
                     or other valid delimiter such as a bracket is
                     needed to assign a variable a negative number.
  x(-1)
  x:-1
  x[-1]
  x{-1}
  x|-1
  x -1       [x= -1] All of these are valid seperators to
                     let GORK know that it is a negative number
                     and we are not trying to subtract from "x".
  x52-27     [x=52-27]
  
multipy:  '*'
  w30                 [w=30]
  l45                 [l=45]
  sqft w*l            [sqft=30*45]
  turtles(3*4)+(2*3)  [turtles=(12)+(6)]
divide:   '/'
modulus:  '%'
  zed32
  out zed%30          [prints 2]
exponent: '^'
  num3
  num^num             [num=3*3*3]
```

Symbols that are only used in CONDITIONAL expressions:
```
greater: '>'
  con x>y out x        [if x>y print x]
lesser:  '<'
  con x<y x+1          [if x<y add 1 to x]
equals:  '='
  con x=y out"EQUAL"   [if x=y print EQUAL]
not:     '!'
  con x!y out "NOT"    [if x is not equal to y print NOT]
and:     '&'
  con x>y & y>z out1   [if x>y AND y>z print 1]
or:      '?'
  con x>0 ? x=-1       [if x>0 OR x= -1] Note we did not used
                                         any whitespace before
                                         -1, that is because there
                                         are no math operations
                                         happening in a conditional.
                                         so anything with a '-' is
                                         considered negative.
```

## Expressions

Determining what makes an expression start and stop is difficult. We need to make some rules for expressions.

1. ### Expression Beginning
  What determines the beginning of an expression?
  
  At this point I am thinking an expression always has to start with a VARIABLE or FUNCTION name.
  As we will always be handling a variable or function.
  
  ```
  [myvar]
  [myfunc]
  [mylist]
  
  [out]
  [con]
  [lpf]
  [ret]
  ```
  
2. ### Expression Middle
  How do we know we are still in the middle of an expression? What kind of expression is it?
  
  Once the variable or function name that is the "owner" of the expression has been parsed. Everything following the first delimiter is the "middle" of the expression.
  Examples of SETTING expressions, where we are just giving values to a VARIABLE or FUNCTION.
  
  ```
  [myvar 123]
  [myfunc 1,2]  A function call with two parameters
  [mylist,"This is a list",123,1.342,"x"]
  
  [out 123,"Hello"] A function call to OUT
  [con myvar>100]   A conditional check on myvar
  [lpf]             A function call to LPF, outside a function, this does nothing.
                    inside a function this will jump to beginning of function.
  [ret myvar]             Outside a function RET also does nothing. If called
                    Inside a function it ends the function, passing the
                    data it is returning to the variable that is calling the
                    function.
  ```
  
3. ### Expression End
  How do we know we are at the end of an expression? How do we parse the expression once we have determined the beginning, middle, and end?
  
  In order to know what ends the expression, we need to know what is determining it is IN the expression.
  
  Assigning a variable: `myvar2`, GORK knows the name and type of the variable. If there is a space after the 2, then GORK obiously knows thats it. But what if we are adding multiple variables together?
  
  ```
  first 12
  second 15
  third 16
  fourth first+second+third
  ```
  So based off of this, we need to be looking for MATH symbols! After the first whitespace seperating the variable name from the variable's value we are assigning to it, we will check if there are any MATH symbols that are adding more to it.
  
  What if we are incrimenting a number like this:
  ```
  fourth+1
  ```
  Then GORK should know that the value is fourth's own value+1. If fourth didn't have a value before, it would assign it the value of 1. Any uninitialized variables should always be started at 0 if a number and "" if a string or CHAR.
  
  So when adding to a variable, we check if there are any math operations happening, once we hit some "whitespace", we know that the expression is over.
  
## More Expression Examples

Considering that with different data types and functions GORK will be looking for different indicators for starting and ending a function, lets go over the basics.

### Regular Variables

1. Start with variable name `myvar`
2. Immediately after variable name have "whitespace" seperating the first value. 
The '-' symbol is not considered acceptable whitespace, therefore it must have an aditional whitespace after the VAR name in order to set the var to a negative number.
Acceptable whitespace:
  ```
  myvar2
  myvar 2
  myvar"hello"
  myvar yourvar
  myvar("HELLO")
  myvar:231.324
  myvar: 123
  myvar -3
  myvar(-3)
  ```
3. If more is going to be added to the variable, MATH symbols will be used. The only Math symbol that can be used on a NON NUMBER data type such as a STRING or CHAR is '+'
  ```
  myvar2+2
  myvar 2-3
  myvar"hello "+2+" all!"
  myvar yourvar+myvar
  myvar("hello"+" world!")
  myvar:231.324*1
  myvar: 123/3*(4+yourvar)
  myvar -3*-1
  myvar(-3+3)
  ```
  So after reaching the end of a value and there is no MATH symbol following, we know the expression has ended and GORK needs to put it all together.
  
### Assigning Variables from Variables

Let's say we want to assign the value of `bar` to the variable `foo`:
```
foo bar
```
That's it! Gork looks up the value of `bar` and put's it in `foo`'s value slot in the var table.

### Expressions Regarding Functions
Functions are defined like this: `func # <function data> #`
When Gork reads a function name immediately followed by a '#' symbol, it knows that's a function and stores it's name and address in variable table.

Functions are the only place in GORK that will have variables with a number in them.

If parameters are sent to a function, they are accessed within a function in the order they are sent: 0p, 1p, 2p, 3p...

If no parameters were sent, and they are tried to access within the function, they return NULL values.

All the data contained within the `# #` function block is skipped upon interpreting the PROGRAM code, until the function is called. If for some reason the function is never called, it is never interpreted.

When the function is called, it is interpreted, any variables initialized within the function are local and cleared from the STACK when function is finished running. All parameters are also temorary.

Lets look at a simple function example:
  
```
hello #
  ret "Hello World"
#

out hello
```

Above we made a "Hello World" function.
when `hello` is called, all it does is return the string "Hello World!". Our BUILTIN OUT function prints this to the console.

Another simple verison of "Hello World":

```
hello#out"Hello World"#

hello
```
In this version, we call the OUT BUILTIN from within the function, so all we have to do is call `hello` to run the function.
Therefore a functions name can be an expression by itself.
GORK knows that nothing HAS to follow the function call for the expression to be over, but it will look anyways. It will look for a NUMBER or STRING, or VARIABLE name then a COMMA, incase there are multiple parameters.

We will discuss this in the next section.

### Assigning Variables from Functions
A Variable name preceeding a function call will be assigned the value the function returns, if it returns any data. If it doesn't return any data, nothing happens too the variable.

Below is an example of a function that takes two parameters (in this case numbers) the first number is the start of the counting, the second is the end of the counting, the function returns the end number to the calling VARIABLE `x`, to where it is stored.
```
myfunc#out0p0p+1con0p<1plpf ret0p#
x0x myfunc0,100
```
A little hard to read eh? Lets expand that and add some "whitespace" to make it easier to see the expressions:
```
myfunc# {
  out( 0p );
  0p +1;
  con( 0p < 1p ){ lpf; } 
  ret( 0p );
  } #
x: 0;

x: myfunc( x, 100 );
```

That may look a little more familiar to most people to programming languages. That is the beauty of GORK in the flexibility we can write with. As an interpeted language, the ability to strip whitespace to shorten the code could be a great asset for memory storage and interpreting time. But we also have the ability to write with as much white space as we want.

On the last line of that code example: `x: myfunc( x, 100 );` that entire line is an expression. We are starting with the variable name 'x', and telling GORK we want to make it's value whatever 'myfunc' returns. `myfunc` and `x,100` are also expressions in a way, but they are still part of the main expression of assigning value to `x`.
  

  