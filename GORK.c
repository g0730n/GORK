#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MEM_SIZE    512
#define VAR_START   256
#define STACK_START 512


#define CHAR_SIZE   sizeof(unsigned char)
#define INT_SIZE    sizeof(int8_t)
#define UINT_SIZE   sizeof(uint8_t)
#define LONG_SIZE   sizeof(int16_t)
#define LONGER_SIZE sizeof(int32_t)
#define DOUBLE_SIZE sizeof(double)

#define DOUBLE_MAX 100000000000
#define DOUBLE_MIN -100000000000

#define MAX_INPUT_SIZE  32
#define MAX_STRING_SIZE 16
#define MAX_WORD_SIZE 8
#define MAX_BUILTINS 5

const char * program[] = "var4 out var var+4 out "
												 "\"hello there!\""
												 "func#x0 out x x+1 con x<10 lpf ret0#"
												 "func out var";
                         
/* []shows expressions, ()shows function code that is iterpreted when function called
[var4] [out var] [var+4] [out"hello there!"]
[func]#([x0] [out x] [x+1][con x<10][lpf] [ret0])#
[var func] [out var]

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
  
  
*/

char *words[MAX_BUILTINS] = { "out", "lpf", "ret", "con", "NULL" };
char cmd[MAX_INPUT_SIZE];
char *mem;
int mem_pos;
int mem_end;
int stack_end = STACK_START-CHAR_SIZE;
int stack_pos = STACK_START-CHAR_SIZE;
int var_end =   VAR_START;
int error;
int pos_count;

int check_builtins(char* word){
  int i;
  for(i=0; i<MAX_BUILTINS; i++){
    if(word[0]==words[i][0]){
      if(!strcmp(word,words[i])){
        printf("[[%s]]\n",words[i]);
        return i;
      }
    }
  }
  return i;
}

enum prog_type{
	COMMAND,
	PROGRAM
};

enum types {
		NUN,
    CHAR,
		STR,
		UINT,
    INT,
		LONG,
		LONGER,
    DOUBLE,
		NUM,
		FUNC
};

enum errors {
	NONE,
	UNKNOWN,
	OUT_OF_MEMORY,
	NUMBER_TOO_BIG,
	NUMBER_TOO_SMALL,
	STRING_TOO_LONG,
	STACK_OVERFLOW,
	STACK_UNDERFLOW,
  ADDRESS_OUT_OF_RANGE,
  MEMORY_NOT_ALLOCATED
};

enum states {
	BETWEEN,
	INWORD,
	INSTRING,
	INVAR,
	INFUNC,
	INNUM
};

int check_errors(int e){
	if(e>NONE){
		printf("\nERROR: ");
		if(e==OUT_OF_MEMORY){ printf("OUT OF MEMORY!"); }
		else if(e==NUMBER_TOO_BIG){ printf("NUMBER TOO BIG!"); }
		else if(e==NUMBER_TOO_SMALL){ printf("NUMBER TOO SMALL!"); }
		else if(e==STRING_TOO_LONG){ printf("STRING TOO LONG!"); }
		else if(e==STACK_OVERFLOW){ printf("STACK OVERFLOW!"); }
		else if(e=STACK_UNDERFLOW){printf("STACK UNDERFLOW!"); }
    else if(e=ADDRESS_OUT_OF_RANGE){printf("ADDRESS OUT OF RANGE!"); }
    else if (e=MEMORY_NOT_ALLOCATED){printf("MEMORY NOT ALLOCATED!"); }
		else { printf("UNKNOWN ERROR!"); }
		free(mem);
		exit(0);
	}
}

int get_type_size(int type){
	int size;
	if(type==CHAR){ size=CHAR_SIZE; }
	else if(type==UINT){ size=UINT_SIZE; }
	else if(type==INT){ size=INT_SIZE; }
	else if(type==LONG || type==FUNC){ size=LONG_SIZE; }
	else if(type==LONGER){ size=LONGER_SIZE; }
	else if(type==DOUBLE){ size=DOUBLE_SIZE; }
	else if(type==STR){ size=0; }
	else{ error=UNKNOWN; }
	check_errors(error);
	return size;
}

