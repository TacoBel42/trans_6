#pragma once
#include <iostream>
#include <string.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// simple tokens
#define SEMICOLON 256
#define COMMA 261
#define EQUAL 262
#define PLUS 265

// complex tokens
#define TRUE 257
#define FALSE 258
#define BOOL 259
#define INT 260

// lexems
#define ID 263
#define NUM 264

using OptionInt = std::variant<std::monostate, int>;
using OptionBool = std::variant<std::monostate, bool>;

int error(char *msg) {
  printf("%s\n", msg);
  exit(1);
}

int symbol;
int value;
char buf[256];
int size = -1;
FILE *current_stream;
int t;

enum Type { Bool, Int };

typedef struct {
  std::string name;
  std::variant<std::monostate, int, bool> value;
  Type type;
} Var;

// std::vector<Var *> vars;
using vars_map = std::unordered_map<std::string, Var>;
// vars_map vars;
//
int scan();
int number();
int number_tail();
OptionInt assign_int();
void var_list(vars_map &);
void var_list_tail(vars_map &);
void var_int(vars_map &);
void var_int_tail(vars_map &);

bool boolean();
OptionBool assign_bool();
void var_bool(vars_map &);
void var_bool_tail(vars_map &);

void print_vars_map(vars_map &vars) {
  for (auto &pair : vars) {
    if (pair.second.type == Bool) {
      std::cout << "boolean ";
      std::cout << pair.second.name << " ";

      if (auto val_bool = std::get_if<bool>(&(pair.second.value))) {
        if (*val_bool) {
          std::cout << " = true";
        } else {
          std::cout << " = false";
        }
      } else {
        std::cout << " do not have value";
      }
    } else {
      std::cout << "integer ";
      std::cout << pair.second.name;

      if (auto val_int = std::get_if<int>(&(pair.second.value))) {
        std::cout << " = " << *val_int << " ";
      } else {
        std::cout << " do not have value";
      }
    }

    std::cout << "\n";
  }
}

// start: var;
//
// var: var_bool ';' var | var_int ';' var |;
//
// <var_bool> ::= 'bool' <var_list> <assign_bool> <var_bool_tail>
// <var_bool_tail> ::= ',' var_list assign_bool var_bool_tail | ε
// <var_int> ::= 'int' <var_list> <assign_int> <var_int_tail>
// <var_int_tail> ::= ',' var_list assign_int var_int_tail | ε
//
// var_list: ID var_list_tail;
// var_list_tail: | ',' var_list;
//
// assign_bool: | '=' bool;
// boolean: 'true' | 'false';
//
// assign_int: | '=' number;
// number: NUM number_tail;
// number_tail: | '+' number;

void var(vars_map &vars) {
  if (symbol == BOOL) {
    // var ::= var_bool ';' var
    var_bool(vars);

    if (symbol != SEMICOLON) {
      error("Expected ; in bool var!");
    }

    symbol = scan();
    var(vars);
  } else if (symbol == INT) {
    // var ::= var_int ';' var
    var_int(vars);

    if (symbol != SEMICOLON) {
      error("Expected ; in int var!");
    }

    symbol = scan();
    var(vars);
  } else {
    // var ::= epsilon
    return;
  }
}

void var_bool_common(vars_map &vars) {
  // <var_list> <assign_bool> <var_bool_tail>
  vars_map vars_local;

  symbol = scan();
  var_list(vars_local);

  auto val = assign_bool();

  for (auto &pair : vars_local) {
    if (vars.count(pair.first) > 0)
      error("Variable already exists!");
    if (auto val_bool = std::get_if<bool>(&val)) {
      pair.second.value = *val_bool;
    } else {
      pair.second.value = std::monostate{};
    }
    pair.second.type = Bool;
  }

  var_bool_tail(vars);

  vars.insert(vars_local.begin(), vars_local.end());
}

void var_bool(vars_map &vars) {
  // <var_bool> ::= 'bool' <var_list> <assign_bool> <var_bool_tail>
  if (symbol != BOOL) {
    error("Expected bool!");
  }
  var_bool_common(vars);
}

void var_bool_tail(vars_map &vars) {
  // <var_bool_tail> ::= ',' var_list assign_bool var_bool_tail | ε
  if (symbol != COMMA) {
    return;
  }
  var_bool_common(vars);
}

OptionBool assign_bool() {
  // assign_bool : | '=' bool;
  OptionBool result;
  if (symbol == EQUAL) {
    symbol = scan();
    result = boolean();
    symbol = scan();
  }

  return result;
}

bool boolean() {
  if (symbol == TRUE) {
    return true;
  }
  if (symbol == FALSE)
    return false;
  error("Wrong type!");
}

