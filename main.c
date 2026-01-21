#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINE_LEN 256
#define MAX_TOKENS 64
#define MAX_VARS 32

typedef struct {
  char name[32];
  int value;
} Var;

typedef enum { ADD = 1, SUB = -1, MUL = 2, DIV = 3, MOD = 4 } Operator;

Var vars[MAX_VARS];
int var_count = 0;
Var *get_var(const char *name) {
  for (int i = 0; i < MAX_VARS; i++) {
    if (strcmp(vars[i].name, name) == 0)
      return &vars[i];
  }
  return NULL;
}

void set_var(const char *name, int value) {
  Var *v = get_var(name);
  if (v) {
    v->value = value;
  } else {
    if (var_count < MAX_VARS) {
      strcpy(vars[var_count].name, name);
      vars[var_count].value = value;
      var_count++;
    } else {
      printf("Error: too many variables\n");
    }
  }
}

int tokenize(char *line, char *tokens[]) {
  int count = 0;
  char *p = line;

  while (*p) {
    // 跳过空格
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (*p == '\0')
      break;
    if (*p == '=' && *(p + 1) == '=') {
      tokens[count] = malloc(3);
      tokens[count][0] = '=';
      tokens[count][1] = '=';
      tokens[count][2] = '\0';
      p += 2;
      count++;
      continue;
    }
    if (*p == '=' || *p == '*' || *p == '/' || *p == '%' || *p == '+' ||
        *p == '-') {
      tokens[count] = malloc(2);
      tokens[count][0] = *p;
      tokens[count][1] = '\0';
      p++;
      count++;
      continue;
    }
    char *start = p;
    if (*p != '"' && *p != '{' && *p != '}' && *p != '=' && *p != '*' &&
        *p != '/' && *p != '%' && *p != '+' && *p != '-') {
      while (*p && !isspace(*p) && *p != '=' && *p != '*' && *p != '/' &&
             *p != '%' && *p != '+' && *p != '-') {
        p++;
      }
      int len = p - start;
      tokens[count] = malloc(len + 1);
      memcpy(tokens[count], start, len);
      tokens[count][len] = '\0';
      count++;
    }
    if (*p == '"') {
      p++;
      start = p;
      while (*p != '"') {
        if (*p == '\0')
          break;
        p++;
      }
      int len = p - start;
      tokens[count] = malloc(len + 1);
      memcpy(tokens[count], start, len);
      tokens[count][len] = '\0';
      count++;
      p++;
      if (*p == '"')

        p++;
    }
    if (*p == '{') {
      p++;
      start = p;
      while (*p != '}') {
        if (*p == '\0')
          break;
        p++;
      }
      int len = p - start;
      tokens[count] = malloc(len + 1);
      memcpy(tokens[count], start, len);
      tokens[count][len] = '\0';
      count++;
      p++;
      if (*p == '}')

        p++;
    }
  }
  return count;
}

int is_number(const char *s) {
  if (*s == '\0')
    return 0;
  while (*s) {
    if (!isdigit(*s))
      return 0;
    s++;
  }
  return 1;
}

int eval_expr(char *tokens[], int count) {
  int result = 0;
  int sign = 1;
  for (int i = 0; i < count; i++) {
    if (strcmp(tokens[i], "+") == 0) {
      sign = ADD;
    } else if (strcmp(tokens[i], "-") == 0) {
      sign = SUB;
    } else if (strcmp(tokens[i], "*") == 0) {
      sign = MUL;
    } else if (strcmp(tokens[i], "/") == 0) {
      sign = DIV;
    } else if (strcmp(tokens[i], "%") == 0) {
      sign = MOD;
    } else {
      int val;
      if (is_number(tokens[i])) {
        val = atoi(tokens[i]);
      } else {
        Var *v = get_var(tokens[i]);
        if (!v) {
          printf("Error: undefined variable '%s'\n", tokens[i]);
          return 0;
        }
        val = v->value;
      }
      if (sign == ADD || sign == SUB) {
        result += sign * val;
      } else if (sign == MUL) {
        result *= val;
      } else if (sign == DIV) {
        if (val == 0) {
          printf("Error: division by zero\n");
          return 0;
        }
        result /= val;
      } else if (sign == MOD) {
        if (val == 0) {
          printf("Error: division by zero\n");
          return 0;
        }
        result %= val;
      }
    }
  }
  return result;
}

void handle_line(char *line) {
  char *tokens[MAX_TOKENS];
  int count = tokenize(line, tokens);
  if (count == 0)
    return;

  if (count == 3 && strcmp(tokens[1], "=") == 0) {
    set_var(tokens[0], atoi(tokens[2]));
    return;
  }

  if (count == 2 && strcmp(tokens[0], "echo") == 0) {
    Var *v = get_var(tokens[1]);
    if (v) {
      printf("%d\n", v->value);
    } else {
      printf("%s\n", tokens[1]);
    }
    return;
  }
  if (count >= 4 && strcmp(tokens[0], "while") == 0 &&
      strcmp(tokens[2], "==") == 0) {
    int left_value = eval_expr(&tokens[1], 1);
    int right_value = eval_expr(&tokens[3], 1);
    while (left_value == right_value) {
      if (count > 4) {
        handle_line(tokens[4]);
      } else {
        printf("false\n");
        return;
      }
    }

    return;
  }

  // IF 语句: if a == 5 命令
  if (count >= 4 && strcmp(tokens[0], "if") == 0 &&
      strcmp(tokens[2], "==") == 0) {
    int left_value = eval_expr(&tokens[1], 1);
    int right_value = eval_expr(&tokens[3], 1);
    if (left_value == right_value) {
      if (count > 4) {
        handle_line(tokens[4]);
        return;
      } else {
        printf("false\n");
        return;
      }
    }
  }
  // 算数运算符
  if (count >= 3 && strcmp(tokens[1], "=") == 0) {
    // printf("count = %d\n", count);
    char *name = tokens[0];
    int val = eval_expr(&tokens[2], count - 2);
    Var *v = get_var(name);
    if (v) {
      v->value = val;
    } else {
      if (var_count < MAX_VARS) {
        strcpy(vars[var_count].name, name);
        vars[var_count].value = val;
        var_count++;
      } else {
        printf("Error: too many variables\n");
      }
    }
    return;
  }
  int result = eval_expr(tokens, count);
  printf("%d\n", result);
}

int main() {
  char line[LINE_LEN];

  printf("mini-shell (type 'exit' to quit)\n");

  while (1) {
    printf("mini-sh> ");
    if (!fgets(line, sizeof(line), stdin))
      break;
    if (strncmp(line, "exit", 4) == 0)
      break;

    handle_line(line);
  }

  printf("Bye\n");
  return 0;
}
