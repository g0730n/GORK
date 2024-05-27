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

#define MAX_STRING_SIZE 24

const char * program[] = "var4 out var var+4 out"
												 "\"hello there!\""
												 "func#x0 out x x+1 con x<10 lpf ret0#"
												 "func out var";

char * words[] = { "out", "lpf", "ret", "con" };
char cmd[32];
char *mem;
int mem_pos;
int mem_end;
int stack_end = STACK_START-CHAR_SIZE;
int stack_pos = STACK_START-CHAR_SIZE;
int var_end =   VAR_START;
int error;
int pos_count;

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
  ADDRESS_OUT_OF_RANGE
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
	else if(type==LONG){ size=LONG_SIZE; }
	else if(type==LONGER){ size=LONGER_SIZE; }
	else if(type==DOUBLE){ size=DOUBLE_SIZE; }
	else if(type==STR){ size=0; }
  else if(type==FUNC){ size=LONG_SIZE; }
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
	pos-=amount;
	if(pos<=var_end){ check_errors(STACK_OVERFLOW); }
	return pos;
}

int shrink_stack(int pos, int amount){
	pos+=amount;
	if(pos>MEM_SIZE){ check_errors(STACK_UNDERFLOW); }
	return pos;
}

void dump_mem(int pos, int length) {
	if (mem[pos] == CHAR) {
			pos = move_pos(pos, CHAR_SIZE);
			printf("VAR_NAME: %s, ", mem + pos);
			pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
			printf("CHAR: %c\n", mem[pos]);
			pos = move_pos(pos, CHAR_SIZE);
	} else if (mem[pos] == INT) {
			pos = move_pos(pos, CHAR_SIZE);
			printf("VAR_NAME: %s, ", mem + pos);
			pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
			printf("INT: %d\n", (int8_t)mem[pos]);
			pos = move_pos(pos, INT_SIZE);
	} else if (mem[pos] == UINT) {
		 pos = move_pos(pos, CHAR_SIZE);
		 printf("VAR_NAME: %s, ", mem + pos);
		 pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
		 printf("UINT: %d\n", (uint8_t)mem[pos]);
		 pos = move_pos(pos, UINT_SIZE);
	} else if (mem[pos] == LONG) {
			pos = move_pos(pos, CHAR_SIZE);
			printf("VAR_NAME: %s, ", mem + pos);
			pos = move_pos(pos, strlen(mem + pos)+CHAR_SIZE);
			int16_t *lptr = (int16_t *)(mem + pos);
			printf("LONG: %d\n", *lptr);
			pos = move_pos(pos, LONG_SIZE);
	} else if (mem[pos] == LONGER) {
			pos = move_pos(pos, CHAR_SIZE);
			printf("VAR_NAME: %s, ", mem + pos);
			pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
			int32_t *lgrptr = (int32_t *)(mem + pos);
			printf("LONGER: %d\n", *lgrptr);
			pos = move_pos(pos, LONGER_SIZE);
	} else if (mem[pos] == DOUBLE) {
			pos = move_pos(pos, CHAR_SIZE);
			printf("VAR_NAME: %s, ", mem + pos);
			pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
			double *dptr = (double *)(mem + pos);
			printf("DOUBLE: %f\n", *dptr);
			pos = move_pos(pos, DOUBLE_SIZE);
	} else if (mem[pos] == STR) {
			pos = move_pos(pos, CHAR_SIZE);
			printf("VAR_NAME: %s, ", mem + pos);
			pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
			printf("STRING: %s\n", mem + pos);
			pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
	}else {
			printf("ERROR\n");
	}
}

void break_line(int pos){
	if(pos-pos_count>23){
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
			} else if (mem[pos] == LONG) {
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
	stack_pos=shrink_stack(stack_end,INT_SIZE);
}

void add_var_name_from_stack(){
	int name_size = strlen(mem + stack_pos)+CHAR_SIZE;
	memcpy(mem+var_end, mem+stack_pos, name_size+CHAR_SIZE);
	var_end = move_pos(var_end, name_size);
	stack_pos = shrink_stack(stack_pos, name_size);
}

void add_var_from_stack(){
	int var_type = mem[stack_end];
	int var_size = get_type_size(var_type);
	mem[var_end]=var_type;

	if(var_type==STR){
		var_size=strlen(mem+stack_pos)+CHAR_SIZE;
		memcpy(mem + var_end, mem + stack_pos, var_size);
		var_end = move_pos(var_end, var_size);
		stack_pos = shrink_stack(stack_pos, var_size-1);
	}
	else{
		memcpy(mem + var_end, mem + stack_pos, var_size);
		var_end = move_pos(var_end, var_size);
		stack_pos = shrink_stack(stack_pos, var_size);
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
				stack_end = grow_stack(stack_end,UINT_SIZE);
				memcpy((uint8_t *)(mem + stack_end), &num, UINT_SIZE);
			}
			else if(num>=INT8_MIN && num<= INT8_MAX){
				int8_t num = (int8_t)data;
				type = INT;
				stack_end = grow_stack(stack_end, INT_SIZE);
				memcpy((int8_t *)(mem + stack_end), &num, INT_SIZE);
			}
			else if(num>=INT16_MIN && num<=INT16_MAX){
				uint16_t num = (uint16_t)data;
				type = LONG;
				stack_end = grow_stack(stack_end,LONG_SIZE);
				memcpy((uint16_t *)(mem + stack_end), &num, LONG_SIZE);
			}
			else{
				type = LONGER;
				stack_end = grow_stack(stack_end,LONGER_SIZE);
				memcpy((uint32_t *)(mem + stack_end), &num, LONGER_SIZE);
			}
		}else{
				double num = data;
				type = DOUBLE;
				stack_end = grow_stack(stack_end,DOUBLE_SIZE);
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
    //uint16_t num = (uint16_t)data;
    //type = LONG;
    stack_end = grow_stack(stack_end,LONG_SIZE);
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
		stack_end = grow_stack(stack_end, length+CHAR_SIZE);
		strcpy(mem + stack_end, str);
	}
	check_errors(error);
}

