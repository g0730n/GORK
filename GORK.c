#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MEM_SIZE    512
#define STACK_START MEM_SIZE-1

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
#define MAX_OPS      7
#define MAX_BUILTINS 5


//func stack [STACK_END][lpf] [con 0p<1p] [0p+1] [out 0p] [1p=100] [0p=0] [func=addr][STACK_START]
//This needs to be parsed FIFO [STACK_END] <- [STACK_START]
const char* program[]="var\"Hello World!\"x43y255var32.234";
                         
const char* words[MAX_BUILTINS]={"out","lpf","ret","con","NULL"};

const char math_ops[MAX_OPS]={'0','+','-','*','/','%','^'};
enum math_op_type{
  NNN,
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  EXP
};

const char con_ops[MAX_OPS]={'0','=','!','>','<', '&', '?'};
enum con_op_type{
  NN,
  IS,
  NOT,
  MORE,
  LESS,
  AND,
  OR
};

//const char space_syms[] = {};

char cmd[MAX_INPUT_SIZE];
char *mem;
//int stack_end = STACK_START-CHAR_SIZE;
int prog_end;
int var_start;
int stack_pos=STACK_START;
int stack_end=STACK_START;

//int stack_pos = STACK_START;
//int stack_pos = STACK_START-CHAR_SIZE;
int var_end;
uint8_t error;
int pos_count;

