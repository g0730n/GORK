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
												 "func out var "
                         "x-123456";
                         
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
			} else if (mem[pos] == LONG || mem[pos] == FUNC) {
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
				int16_t num = (int16_t)data;
				type = LONG;
				stack_end = grow_stack_pointer(stack_end,LONG_SIZE);
				memcpy((int16_t *)(mem + stack_end), &num, LONG_SIZE);
			}
			else{
				type = LONGER;
				stack_end = grow_stack_pointer(stack_end,LONGER_SIZE);
				memcpy((int32_t *)(mem + stack_end), &num, LONGER_SIZE);
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

void push_func_to_stack(int16_t addr){
  if(addr<0 || addr>MEM_SIZE){
  	printf("%f\n",addr);
    error=ADDRESS_OUT_OF_RANGE;
  }else{
    stack_end = grow_stack_pointer(stack_end,LONG_SIZE);
    memcpy((int16_t *)(mem + stack_end), &addr, LONG_SIZE);
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
          if(ptr[i]=='-'){
            int p=i;
            p++;
            if(isdigit(ptr[p])){
              while(isdigit(ptr[p])){
                printf("<%c>",(char)ptr[p]);
                p++;
              }
              if(ptr[p]!='+' && ptr[p]!='*' && ptr[p]!='/' && ptr[p]!='%'){
                printf("<%d>",ptr[p]);
                sub_word_pos=0;
                data[sub_word_pos]=ptr[i];
                word_type=NUM;
                sub_state=INNUM;
                i++;
              }
            }
          }
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
      
      if(isspace(ptr[i])){putchar(ptr[i]); }
      //if(ptr[i]=='\n'){putchar(ptr[i]); }
      //if(ptr[i]==' '){ putchar(ptr[i]); }
      
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
    int var_addr=get_var("x");
    printf("ADDR:%d\n",var_addr);
    printf("\n-------END DEBUG---------\n\n");
        
    dump_all_mem(0,MEM_SIZE);
  }
  
  free(mem);
  printf("\n<Memory Freed>\n");
  return 0;
}