void var_int_common(vars_map &vars) {

  vars_map vars_local;

  symbol = scan();
  var_list(vars_local);

  auto val = assign_int();

  for (auto &pair : vars_local) {
    if (vars.count(pair.first) > 0)
      error("Variable already exists!");
    if (auto val_int = std::get_if<int>(&val)) {
      pair.second.value = *val_int;
    } else {
      pair.second.value = std::monostate{};
    }
    pair.second.type = Int;
  }

  var_int_tail(vars);

  vars.insert(vars_local.begin(), vars_local.end());
}

void var_int(vars_map &vars) {
  // <var_int> ::= 'int' <var_list> <assign_int> <var_int_tail>
  if (symbol != INT) {
    error("Expected int!");
  }
  var_int_common(vars);
}

void var_int_tail(vars_map &vars) {
  // <var_int_tail> ::= ',' var_list assign_int var_int_tail | ε
  if (symbol != COMMA) {
    return;
  }
  var_int_common(vars);
}

void var_list_tail(vars_map &vars) {
  // var_list_tail: | ',' var_list;
  if (symbol != COMMA) {
    return;
  }

  symbol = scan();
  var_list(vars);
}

void var_list(vars_map &vars) {
  // var_list: ID var_list_tail;
  if (symbol != ID)
    error("Expected ID");
  std::string id(buf);
  vars[id] = {};
  vars[id].name = std::string(id);

  symbol = scan();
  var_list_tail(vars);
}

OptionInt assign_int() {
  // assign_int: | '=' number;
  OptionInt result;
  if (symbol == EQUAL) {
    symbol = scan();
    result = number();
  }

  return result;
}

int number() {
  if (symbol == NUM) {
    // number: NUM number_tail;
    int val = value;

    symbol = scan();
    int tail = number_tail();

    return val + tail;

  } else {
    error("Error in number!");
  }
}

int number_tail() {
  if (symbol == PLUS) {
    // number_tail: '+' number;

    symbol = scan();
    int val = number();

    return val;
  } else {
    // number_tail: epsilon
    return 0;
  }
}

int is_any_separator(char t) {
  if (t == ' ' || t == '\t' || t == '\n' || t == '\r' || t == EOF)
    return 1;
  return 0;
}

int is_simple_token(char t) {
  if (t == ';' || t == ',' || t == '=' || t == '+')
    return 1;
  return 0;
}

int return_simple_token(char t) {
  switch (t) {
  case ';':
    return SEMICOLON;
  case '=':
    return EQUAL;
  case ',':
    return COMMA;
  case '+':
    return PLUS;
  }
  error("Internal error");
}

int is_true() {
  if (size == 4) {
    if (buf[0] == 't' && buf[1] == 'r' && buf[2] == 'u' && buf[3] == 'e')
      return 1;
    if (buf[0] == 'T' && buf[1] == 'R' && buf[2] == 'U' && buf[3] == 'E')
      return 1;
  }
  return 0;
}

int is_false() {
  if (size == 5) {
    if (buf[0] == 'f' && buf[1] == 'a' && buf[2] == 'l' && buf[3] == 's' &&
        buf[4] == 'e')
      return 1;
    if (buf[0] == 'F' && buf[1] == 'A' && buf[2] == 'L' && buf[3] == 'S' &&
        buf[4] == 'E')
      return 1;
  }
  return 0;
}

int is_bool() {
  if (size == 4) {
    if (buf[0] == 'b' && buf[1] == 'o' && buf[2] == 'o' && buf[3] == 'l')
      return 1;
  }
  return 0;
}
int is_int() {
  if (size == 3) {
    if (buf[0] == 'i' && buf[1] == 'n' && buf[2] == 't')
      return 1;
  }
  return 0;
}

int scan() {
  for (int i = 0; i < 255; i++)
    buf[i] = '\0';
  size = -1;
  while (1) {
    t = getc(current_stream);
    if (is_any_separator(t)) {
      if (t == EOF)
        return EOF;
    } else if (is_simple_token(t)) {
      return return_simple_token(t);
    } else if (t >= '0' && t <= '9') {
      value = t - '0';
      t = getc(current_stream);
      while (t >= '0' && t <= '9') {
        value = value * 10 + t - '0';
        t = getc(current_stream);
      }
      if (is_simple_token(t))
        ungetc(t, current_stream);
      return NUM;
    } else {
      size = 0;
      buf[size] = t;
      size = size + 1;
      while (1) {
        if (is_true())
          return TRUE;
        if (is_false())
          return FALSE;
        if (is_bool())
          return BOOL;
        if (is_int())
          return INT;
        if (size + 1 > 255)
          error("Exceed size of IDENTIFIER (256)");
        t = getc(current_stream);
        if (is_simple_token(t) || is_any_separator(t)) {
          if (is_simple_token(t)) {
            ungetc(t, current_stream);
          }
          break;
        }
        buf[size] = t;
        size = size + 1;
      }
      buf[size] = '\0';
      return ID;
    }
  }
}