void push_var_name_to_stack(char* name){
	int length=strlen(name)+CHAR_SIZE;
	if(length>MAX_STRING_SIZE){
		printf("%s\n",name);
		error=STRING_TOO_LONG;
	}else{
		stack_end = grow_stack(stack_end, length);
		memcpy(mem + stack_end, name, length);
	}
	check_errors(error);
}

void push_char_to_stack(char data){
	stack_end = grow_stack(stack_end, CHAR_SIZE);
	mem[stack_end]=data;
	check_errors(error);
}

void push_type_to_stack(uint8_t type){
	stack_end = grow_stack(stack_end, INT_SIZE);
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
				dump_mem(pos-UINT_SIZE,var_end);
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
	return pos;
}


void empty_stack(){
	for(int i=stack_end; i<STACK_START; i++) mem[i]='\0';
	stack_pos=stack_end=STACK_START;
}


int add_var_to_stack(char* word, int type, char* value, int addr){
	if(type==NUM){ type=push_num_to_stack(atof(value)); }
	else if(type==CHAR){ push_char_to_stack(value[0]); }
	else if(type==STR){ push_str_to_stack(value); }
  else if(type==FUNC){
    type=FUNC;
    push_func_to_stack(addr+1);
  }
	else {
		printf("ERROR: INVALID TYPE\n");
		return 1;
	}
	push_var_name_to_stack(word);
	push_type_to_stack(type);
	
	add_type_from_stack();
	add_var_name_from_stack();
	add_var_from_stack();
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
	char word[8];
	char data[32];
	int word_type=NUN;
	if(input==PROGRAM){
		ptr=mem;
	}else{
		ptr=cmd;
	}
	
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
        }else{ printf("<IS>"); }
      }else{
        if(state==INWORD){
          word_pos++;
          word[word_pos]='\0';
          state=BETWEEN;
          printf("\n%s:",word);
          if(ptr[i]=='#'){
            add_var_to_stack(word, FUNC, "xo", i);
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
          word_type=push_num_to_stack(atof(data));
          push_var_name_to_stack(word);
          push_type_to_stack(word_type);
          printf("%s",data);
          word_type=NUN;
          sub_state=BETWEEN;
        }
      }
      
      if(ptr[i]=='\n'){putchar(ptr[i]); }
      
      if(ptr[i]==' '){
        putchar(ptr[i]);
      }
      
      if(ptr[i]=='"'){
        if(state!=INSTRING){
          state=INSTRING;
        }
        else{
          state=BETWEEN;
        }
        putchar(ptr[i]);
      }
    }
    
		if(ptr[i]=='#'){
      if(sub_state!=INFUNC){
        //push_num_to_stack(i+1);
        //push_var_name_to_stack(word);
        //word_type=FUNC;
        //push_type_to_stack(word_type);
        if(state==INWORD){
          //word_pos++;
          //word[word_pos]='\0';
          //printf("<\n%s:>",word);
        }
        
        //add_var_to_stack(word, FUNC, "x", i);
        printf("%d",i+1);
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
		//memcpy(mem, program, strlen(program)+1);
		memcpy(mem, *program, strlen(*program));
		interpret(PROGRAM,0,VAR_START);
		//printf("%s",mem);
    if (mem == NULL) {
        printf("\n<Memory not allocated>\n");
        exit(0);
    } else {
        printf("\n<Memory allocated>\n");
				/*
				add_var_to_stack("a", CHAR, "z", 0);
				add_var_to_stack("b", CHAR, "y", 0);
				add_var_to_stack("d", NUM, "3", 0);
				add_var_to_stack("e", NUM,"2342.23423", 0);
				add_var_to_stack("f", NUM,"2342", 0);
				add_var_to_stack("VAR",STR,"Hello How are you?", 0);
				add_var_to_stack("h",NUM,"255", 0);
				add_var_to_stack("i",NUM,"-354", 0);
				add_var_to_stack("k",NUM,"123", 0);
				add_var_to_stack("n",NUM,"254", 0);
				add_var_to_stack("o",CHAR,"x", 0);
				empty_stack();
				*/
				
				dump_all_mem(0,MEM_SIZE);
				if(!get_var("var")){
					printf("VAR NOT FOUND!");
				}
    }
		printf("\nDONE!\n");
    free(mem);
		printf("\n<Memory Freed>\n");
    return 0;
}
