#GORK Programming Language
```
[var4] [out var] [var+4] [out"hello there!"]
[func]#([out 0p] [0p+1][con 0p<10][lpf] [ret0p])#
[var func,var] [out var]
```
RULES FOR IDENTIFYING AN EXPRESSION:
1. var name has been followed by a value, then whitepsace(space or any other non used symbols, tabs, newlines, etc. operators (+ with str or nums), -, /, *, % continue the expression
  [var 4]                  var=4
  [firstvar secondvar]     firstvar=secondvar
  [var+2]                  var=value of var + 2
  [var 2+2]                var=2+2
  [var"hello "+"world!"]   var="hello "+"world!" (+ operator only can be used on str)
  [var2+"hi"]              var="2hi" (a string in expression always results in str)
  [var 32+var]             same as [var+32]

2. RULES FOR BUILTINS, AND FUNCTIONS
  [out2]                  prints "2"
  [outvar] [x+2]          creates var "outvar=x+2"
  [out var]               prints var
  [out"hello "+"world!"]  prints "hello world!"
  [out2+var/3]            prints var/3+2
  [func #]or
  [func#]                 defines a function. stores func name and addr in variable
                          table. Function code block is skipped until function is called.
  [func]                  a func call is always an expression to itself
  [func2,3,2]             unless parameters are used????
  [func"hello "]          str parameter???
  [x func]                storing function return value in x
                          functions can never be redifined. Doing [func3] will never
                          create a new variable of "func" with value of 3, but consider
                          3 to be a parameter. if no parameters are used in function,these values are simply discarded.
  [x func0]               sends "0" parameter to func, stores result in x
  [ret x]
  [ret"DONE!"]
  [ret24]                 "ret" must always be followed by a value or variable
  [lpf]                   "lpf" if called, starts function over again from beginning
  [con3<10]               "con must always be followed by (value)(operator)(value)
  [con x<100]             if x is less than 100
  [con x="hello"]         if x equals "hello"
  [con100>x]              if 100 is greater than x (or if x is less than 100)
  [con100!x]              if 100 does not equal x
  [con100=x]              if 100 equals x
  [con1=1]                creates an infininte loop but with no way to break out of
                          instead do:
  [con x=1] or
  [con1=x]                then x can be changed to other than 1 and loop breaks
                          "con" must always be followed by (value)(operator)(value), if
                          it returns true(uint8_t '1'), then the next expression is evaluated. if it returns false(uint8_t '0'), then the next expression is skipped.
                          a function can be used as the next expression, eg.
  [x0]
  [con100>x][x myfunc x]  this is really considered ONE expression as the conditional
                          will skip the function expression if it doesn't evalueate to
                          true. if it evaluates to true, it executes the function. below is the code for the whole function.
  [myfunc]#
          [ret 0p+1]      parameters are indexed by their position (0p,1p,2p,3p...)
          #               in this case we sent the value of x:(0) as 0p, then add 1
                          to it and return it, which basically does "x=1". the condtion checks that 100>1, and it's true, so it runs [x myfunc0] again.
                          another way of doing this is simpler:
  [myfunc]#[0p+1]
           [out0p]
           [con0p<100]
           [lpf]
           [ret0p]
          #
  [x myfunc0]             this does the same thing basically, but prints out 1-100,
                          then at the end stores 100 in x. we can do the same thing,
                          by passing two parameters:
    [myfunc]#
           [0p+1]
           [out0p]
           [con0p<1p]
           [lpf]
          #
  [myfunc0,100]        here we pass, 0 and 100 as the parameters. and instead of
                       storing the value at the end, we simply print 1-100.
                       
3. RULES FOR LISTS
  [mylist100,200,300,400,500,600,700,800] if a variables first value is followed by a 
                                          ',' then it is created as a list. lists can be useful as they only store one variable name for multiple values in variable table. we will dynamically asign any data type to any value of the list. in memory, here is how a list is stored in the variable table:
  
  (TYPE,VARIABLE_NAME,ELEMENTS)           the list header
   24   mylist        8...  
  (0ELEMENT_TYPE,0ELEMENT_VALUE)          each element only uses two spots in memory
   2             hello world!             here we store type 2 (STR) with value of
                                          "hello world!"
                                          if the list has already been created, then accessing an element is done like this:
  [out mylist0]                           prints first element of "mylist"
  [mylist,0"hello gork!"]                  sets first element of "mylist" to value 
                                          "hello gork!"
  [mylist,7 3]                             assigns value of (3) to element 7 in list
  [mylist,7+23]                            adds 23 to element 7 in list.
  
  [findname]#                             function to find string in list
              [con mylist,0p=1p]          checks if mylist[0p value]=1p(STR)
              [ret 0p]                    if so, return 0p value, which is index num
              [0p+1]                      else, add 1 to op
              [con 0p<mylist]             if 0p is less than length of 'mylist'
              [lpf]                       loop function
            #
  [out findname 0,"hello"]                here we call findname sending 0p=0,1p="hello"
                                          out will print the index of "hello" if it exists.
                                          when comparing variables, if their types do not match, they are skipped. which makes searching much faster. eg.
  [list"hello",2,3.321,"x",-234]
  
  [x findname0,-234]                     elements are STR,UINT,DOUBLE,CHAR,LONG.
                                         -234 is LONG, so it skips first 4 elements immediately.
  [x321] [y83.13]                        x=LONG, y=DOUBLE
  [con x=y out x]                        immediately con will return false upon
                                         comparing their types. more examples of immediate failures:
  [x"a"] [y"hello"]                      comparing CHAR to STR
  [x24]  [y"a"]                          comparing UINT to CHAR
  
  [x"a"] [z"ask"]
  [con x=z,0]                            this will return TRUE, as strings can be
                                         treated as a list, and indexed like (strvar,x), to access their elements. when an elment is accessed individually, it returns a CHAR value.
  [x3421.234] [y32432.322]
  [con x=y]
                                        these expressions will be tested, but obvioulsy found to be false, but because the types are the same, they will be tested.
  [x32][y33.1][con x<y]                 even though their types are different, they
                                        will be tested as they are both numbers. When number types are different, they will automatically return false or true when using "=" or "!" comparisons. but when using "<" or ">" comparisons, they will be tested.
  
  