int move_pos(int pos, int amount){
	pos+=amount;
	if(pos>=stack_end){ check_errors(OUT_OF_MEMORY); }
	return pos;
}

int grow_stack(int pos, int amount){
  
}

int shrink_stack(int pos, int amount){
  
}

int grow_stack_pointer(int pos, int amount){
	pos-=amount;
	if(pos<=var_end){ check_errors(STACK_OVERFLOW); }
	return pos;
}

int shrink_stack_pointer(int pos, int amount){
	pos+=amount;
	if(pos>MEM_SIZE){ check_errors(STACK_UNDERFLOW); }
	return pos;
}

int dump_mem(int pos) {
  int var_size = 0;
  int var_type = mem[pos];
  pos = move_pos(pos, CHAR_SIZE);
  printf("\nVAR_NAME: %s\n", mem + pos);
	pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
	
  if (var_type == CHAR) {
    var_size=CHAR_SIZE;
		printf("TYPE: CHAR\nDATA: %c\n", mem[pos]);
	}else if (var_type == INT) {
    var_size=INT_SIZE;
		printf("TYPE: INT\nDATA: %d\n", (int8_t)mem[pos]);
	}else if (var_type == UINT) {
    var_size=UINT_SIZE;
		printf("TYPE: UINT\nDATA: %d\n", (uint8_t)mem[pos]);
	}else if (var_type == LONG || var_type == FUNC) {
    var_size=LONG_SIZE;
		int16_t *lptr = (int16_t *)(mem + pos);
    printf("TYPE: ");
    if(var_type==LONG){ printf("LONG"); }
    else{ printf("FUNC"); }
		printf("\nDATA: %d\n", *lptr);
	}else if (var_type == LONGER) {
    var_size=LONGER_SIZE;
		int32_t *lgrptr = (int32_t *)(mem + pos);
		printf("TYPE: LONGER\nDATA: %d\n", *lgrptr);
	}else if (var_type == DOUBLE) {
    var_size=DOUBLE_SIZE;
		double *dptr = (double *)(mem + pos);
		printf("TYPE: DOUBLE\nDATA: %f\n", *dptr);
	}else if (var_type == STR) {
		printf("TYPE: STRING\nDATA: %s\n", mem + pos + CHAR_SIZE);
    var_size = strlen(mem + pos + CHAR_SIZE);
	}else {
    check_errors(UNKNOWN);
	}
  pos = move_pos(pos, var_size);
  printf("\n----------------\n");
  return var_size;
}

void break_line(int pos){
	if(pos-pos_count>32){
		putchar('\n');
		pos_count = pos;
	}
}

int print_string(int pos){
	int end=strlen(mem + pos)+CHAR_SIZE;
	for(int i=0; i<end;i++){
		if(mem[pos]=='\0'){ putchar(mem[pos]+48); }
		else{ putchar(mem[pos]); }
		pos+=CHAR_SIZE;
		break_line(pos);
	}
	return pos;
}

