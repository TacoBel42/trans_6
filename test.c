#include "lib.hpp"
#include "miniunit.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void test_setup(void) {
  current_stream = NULL;
  // ht = ht_create();
}

void test_teardown(void) {
  fseek(current_stream, 0, SEEK_END);
  // ht_destroy(ht);
}

void write_string_to_stdin(char *str) {
  current_stream = fmemopen(str, strlen(str), "r+");
}

MU_TEST(test_scaner_id) {
  char input[] = "i";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  mu_check(symbol == ID);
  mu_check(strcmp(input, buf) == 0);
}
MU_TEST(test_scaner_sum) {
  char input[] = "1+2+3+4";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  mu_check(symbol == NUM);
  mu_check(value == 1);

  symbol = scan();
  mu_check(symbol == PLUS);

  symbol = scan();
  mu_check(symbol == NUM);
  mu_check(value == 2);

  symbol = scan();
  mu_check(symbol == PLUS);

  symbol = scan();
  mu_check(symbol == NUM);
  mu_check(value == 3);

  symbol = scan();
  mu_check(symbol == PLUS);

  symbol = scan();
  mu_check(symbol == NUM);
  mu_check(value == 4);
}

MU_TEST(test_scaner_example) {
  char input[] = "int a = 1+2; \n bool x,z=TRUE, c= FALSE, f;";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  mu_check(symbol == INT);

  symbol = scan();
  mu_check(symbol == ID);
  mu_check(strcmp("a", buf) == 0);

  symbol = scan();
  mu_check(symbol == EQUAL);

  symbol = scan();
  mu_check(symbol == NUM);
  mu_check(value == 1);

  symbol = scan();
  mu_check(symbol == PLUS);

  symbol = scan();
  mu_check(symbol == NUM);
  mu_check(value == 2);

  symbol = scan();
  mu_check(symbol == SEMICOLON);

  symbol = scan();
  mu_check(symbol == BOOL);

  symbol = scan();
  mu_check(symbol == ID);
  mu_check(strcmp("x", buf) == 0);

  symbol = scan();
  mu_check(symbol == COMMA);

  symbol = scan();
  mu_check(symbol == ID);
  mu_check(strcmp("z", buf) == 0);

  symbol = scan();
  mu_check(symbol == EQUAL);

  symbol = scan();
  mu_check(symbol == TRUE);

  symbol = scan();
  mu_check(symbol == COMMA);

  symbol = scan();
  mu_check(symbol == ID);
  mu_check(strcmp("c", buf) == 0);

  symbol = scan();
  mu_check(symbol == EQUAL);

  symbol = scan();
  mu_check(symbol == FALSE);

  symbol = scan();
  mu_check(symbol == COMMA);

  symbol = scan();
  mu_check(symbol == ID);
  mu_check(strcmp("f", buf) == 0);

  symbol = scan();
  mu_check(symbol == SEMICOLON);

  mu_check(strlen(input) == ftell(current_stream));
}

MU_TEST(test_number_tail_empty) {
  char input[] = ",";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  int result = number_tail();

  mu_check(result == 0);
}

MU_TEST(test_number_tail_nonempty) {
  char input[] = " + 10,";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  int result = number_tail();

  mu_check(result == 10);
}

MU_TEST(test_number) {
  char input[] = "1+2+3+4,";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  int result = number();

  mu_check(result == 10);
}

MU_TEST(test_assign_int_nonempty) {
  char input[] = "= 1+2+3+4,";
  write_string_to_stdin(&input[0]);

  vars_map vars;

  symbol = scan();
  auto result = assign_int(vars);

  try {
    mu_check(std::get<int>(result) == 10);
  } catch (const std::bad_variant_access &e) {
  }
}

MU_TEST(test_assign_int_empty) {
  char input[] = ",";
  write_string_to_stdin(&input[0]);

  vars_map vars;

  symbol = scan();
  auto result = assign_int(vars);

  try {
    std::get<std::monostate>(result);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be empty!\n");
  }
}

void check_val_int(vars_map &vars, bool noval, int val, char *str) {
  mu_check(vars[str].name == str);
  if (noval) {
    try {
      std::get<std::monostate>(vars[str].value);
    } catch (const std::bad_variant_access &e) {
      mu_fail("Should be empty?!\n");
    }

  } else {
    try {
      mu_check(std::get<int>(vars[str].value) == val);
    } catch (const std::bad_variant_access &e) {
      mu_fail("Should be ?!\n");
    }
  }
  mu_check(vars[str].type == Int);
}