int check_builtins(char* word){
  int i;
  for(i=0; i<MAX_BUILTINS; i++){
    if(word[0]==words[i][0]){
      if(!strcmp(word,words[i])){return i;}
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
    FUNC,
    MATH,
    CON,
    SPACE,
    SYS
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
  MEMORY_NOT_ALLOCATED,
  INVALID_TYPE
};

enum states {
  BETWEEN,
  INWORD,
  INSTRING,
  INVAR,
  INFUNC,
  INNUM,
  INMATH,
  INCON,
  INOUT
};

int check_errors(int e){
  if(e>NONE){
    printf("\nERROR: ");
    if(e==OUT_OF_MEMORY){printf("OUT OF MEMORY!");}
    else if(e==NUMBER_TOO_BIG){printf("NUMBER TOO BIG!");}
    else if(e==NUMBER_TOO_SMALL){printf("NUMBER TOO SMALL!");}
    else if(e==STRING_TOO_LONG){printf("STRING TOO LONG!");}
    else if(e==STACK_OVERFLOW){printf("STACK OVERFLOW!");}
    else if(e==STACK_UNDERFLOW){printf("STACK UNDERFLOW!");}
    else if(e==ADDRESS_OUT_OF_RANGE){printf("ADDRESS OUT OF RANGE!");}
    else if (e==MEMORY_NOT_ALLOCATED){printf("MEMORY NOT ALLOCATED!");}
    else if (e==INVALID_TYPE){printf("INVALID TYPE!");}
    else {printf("UNKNOWN ERROR!");}
    free(mem);
    exit(0);
  }
}

int get_type_size(int type){
  int size;
  if(type==NUN){size=0;}
  else if(type==CHAR){size=CHAR_SIZE;}
  else if(type==UINT){size=UINT_SIZE;}
  else if(type==INT){ size=INT_SIZE; }
  else if(type==LONG||type==FUNC){size=LONG_SIZE;}
  else if(type==LONGER){size=LONGER_SIZE;}
  else if(type==DOUBLE){size=DOUBLE_SIZE;}
  else if(type==STR){size=0;}
  else{
    printf("<TYPE: %d>",type);
    check_errors(INVALID_TYPE);
  }
  return size;
}

int move_pos(int pos, int amount){
  pos+=amount;
  if(pos>=stack_end){check_errors(OUT_OF_MEMORY);}
  return pos;
}

int grow_stack(int pos, int amount){

}

int shrink_stack(int pos, int amount){
  
}

int grow_stack_pointer(int pos, int amount){
  pos-=amount;
  if(pos<=var_end){
    printf("<STACK POS: %d>\n",pos);
    check_errors(STACK_OVERFLOW);
    }
  return pos;
}

int shrink_stack_pointer(int pos, int amount){
  pos+=amount;
  if(pos>STACK_START){check_errors(STACK_UNDERFLOW);}
  return pos;
}

int dump_mem(int pos) {
  uint8_t type=mem[pos];
  uint8_t size=get_type_size(type);
  pos = move_pos(pos, CHAR_SIZE);
  pos = move_pos(pos, strlen(mem + pos) + CHAR_SIZE);
  
  if (type==CHAR){
    printf("%c",mem[pos]);
  }else if (type==INT||type==UINT){
    printf("%d",mem[pos]);
  }else if(type==LONG||type==FUNC){
    int16_t *lptr=(int16_t*)(mem+pos);
    printf("%d",lptr);
  }else if(type==LONGER){
    int32_t *lgrptr=(int32_t*)(mem+pos);
    printf("%d",lgrptr);
  }else if(type==DOUBLE){
    double *dptr=(double *)(mem+pos);
    printf("%f",dptr);
  }else if(type==STR){
    size=strlen(mem+pos)+CHAR_SIZE;
    printf("%s",mem+pos);
  }else{check_errors(UNKNOWN);}
  pos=move_pos(pos,size);
  return size;
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
/*
*/
void dump_all_mem(int pos, int length) {
  pos_count=pos;
  uint8_t type;
  uint8_t size;
  
  while(pos<length){
    if(mem[pos]>=0 && mem[pos]<=9){putchar((char)mem[pos]+48);}
    else{putchar(mem[pos]);}
    
    if(pos>=var_start && pos<=var_end || pos>=stack_end){
      type=mem[pos];
      size=get_type_size(type);
      pos+=CHAR_SIZE;
      break_line(pos);
      pos=print_string(pos);
      
      if(type==CHAR){printf("%c",mem[pos]);}
      else if(type==INT){printf("%d",(int8_t)mem[pos]);}
      else if(type==UINT){printf("%d",(uint8_t)mem[pos]);}
      else if(type==LONG||type==FUNC){
        for(int i=0;i<LONG_SIZE;i++){putchar('l');}
      }
      else if(type==LONGER){
        for(int i=0;i<LONGER_SIZE;i++){putchar('L');}
      }
      else if(type==DOUBLE){
        for(int i=0;i<DOUBLE_SIZE;i++){putchar('d');}
      }
      else if(type==STR){pos=print_string(pos);}
      break_line(pos);
      pos+=size;
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
  int size = get_type_size(var_type);
  mem[var_end]=var_type;

  if(var_type==STR){
    size=strlen(mem+stack_pos)+CHAR_SIZE;
    memcpy(mem + var_end, mem + stack_pos, size);
    var_end = move_pos(var_end, size);
    stack_pos = shrink_stack_pointer(stack_pos, size-1);
  }
  else{
    memcpy(mem + var_end, mem + stack_pos, size);
    var_end = move_pos(var_end, size);
    stack_pos = shrink_stack_pointer(stack_pos, size);
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
    printf("%d\n",addr);
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

int get_var_value_addr(int pos){
  pos++;
  pos+=strlen(mem+pos)+CHAR_SIZE;
  return pos;
}

int get_var_name_addr(int pos){
  pos++;
  return pos;
}

int get_var_size(int pos){
  uint8_t type=mem[pos];
  uint8_t size;
  pos=get_var_value_addr(pos);
  if(type==STR){
    size=strlen(mem + pos)+CHAR_SIZE;
  }else{
    size=get_type_size(type);
  }
  return size;
}

int get_var_type(int pos){
  int type=mem[pos];
  return type;
}

int move_to_var_end(int pos){
  int type=get_var_type(pos);
  int size=get_var_size(pos);
  pos=get_var_value_addr(pos);
  pos+=size;
  return pos;
}

int get_var(char *var_name){
  //find variable from memory's variable table
  int pos=var_start;
  while(pos<var_end){
    if(mem[get_var_name_addr(pos)]==var_name[0]){
      if(!strcmp(mem+get_var_name_addr(pos), var_name)){
        //printf("POS:%d",pos);
        return pos;
      }
      pos = move_to_var_end(pos);
    }
    else{ pos = move_to_var_end(pos); }
  }
  return -1;
}

void empty_stack(){
  memset(mem + stack_end, '\0', (STACK_START-stack_end)*CHAR_SIZE); 
  stack_pos=stack_end=STACK_START;
}

void shift_var_table(uint16_t pos, int16_t old_size, int16_t new_size){
  int shift_point = pos+old_size;
  int shift_to_point = pos+new_size;
  int data_size = var_end-shift_point+new_size;
  //printf("<SP:%d, STP:%d, DS%d, OS:%d, NS:%d>",shift_point,shift_to_point,data_size,old_size,new_size);
  memmove(mem + shift_to_point, mem + shift_point, data_size); 
}

void push_var_to_table(char *word){
  int addr=get_var(word);
  if(addr<0){
    //int var_length=get_var_from_stack(addr);
    add_type_from_stack();
    add_var_name_from_stack();
    add_var_from_stack();
    addr=get_var(word);
  }else{
    uint8_t stack_type=get_var_type(stack_end);
    uint8_t table_type=get_var_type(addr);
    uint8_t stack_length=get_var_size(stack_end);
    uint8_t table_length=get_var_size(addr);
    uint16_t table_addr=get_var_value_addr(addr);
    uint16_t stack_addr=get_var_value_addr(stack_end);
    //printf("ST:%d,SL:%d,SA:%d  TT:%d,TL:%d,TA:%d",stack_type,stack_length,stack_addr,table_type,table_length,table_addr);
    if(stack_type!=table_type){
      //printf("\nTEST1\n");
      mem[addr]=mem[stack_end];
    }
    if(stack_length!=table_length){
      //printf("\nTEST2\n");
      shift_var_table(table_addr, table_length, stack_length);
      //printf("\nTEST3\n");
    }
    //memcpy(mem + stack_addr, mem + table_addr, length);
    
    memcpy(mem + table_addr, mem + stack_addr, stack_length);
    var_end+=stack_length-table_length;
  }
  
}

int add_var_to_stack(char* word, int type, char* value, int addr){
  if(type==NUM){type=push_num_to_stack(atof(value));}
  else if(type==CHAR){push_char_to_stack(value[0]);}
  else if(type==STR){push_str_to_stack(value);}
  else if(type==FUNC){push_func_to_stack(addr+1);}
  else {
    check_errors(INVALID_TYPE);
    return 1;
  }
  push_var_name_to_stack(word);
  push_type_to_stack(type);
  return 0;
}

int ismath(char sym){
  for(int i=0; i<MAX_OPS+CHAR_SIZE; i++){
    if(math_ops[i]==sym){return i;}
  }
  return 0;
}

int iscon(char sym){
  for(int i=0; i<MAX_OPS+CHAR_SIZE; i++){
    if(con_ops[i]==sym){return i;}
  }
  return 0;
}

int parse_stack(){
  stack_pos=stack_end;
  uint8_t type=mem[stack_end];
  uint8_t size=get_type_size(type);
  stack_pos++;
  uint8_t btype=check_builtins(mem+stack_pos);
  if(btype!=MAX_BUILTINS){
    //printf("$$%d$$",btype);
    stack_pos+=strlen(mem+stack_pos)+CHAR_SIZE;
  }else{
    push_var_to_table(mem+stack_pos);
    if(type==STR){size=strlen(mem+stack_pos)+CHAR_SIZE; }
    stack_pos+=strlen(mem+stack_pos)+CHAR_SIZE+size;
  }
  return 0;
}

char get_next_token(int pos){
  while(mem[pos]==' '){pos++;}
  return mem[pos];
}

int16_t get_func_address(char * word){
  int pos = get_var_value_addr(get_var(word));
  int16_t lptr=(int16_t)(mem[pos]);
  //printf("[[%d, %d]]]",lptr, pos);
  return lptr;
}

int interpret(int input, int start, int stop){
  int pos;
  int this_word;
  char *ptr;
  char word[MAX_WORD_SIZE];
  char data[MAX_INPUT_SIZE];
  
  uint8_t expr_type;
  uint8_t top_state;
  uint8_t mid_state;
  uint8_t sub_state;
  
  uint8_t word_pos;
  uint8_t data_pos;
  
  uint8_t sys;
  
  
  if(input==PROGRAM){ptr=mem;}else{ptr=cmd;}
  
  for(pos=start;pos<stop;pos++){
    if(ptr[pos]=='\0'){
      printf("\nPROGRAM SIZE: %d\n",pos);
      return pos;
    }
    if(isalpha(ptr[pos])){
      if(sub_state!=INSTRING && sub_state!=INFUNC){
        if(mid_state!=INWORD){
          mid_state=sub_state=INWORD;
          word_pos=0;
          word[word_pos]=ptr[pos];
        }
        else{
          word_pos++;
          word[word_pos]=ptr[pos];
        }
      }else{
        if(sub_state==INSTRING){
          data[data_pos]=ptr[pos];
          data_pos++;
        }
      }
    }else{
      if(sub_state!=INSTRING && sub_state!=INFUNC){
        if(mid_state==INWORD){
          mid_state=sub_state=BETWEEN;
          word_pos++;
          word[word_pos]='\0';
          //printf("<%s>",word);
          sys=check_builtins(word);
          if(sys!=MAX_BUILTINS){
            if(!strcmp(words[sys],"out")){
              //here we launch OUT function
              //printf("(OUT)");
              top_state=INOUT;
            }
          }
          this_word=get_var(word);
          if(this_word>0){
            uint8_t this_word_type=get_var_type(this_word);
            if(this_word_type==FUNC){
              //here we launch functions
              add_var_to_stack(word, FUNC, "", get_func_address(word));
              parse_stack();
              //empty_stack();
              //printf("(FUNC: %d)",get_func_address(word));
            }
            //printf("<<%s>>",word);
          }
          
        }
        //if(isspace(ptr[pos])){ }
        if(isdigit(ptr[pos])){
          if(sub_state!=INNUM){
            sub_state=INNUM;
            data_pos=0;
            if(ptr[pos-1]=='-' && ptr[pos-2]==' '){
              data[data_pos]='-';
              data_pos++;
            }
            data[data_pos]=ptr[pos];
            if(!isdigit(ptr[pos+1])){
              data_pos++;
              if(ptr[pos+1]=='.'){
                pos++;
                data[data_pos]=ptr[pos];
              }else{
                data[data_pos]='\0';
                add_var_to_stack(word, NUM, data, pos);
                parse_stack();
                //empty_stack();
              }
            }
          }else{
            data_pos++;
            data[data_pos]=ptr[pos];
            if(!isdigit(ptr[pos+1])){
              data_pos++;
              if(ptr[pos+1]=='.'){
                pos++;
                data[data_pos]=ptr[pos];
              }else{
                data[data_pos]='\0';
                add_var_to_stack(word, NUM, data, pos);
                parse_stack();
                //empty_stack();
              }
            }
          }
        }
        else if(ispunct(ptr[pos])){
          if(ptr[pos]=='"'){
            sub_state=INSTRING;
            data_pos=0;
          }
          else if(ptr[pos]=='#'){
            if(top_state!=INFUNC){
              sub_state=INFUNC;
              add_var_to_stack(word, FUNC, "", pos);
              parse_stack();
              //empty_stack();
            }
          }
          else{ //push_char_to_stack(ptr[pos]);
          }
        }
      }
      else{
        if(sub_state==INSTRING){
          if(ptr[pos]=='"'){
            sub_state=BETWEEN;
            data[data_pos]='\0';
            //printf("(%s)",data);
            add_var_to_stack(word, STR, data, pos);
            if(get_next_token(pos)!='+'){
              parse_stack();
              //empty_stack();
            }
          }else{
            data[data_pos]=ptr[pos];
            data_pos++;
          }
        }
        else if(sub_state==INFUNC){
          if(ptr[pos]=='#'){
            sub_state=BETWEEN;
          }
        }
      }
    }
    break_line(pos);
  }
  
  return pos;
}

int main() {
  mem=(char *)calloc(MEM_SIZE,CHAR_SIZE);
  
  prog_end=strlen(*program)+CHAR_SIZE;
  memcpy(mem,*program,prog_end);
  
  var_start=var_end=prog_end;
  
  if(mem==NULL){check_errors(MEMORY_NOT_ALLOCATED);}
  else{
    printf("\n<Memory allocated>\n");
    printf("\n-----------DEBUG---------\n");
    interpret(PROGRAM,0,var_start);
    dump_all_mem(0,STACK_START);

    printf("\n-------END DEBUG---------\n\n");
  }
  
  free(mem);
  printf("\n<Memory Freed>\n");
  return 0;
}