void dump_all_mem(int pos, int length) {
	pos_count=pos;
	while(pos<length){
		if(mem[pos]>=0 && mem[pos]<=9){
			putchar((char)mem[pos]+48);
		}else{
			putchar(mem[pos]);
		}
		
		if(pos>=VAR_START){
			if (mem[pos] == CHAR) {
					pos+=CHAR_SIZE;
					break_line(pos);
					pos=print_string(pos);
					printf("%c", mem[pos]);
					pos+=CHAR_SIZE;
					break_line(pos);
			} else if (mem[pos] == INT) {
					pos+=CHAR_SIZE;
					break_line(pos);
					pos=print_string(pos);
					printf("%d", (int8_t)mem[pos]);
					pos+=INT_SIZE;
					break_line(pos);
			} else if (mem[pos] == UINT) {
				 pos+=CHAR_SIZE;
				 break_line(pos);
				 pos=print_string(pos);
				 printf("%d", (uint8_t)mem[pos]);
				 pos+=UINT_SIZE;
				 break_line(pos);
			} else if (mem[pos] == LONG || mem[pos]== FUNC) {
					pos+=CHAR_SIZE;
					break_line(pos);
					pos=print_string(pos);
					for(int i=0; i<LONG_SIZE; i++){
						putchar('l');
						pos+=CHAR_SIZE;
						break_line(pos);
					}
			} else if (mem[pos] == LONGER) {
					pos+=CHAR_SIZE;
					break_line(pos);
					pos=print_string(pos);
					for(int i=0; i<LONGER_SIZE; i++){
						putchar('L');
						pos+=CHAR_SIZE;
						break_line(pos);
					}
			} else if (mem[pos] == DOUBLE) {
					pos+=CHAR_SIZE;
					break_line(pos);
					pos=print_string(pos);
					for(int i=0; i<DOUBLE_SIZE; i++){
						putchar('d');
						pos+=CHAR_SIZE;
						break_line(pos);
					}
			} else if (mem[pos] == STR) {
					pos+=INT_SIZE;
					break_line(pos);
					pos=print_string(pos);
					pos=print_string(pos);
			}else {
					pos+=CHAR_SIZE;
			}
			break_line(pos);
		}else{
			pos+=CHAR_SIZE;
			break_line(pos);
		}

	}
	putchar('\n');
}

void add_type_from_stack(){
	mem[var_end]=mem[stack_end];
	var_end=move_pos(var_end,INT_SIZE);
	stack_pos=shrink_stack_pointer(stack_end,INT_SIZE);
}

void add_var_name_from_stack(){
	int name_size = strlen(mem + stack_pos)+CHAR_SIZE;
	memcpy(mem+var_end, mem+stack_pos, name_size+CHAR_SIZE);
	var_end = move_pos(var_end, name_size);
	stack_pos = shrink_stack_pointer(stack_pos, name_size);
}

void add_var_from_stack(){
	int var_type = mem[stack_end];
	int var_size = get_type_size(var_type);
	mem[var_end]=var_type;

	if(var_type==STR){
		var_size=strlen(mem+stack_pos)+CHAR_SIZE;
		memcpy(mem + var_end, mem + stack_pos, var_size);
		var_end = move_pos(var_end, var_size);
		stack_pos = shrink_stack_pointer(stack_pos, var_size-1);
	}
	else{
		memcpy(mem + var_end, mem + stack_pos, var_size);
		var_end = move_pos(var_end, var_size);
		stack_pos = shrink_stack_pointer(stack_pos, var_size);
	}
}

int push_num_to_stack(double data){
	int type=DOUBLE;
	if(data>DOUBLE_MAX){
		printf("%f\n",data);
		error=NUMBER_TOO_BIG;
	}else if(data<DOUBLE_MIN){
		printf("%f\n",data);
		error=NUMBER_TOO_SMALL;
	}else{
		if (data == (int32_t)data) {
			int32_t num = (int32_t)data;
			if(num>=0 && num<=UINT8_MAX){
				uint8_t num = (uint8_t)data;
				type = UINT;
				stack_end = grow_stack_pointer(stack_end,UINT_SIZE);
				memcpy((uint8_t *)(mem + stack_end), &num, UINT_SIZE);
			}
			else if(num>=INT8_MIN && num<= INT8_MAX){
				int8_t num = (int8_t)data;
				type = INT;
				stack_end = grow_stack_pointer(stack_end, INT_SIZE);
				memcpy((int8_t *)(mem + stack_end), &num, INT_SIZE);
			}
			else if(num>=INT16_MIN && num<=INT16_MAX){
				uint16_t num = (uint16_t)data;
				type = LONG;
				stack_end = grow_stack_pointer(stack_end,LONG_SIZE);
				memcpy((uint16_t *)(mem + stack_end), &num, LONG_SIZE);
			}
			else{
				type = LONGER;
				stack_end = grow_stack_pointer(stack_end,LONGER_SIZE);
				memcpy((uint32_t *)(mem + stack_end), &num, LONGER_SIZE);
			}
		}else{
				double num = data;
				type = DOUBLE;
				stack_end = grow_stack_pointer(stack_end,DOUBLE_SIZE);
				memcpy((double *)(mem + stack_end), &num, DOUBLE_SIZE);
		}
	}
	check_errors(error);
	return type;
}