MU_TEST(test_var_int) {
  char input[] = "int a,b,c,d = 10, f,e;";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  vars_map vars;
  var_int(vars);

  std::cout << "\n" << vars.size() << std::endl;
  mu_check(vars.size() == 6);

  check_val_int(vars, false, 10, "a");
  check_val_int(vars, false, 10, "b");
  check_val_int(vars, false, 10, "c");
  check_val_int(vars, false, 10, "d");
  check_val_int(vars, true, 10, "f");
  check_val_int(vars, true, 10, "e");

  // mu_check(strlen(input) - 1 == ftell(current_stream));
  // symbol = scan();
  // mu_check(symbol == EQUAL);
}

void check_val_bool(vars_map &vars, bool noval, bool val, char *str) {
  mu_check(vars[str].name == str);
  if (noval) {
    try {
      std::get<std::monostate>(vars[str].value);
    } catch (const std::bad_variant_access &e) {
      mu_fail("Should be empty?!\n");
    }

  } else {
    try {
      mu_check(std::get<bool>(vars[str].value) == val);
    } catch (const std::bad_variant_access &e) {
      mu_fail("Should be ?!\n");
    }
  }
  mu_check(vars[str].type == Bool);
}

MU_TEST(test_var_bool) {
  char input[] = "bool a,b,c,d = true, f,e;";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  vars_map vars;
  var_bool(vars);

  std::cout << "\n" << vars.size() << std::endl;
  mu_check(vars.size() == 6);

  check_val_bool(vars, false, true, "a");
  check_val_bool(vars, false, true, "b");
  check_val_bool(vars, false, true, "c");
  check_val_bool(vars, false, true, "d");
  check_val_bool(vars, true, true, "f");
  check_val_bool(vars, true, true, "e");

  // mu_check(strlen(input) - 1 == ftell(current_stream));
  // symbol = scan();
  // mu_check(symbol == EQUAL);
}

// MU_TEST(test_var_int) {
//   // Тестирует и var_list и var_list_tail
//   char input[] = "int a,b,c,d = 1+2+3+4, e=6, f;";
//   write_string_to_stdin(&input[0]);
//
//   symbol = scan();
//   vars_map vars;
//   var_int(vars);
//
//   mu_check(vars.size() == 6);
//
//   mu_check(vars["a"].name == "a");
//   mu_check(vars["a"].type == Int);
//   try {
//     mu_check(std::get<int>(vars["a"].value) == 10);
//   } catch (const std::bad_variant_access &e) {
//     mu_fail("Should be 10!\n");
//   }
//
//   mu_check(vars["b"].name == "b");
//   mu_check(vars["b"].type == Int);
//   try {
//     mu_check(std::get<int>(vars["b"].value) == 10);
//   } catch (const std::bad_variant_access &e) {
//     mu_fail("Should be 10!\n");
//   }
//
//   mu_check(vars["c"].name == "c");
//   mu_check(vars["c"].type == Int);
//   try {
//     mu_check(std::get<int>(vars["c"].value) == 10);
//   } catch (const std::bad_variant_access &e) {
//     mu_fail("Should be 10!\n");
//   }
//
//   mu_check(vars["d"].name == "d");
//   mu_check(vars["d"].type == Int);
//   try {
//     mu_check(std::get<int>(vars["d"].value) == 10);
//   } catch (const std::bad_variant_access &e) {
//     mu_fail("Should be 10!\n");
//   }
//
//   mu_check(vars["e"].name == "e");
//   mu_check(vars["e"].type == Int);
//   try {
//     mu_check(std::get<int>(vars["e"].value) == 6);
//   } catch (const std::bad_variant_access &e) {
//     mu_fail("Should be 6!\n");
//   }
//
//   mu_check(vars["f"].name == "f");
//   mu_check(vars["f"].type == Int);
//   try {
//     std::get<std::monostate>(vars["f"].value);
//   } catch (const std::bad_variant_access &e) {
//     mu_fail("Should be empty!\n");
//   }
// }

