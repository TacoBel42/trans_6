#include "lib.hpp"

int main() {
  // char input[] = "int a = 1+2;bool x, z = TRUE;";
  char input[] =
      "int a = 1+2; int b, c = 42, d = 1+2+3, b0; bool e,f = false, e0; int "
      "g, h = 42, k = 1+2+3;";
  current_stream = fmemopen(input, strlen(input), "r+");
  printf("%s\n", input);
  symbol = scan();
  vars_map vars;
  var(vars);
  print_vars_map(vars);

  if (!(symbol == EOF ||
        strlen(input) ==
            ftell(current_stream))) // fmemopen не возвращает в конце потока EOF
    error("Still some input exists");

  printf("\n");
  return 0;
}