void push_func_to_stack(uint16_t addr){
  if(addr<0 || addr>MEM_SIZE){
  	printf("%f\n",addr);
    error=ADDRESS_OUT_OF_RANGE;
  }else{
    stack_end = grow_stack_pointer(stack_end,LONG_SIZE);
    memcpy((uint16_t *)(mem + stack_end), &addr, LONG_SIZE);
  }
  check_errors(error);
}


void push_str_to_stack(char* str){
	int length=strlen(str);
	if(length>MAX_STRING_SIZE){
		printf("%s\n",str);
		error=STRING_TOO_LONG;
	}else{
		stack_end = grow_stack_pointer(stack_end, length+CHAR_SIZE);
		strcpy(mem + stack_end, str);
	}
	check_errors(error);
}

void push_var_name_to_stack(char* name){
	int length=strlen(name)+CHAR_SIZE;
	if(length>MAX_WORD_SIZE){
		printf("%s\n",name);
		error=STRING_TOO_LONG;
	}else{
		stack_end = grow_stack_pointer(stack_end, length);
		memcpy(mem + stack_end, name, length);
	}
	check_errors(error);
}

void push_char_to_stack(char data){
	stack_end = grow_stack_pointer(stack_end, CHAR_SIZE);
	mem[stack_end]=data;
	check_errors(error);
}

void push_type_to_stack(uint8_t type){
	stack_end = grow_stack_pointer(stack_end, INT_SIZE);
	mem[stack_end]=(uint8_t)type;
	check_errors(error);
}

int get_var(char *var_name){
	//find variable from memory's variable table
	int pos=VAR_START;
	uint8_t type;
	uint8_t size;
	/*
		loop through all variables until finding a match.
		if no match is found, return the end of the var table address,
		otherwise return the address of the variable with matching name.
	*/
	while(pos<var_end){
		type=mem[pos];
		pos=move_pos(pos, UINT_SIZE);
		size = get_type_size(type);
		if(mem[pos]==var_name[0]){ //if first letter of variable name matches
			if(!strcmp(mem+pos, var_name)){ //if variable name matches
				dump_mem(pos-UINT_SIZE);
				return pos;
			}
			else{ //if it's a string and theres no match
				if(type==STR){
					pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
					pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
				}
				else{ //if its not a string and there's no match
					pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE + size);
				}
			}
		}
		else{
			if(type==STR){ //if it's a string and theres no match
				pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
				pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
			}
			else{ //if its not a string and there's no match
				pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
				pos = move_pos(pos, get_type_size(type));
			}
		}
	}
	if(VAR_START==var_end){
		pos=0;
	}
	return 0;
}


void empty_stack(){
	for(int i=stack_end; i<STACK_START; i++) mem[i]='\0';
	stack_pos=stack_end=STACK_START;
}

int get_var_from_stack(int addr){
  int var_type=mem[stack_end];
  int var_size=get_type_size(var_type);
  if(var_type==STR){
    int add = strlen(mem + stack_end + INT_SIZE);
    var_size = strlen(mem + stack_end + INT_SIZE)+add;
  }else{
    var_size=get_type_size(var_type);
  }
  
  return var_size;
}

void push_var_to_table(char *word){
  int addr=get_var(word);
  if(!addr){
    int var_length=get_var_from_stack(addr);
    add_type_from_stack();
    add_var_name_from_stack();
    add_var_from_stack();
    addr=get_var(word);
  }else{
    
  }
  
}