MU_TEST(test_varint) {
  // Тестирует var с заданным int
  char input[] = "int a,b,c,d = 1+2+3+4, e=6, f;";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  vars_map vars;
  var(vars);

  mu_check(vars.size() == 6);

  mu_check(vars["a"].name == "a");
  mu_check(vars["a"].type == Int);
  try {
    mu_check(std::get<int>(vars["a"].value) == 10);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be 10!\n");
  }

  mu_check(vars["b"].name == "b");
  mu_check(vars["b"].type == Int);
  try {
    mu_check(std::get<int>(vars["b"].value) == 10);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be 10!\n");
  }

  mu_check(vars["c"].name == "c");
  mu_check(vars["c"].type == Int);
  try {
    mu_check(std::get<int>(vars["c"].value) == 10);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be 10!\n");
  }

  mu_check(vars["d"].name == "d");
  mu_check(vars["d"].type == Int);
  try {
    mu_check(std::get<int>(vars["d"].value) == 10);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be 10!\n");
  }

  mu_check(vars["e"].name == "e");
  mu_check(vars["e"].type == Int);
  try {
    mu_check(std::get<int>(vars["e"].value) == 6);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be 6!\n");
  }

  mu_check(vars["f"].name == "f");
  mu_check(vars["f"].type == Int);
  try {
    std::get<std::monostate>(vars["f"].value);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be empty!\n");
  }
}

MU_TEST(test_varbool) {
  // Тестирует var с заданным int
  char input[] = "bool a,b,c,d = true, e=false, f;";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  vars_map vars;
  var(vars);

  mu_check(vars.size() == 6);

  mu_check(vars["a"].name == "a");
  mu_check(vars["a"].type == Bool);
  try {
    mu_check(std::get<bool>(vars["a"].value) == true);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be true!\n");
  }

  mu_check(vars["b"].name == "b");
  mu_check(vars["b"].type == Bool);
  try {
    mu_check(std::get<bool>(vars["b"].value) == true);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be true!\n");
  }

  mu_check(vars["c"].name == "c");
  mu_check(vars["c"].type == Bool);
  try {
    mu_check(std::get<bool>(vars["c"].value) == true);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be true!\n");
  }

  mu_check(vars["d"].name == "d");
  mu_check(vars["d"].type == Bool);
  try {
    mu_check(std::get<bool>(vars["d"].value) == true);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be true!\n");
  }

  mu_check(vars["e"].name == "e");
  mu_check(vars["e"].type == Bool);
  try {
    mu_check(std::get<bool>(vars["e"].value) == false);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be false!\n");
  }

  mu_check(vars["f"].name == "f");
  mu_check(vars["f"].type == Bool);
  try {
    std::get<std::monostate>(vars["f"].value);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be empty!\n");
  }
}

MU_TEST(test_example) {
  // Тестирует выражение из примера
  char input[] = "int a = 1+2;bool x, z = TRUE;";
  write_string_to_stdin(&input[0]);

  symbol = scan();
  vars_map vars;
  start(vars);

  mu_check(vars.size() == 3);

  mu_check(vars["a"].name == "a");
  mu_check(vars["a"].type == Int);
  try {
    mu_check(std::get<int>(vars["a"].value) == 3);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be 3!\n");
  }

  mu_check(vars["x"].name == "x");
  mu_check(vars["x"].type == Bool);
  try {
    mu_check(std::get<bool>(vars["x"].value) == true);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be true!\n");
  }

  mu_check(vars["z"].name == "z");
  mu_check(vars["z"].type == Bool);
  try {
    mu_check(std::get<bool>(vars["z"].value) == true);
  } catch (const std::bad_variant_access &e) {
    mu_fail("Should be true!\n");
  }
}

MU_TEST_SUITE(test_suite) {
  MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
  MU_RUN_TEST(test_scaner_sum);
  MU_RUN_TEST(test_scaner_id);
  MU_RUN_TEST(test_scaner_example);
  MU_RUN_TEST(test_number_tail_empty);
  MU_RUN_TEST(test_number_tail_nonempty);
  MU_RUN_TEST(test_number);
  MU_RUN_TEST(test_assign_int_nonempty);
  MU_RUN_TEST(test_assign_int_empty);
  MU_RUN_TEST(test_var_int);
  MU_RUN_TEST(test_var_bool);
  //   MU_RUN_TEST(test_var_int);
  //  MU_RUN_TEST(test_varint);
  //  MU_RUN_TEST(test_varbool);

  MU_RUN_TEST(test_example);
}

int main() {
  MU_RUN_SUITE(test_suite);
  MU_REPORT();
  return MU_EXIT_CODE;
}