int add_var_to_stack(char* word, int type, char* value, int addr){
	if(type==NUM){ type=push_num_to_stack(atof(value)); }
	else if(type==CHAR){ push_char_to_stack(value[0]); }
	else if(type==STR){ push_str_to_stack(value); }
  else if(type==FUNC){ push_func_to_stack(addr+1); }
	else {
		printf("ERROR: INVALID TYPE\n");
		return 1;
	}
	push_var_name_to_stack(word);
	push_type_to_stack(type);
  
  int b=check_builtins(word);
  if(b==MAX_BUILTINS){
    push_var_to_table(word);
  }
	//stack_pos=stack_end;
	//printf("STACK_END:%d",stack_end);
	return 0;
}

int interpret(int input, int start, int stop){
	int state=BETWEEN;
	int sub_state=BETWEEN;
	int word_pos;
	int sub_word_pos;
	int pos;
	char *ptr;
	char word[MAX_WORD_SIZE];
	char data[MAX_INPUT_SIZE];
	int word_type=NUN;
  
	if(input==PROGRAM){ ptr=mem; }
  else{ ptr=cmd; }
	
	for(int i=start; i<stop+1; i++){
    if(sub_state!=INFUNC){
      
      if(isalpha(ptr[i])){
        if(state!=INSTRING){
          if(state!=INWORD){
            word_pos=0;
            state=INWORD;
            word[word_pos]=ptr[i];
          }else{
            word_pos++;
            word[word_pos]=ptr[i];
          }
        }else{ ;}
      }else{
        if(state==INWORD){
          word_pos++;
          word[word_pos]='\0';
          state=BETWEEN;
          printf("\n%s:",word);
          if(ptr[i]=='#'){ add_var_to_stack(word, FUNC, "", i); }
        }
      }
      
      if(isdigit(ptr[i])){
        if(word_type!=NUM){
          sub_word_pos=0;
          data[sub_word_pos]=ptr[i];
          word_type=NUM;
          sub_state=INNUM;
        }else{
          sub_word_pos++;
          data[sub_word_pos]=ptr[i];
        }
      }else{
        if(sub_state==INNUM){
          sub_word_pos++;
          data[sub_word_pos]='\0';
          add_var_to_stack(word, NUM, data, i);
          printf("%s",data);
          word_type=NUN;
          sub_state=BETWEEN;
        }
      }
      
      if(ptr[i]=='\n'){putchar(ptr[i]); }
      if(ptr[i]==' '){ putchar(ptr[i]); }
      
      if(ptr[i]=='"'){
        if(state!=INSTRING){
          sub_word_pos=0;
          i++;
          data[sub_word_pos]=ptr[i];
          word_type=STR;
          state=INSTRING;
        }
        else{
          sub_word_pos++;
          data[sub_word_pos]='\0';
          add_var_to_stack(word, STR, data, i);
          word_type=NUN;
          state=BETWEEN;
        }
      }else{
        if(state==INSTRING){
          sub_word_pos++;
          data[sub_word_pos]=ptr[i];
        }
      }
    }
    
		if(ptr[i]=='#'){
      if(sub_state!=INFUNC){
        word_type=NUN;
        sub_state=INFUNC;
        printf("<FS>");
      }else{
        sub_state=BETWEEN;
        printf("<FE>");
      }
			sub_state==INFUNC;
		}
	}
}

int main() {
  mem = (char *)calloc(MEM_SIZE,CHAR_SIZE);
  memcpy(mem, *program, strlen(*program));

  if(mem == NULL) {
    check_errors(MEMORY_NOT_ALLOCATED);
  }else {
    
    printf("\n<Memory allocated>\n");
    printf("\n-----------DEBUG---------\n");
    interpret(PROGRAM,0,VAR_START);
      
		//add_var_to_stack("a", CHAR, "z", 0);
     
    printf("\n-------------------------\n");
    //int var_addr=get_var("func");
    //printf("ADDR:%d\n",var_addr);
    printf("\n-------END DEBUG---------\n\n");
        
    dump_all_mem(0,MEM_SIZE);
  }
  
  free(mem);
  printf("\n<Memory Freed>\n");
  return 0;
}
