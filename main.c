#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define LINE_LEN 256
#define MAX_TOKENS 64
#define MAX_VARS 32
#define MAX_ARRAY_SIZE 64
// #define DEBUG
void handle_line(char *line);
int is_number(const char *s);
typedef struct {
  char name[32];
  int is_array;
  int value;
  int array[MAX_ARRAY_SIZE];
  int array_size;
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

void set_array(const char *name, int values[], int size) {
  Var *v = get_var(name);
  if (!v) {
    if (var_count >= MAX_VARS) {
      printf("Error: too many variables\n");
      return;
    }
    v = &vars[var_count];
    strcpy(v->name, name);
    var_count++;
  }
  v->is_array = 1;
  v->array_size = size;
  for (int i = 0; i < size; i++) {
    v->array[i] = values[i];
  }
}

int get_array_element(const char *name, int index) {
  Var *v = get_var(name);
  if (!v || !v->is_array) {
    printf("Error: '%s' is not an array\n", name);
    return 0;
  }
  if (index < 0 || index >= v->array_size) {
    printf("Error: index out of bounds\n");
    return 0;
  }
  return v->array[index];
}
void set_array_element(const char *name, int index, int value) {
  Var *v = get_var(name);
  if (!v || !v->is_array) {
    printf("Error: '%s' is not an array\n", name);
    return;
  }
  if (index < 0 || index >= v->array_size) {
    printf("Error: index out of bounds\n");
    return;
  }
  v->array[index] = value;
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
    if (*p == '|') {
      tokens[count] = malloc(2);
      tokens[count][0] = '|';
      tokens[count][1] = '\0';
      p++;
      count++;
      continue;
    }
    if (*p == '=' && *(p + 1) == '=') {
      tokens[count] = malloc(3);
      tokens[count][0] = '=';
      tokens[count][1] = '=';
      tokens[count][2] = '\0';
      p += 2;
      count++;
      continue;
    }
    if (*p == '!' && *(p + 1) == '=') {
      tokens[count] = malloc(3);
      tokens[count][0] = '!';
      tokens[count][1] = '=';
      tokens[count][2] = '\0';
      p += 2;
      count++;
      continue;
    }
    if (*p == '>' && *(p + 1) == '=') {
      tokens[count] = malloc(3);
      tokens[count][0] = '>';
      tokens[count][1] = '=';
      tokens[count][2] = '\0';
      p += 2;
      count++;
      continue;
    }
    if (*p == '<' && *(p + 1) == '=') {
      tokens[count] = malloc(3);
      tokens[count][0] = '<';
      tokens[count][1] = '=';
      tokens[count][2] = '\0';
      p += 2;
      count++;
      continue;
    }
    if (*p == '>' || *p == '<' || *p == ';' || *p == '=' || *p == '*' ||
        *p == '/' || *p == '%' || *p == '+' || *p == '-') {
      tokens[count] = malloc(2);
      tokens[count][0] = *p;
      tokens[count][1] = '\0';
      p++;
      count++;
      continue;
    }
    char *start = p;
    if (*p != '<' && *p != '>' && *p != '!' && *p != ',' && *p != ',' &&
        *p != '[' && *p != ']' && *p != ';' && *p != '"' && *p != '{' &&
        *p != '}' && *p != '=' && *p != '*' && *p != '/' && *p != '%' &&
        *p != '+' && *p != '-') {
      while (*p && !isspace(*p) && *p != '<' && *p != '>' && *p != '!' &&
             *p != ',' && *p != '[' && *p != ']' && *p != '{' && *p != '}' &&
             *p != ';' && *p != '=' && *p != '*' && *p != '/' && *p != '%' &&
             *p != '+' && *p != '-') {
        p++;
      }
      int len = p - start;
      tokens[count] = malloc(len + 1);
      memcpy(tokens[count], start, len);
      tokens[count][len] = '\0';
      count++;
    }
    if (*p == ',') {
      tokens[count] = malloc(2);
      tokens[count][0] = ',';
      tokens[count][1] = '\0';
      count++;
      p++;
      continue;
    }
    if (*p == '[') {
      tokens[count] = malloc(1);
      memcpy(tokens[count], p, 1);
      count++;
      p++;
      start = p;
      while (*p != ']') {
        if (*p == '\0')
          break;
        p++;
      }

      int len = p - start;
      tokens[count] = malloc(len + 1);
      memcpy(tokens[count], start, len);
      tokens[count][len] = '\0';
      count++;
      if (*p == ']') {

        tokens[count] = malloc(1);
        memcpy(tokens[count], p, 1);
        count++;
        p++;
      }
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
      // 添加 '{' token
      tokens[count] = malloc(2);
      tokens[count][0] = '{';
      tokens[count][1] = '\0';
      count++;
      p++;
      start = p;
      int depth = 1;
      while (*p && depth > 0) {
        if (*p == '{') {
          depth++;
        } else if (*p == '}') {
          depth--;
        }
        if (depth > 0)
          p++;
      }
      if (depth > 0) {
        printf("error: unmatched '{'\n");
        return -1;
      }
      int len = p - start;
      tokens[count] = malloc(len + 1);
      memcpy(tokens[count], start, len);
      tokens[count][len] = '\0';
      count++;
      // 添加 '}' token
      tokens[count] = malloc(2);
      tokens[count][0] = '}';
      tokens[count][1] = '\0';
      count++;
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

void execute(char *tokens[], int count) {
  int has_pipe = 0;
  for (int i = 0; i < count; i++) {
    if (strcmp(tokens[i], "|") == 0) {
      has_pipe = 1;
      break;
    }
  }
  if (count >= 2 && has_pipe == 1) {
    if (strcmp(tokens[0], "|") == 0) {
      printf("syntax error near unexpected token '|'\n");
      return;
    }
    if (strcmp(tokens[count - 1], "|") == 0) {
      printf("syntax error near unexpected token '|'\n");
      return;
    }
    for (int i = 0; i < count; i++) {
      if (strcmp(tokens[i], "|") == 0 && strcmp(tokens[i + 1], "|") == 0) {
        printf("syntax error near unexpected token '|'\n");
        return;
      }
    }
    int pipe_count = 0;
    for (int i = 0; i < count; i++) {
      if (strcmp(tokens[i], "|") == 0) {
        pipe_count++;
      }
    }

    int pipefds[pipe_count][2];
    for (int i = 0; i < pipe_count; i++) {
      if (pipe(pipefds[i]) < 0) {
        perror("pipe");
        exit(1);
      }
    }

    int start = 0;
    int cmd_index = 0;
    int pipe_idx = 0;

    while (start < count) {
      int end = start;
      while (end < count && strcmp(tokens[end], "|") != 0)
        end++;
      int cmd_len = end - start;
      char *cmd[cmd_len + 1];
      for (int i = 0; i < cmd_len; i++) {
        cmd[i] = tokens[start + i];
      }
      cmd[cmd_len] = NULL;
      pid_t pid = fork();
      if (pid == 0) {
        if (cmd_index > 0) {
          dup2(pipefds[pipe_idx - 1][0], STDIN_FILENO); // read
        }
        if (pipe_idx < pipe_count) {
          dup2(pipefds[pipe_idx][1], STDOUT_FILENO); // write
        }
        for (int i = 0; i < pipe_count; i++) {
          close(pipefds[i][0]);
          close(pipefds[i][1]);
        }
        execvp(cmd[0], cmd);
        perror("execvp");
        exit(1);
      }
      if (cmd_index > 0) {
        close(pipefds[pipe_idx - 1][0]);
      }
      if (pipe_idx < pipe_count) {
        close(pipefds[pipe_idx][1]);
      }

      start = end + 1;
      cmd_index++;
      pipe_idx++;
    }
    while (wait(NULL) > 0)
      ;
    return;
  }

  if (count >= 6 && strcmp(tokens[1], "=") == 0 &&
      strcmp(tokens[3], "[") == 0 && strcmp(tokens[5], "]") == 0) {
    int index;
    if (is_number(tokens[4])) {
      index = atoi(tokens[4]);
    } else {
      Var *idx_var = get_var(tokens[4]);
      if (!idx_var) {
        printf("Error: undefined variable '%s'\n", tokens[4]);
        return;
      }
      index = idx_var->value;
    }
    int val = get_array_element(tokens[2], index);
    set_var(tokens[0], val);
    return;
  }

  if (count == 3 && strcmp(tokens[1], "=") == 0) {
    int value;
    if (is_number(tokens[2])) {
      value = atoi(tokens[2]);
    } else {
      Var *idx_var = get_var(tokens[2]);
      if (!idx_var) {
        printf("Error: undefined variable '%s'\n", tokens[2]);
        return;
      }
      value = idx_var->value;
    }
    set_var(tokens[0], value);
    return;
  }

  if (count >= 6 && strcmp(tokens[1], "[") == 0 &&
      strcmp(tokens[3], "]") == 0 && strcmp(tokens[4], "=") == 0 &&
      strcmp(tokens[5], "{") != 0) {
    int index;
    if (is_number(tokens[2])) {
      index = atoi(tokens[2]);
    } else {
      Var *idx_var = get_var(tokens[2]);
      if (!idx_var) {
        printf("Error: undefined variable '%s'\n", tokens[2]);
        return;
      }
      index = idx_var->value;
    }
    int value;
    if (is_number(tokens[5])) {
      value = atoi(tokens[5]);
    } else {
      Var *val_var = get_var(tokens[5]);
      if (!val_var) {
        printf("Error: undefined variable '%s'\n", tokens[5]);
        return;
      }
      value = val_var->value;
    }
    set_array_element(tokens[0], index, value);
    return;
  }

  if (count >= 8 && strcmp(tokens[1], "[") == 0 &&
      strcmp(tokens[3], "]") == 0 && strcmp(tokens[4], "=") == 0 &&
      strcmp(tokens[5], "{") == 0) {
    char *values_tokens[MAX_ARRAY_SIZE];
    int value_count = tokenize(tokens[6], values_tokens);
#ifdef DEBUG
    // 调试输出
    printf("count = %d\n", value_count);
    for (int i = 0; i < value_count; i++) {
      printf("token[%d] = '%s'\n", i, values_tokens[i]);
    }
#endif

    int values[MAX_ARRAY_SIZE] = {0};
    int size = 0;
    int array_size = atoi(tokens[2]);
    if (!is_number(tokens[2])) {
      Var *v = get_var(tokens[2]);
      if (!v) {
        printf("Error: undefined variable '%s'\n", tokens[2]);
        return;
      }
      array_size = v->value;
    }
    for (int i = 0; i < value_count; i++) {
      if (strcmp(values_tokens[i], ",") != 0) {
        values[size++] = atoi(values_tokens[i]);
      }
    }
    if (array_size < size) {
      printf("Error: too many initializers\n");
      return;
    }

    if (atoi(tokens[2]) > size) {
      size = atoi(tokens[2]);
    }
    set_array(tokens[0], values, size);
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

  if (count >= 5 && strcmp(tokens[0], "echo") == 0 &&
      strcmp(tokens[2], "[") == 0 && strcmp(tokens[4], "]") == 0) {
    // 获取索引（支持变量）
    int index;
    if (is_number(tokens[3])) {
      index = atoi(tokens[3]);
    } else {
      Var *idx_var = get_var(tokens[3]);
      if (!idx_var) {
        printf("Error: undefined variable '%s'\n", tokens[3]);
        return;
      }
      index = idx_var->value;
    }
    int val = get_array_element(tokens[1], index);
    Var *v = get_var(tokens[1]);
    if (index >= 0 && index < v->array_size) {
      printf("%d\n", val);
      return;
    }
  }

  if (count >= 4 && strcmp(tokens[0], "while") == 0 &&
      (strcmp(tokens[2], "==") == 0 || strcmp(tokens[2], "!=") == 0 ||
       strcmp(tokens[2], ">") == 0 || strcmp(tokens[2], "<") == 0 ||
       strcmp(tokens[2], ">=") == 0 || strcmp(tokens[2], "<=") == 0)) {

    if (strcmp(tokens[2], "!=") == 0) {
      int left_value = eval_expr(&tokens[1], 1);
      int right_value = eval_expr(&tokens[3], 1);
      while (left_value != right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          left_value = eval_expr(&tokens[1], 1);
          right_value = eval_expr(&tokens[3], 1);
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], "==") == 0) {
      int left_value = eval_expr(&tokens[1], 1);
      int right_value = eval_expr(&tokens[3], 1);
      while (left_value == right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          left_value = eval_expr(&tokens[1], 1);
          right_value = eval_expr(&tokens[3], 1);
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], ">") == 0) {
      int left_value = eval_expr(&tokens[1], 1);
      int right_value = eval_expr(&tokens[3], 1);
      while (left_value > right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          left_value = eval_expr(&tokens[1], 1);
          right_value = eval_expr(&tokens[3], 1);
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], "<") == 0) {
      int left_value = eval_expr(&tokens[1], 1);
      int right_value = eval_expr(&tokens[3], 1);
      while (left_value < right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          left_value = eval_expr(&tokens[1], 1);
          right_value = eval_expr(&tokens[3], 1);
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], ">=") == 0) {
      int left_value = eval_expr(&tokens[1], 1);
      int right_value = eval_expr(&tokens[3], 1);
      while (left_value >= right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          left_value = eval_expr(&tokens[1], 1);
          right_value = eval_expr(&tokens[3], 1);
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], "<=") == 0) {
      int left_value = eval_expr(&tokens[1], 1);
      int right_value = eval_expr(&tokens[3], 1);
      while (left_value <= right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          left_value = eval_expr(&tokens[1], 1);
          right_value = eval_expr(&tokens[3], 1);
        } else {
          printf("false\n");
          return;
        }
      }
    }
    return;
  }

  // IF 语句: if a == 5 命令
  if (count >= 4 && strcmp(tokens[0], "if") == 0 &&
      (strcmp(tokens[2], "==") == 0 || strcmp(tokens[2], "!=") == 0 ||
       strcmp(tokens[2], ">") == 0 || strcmp(tokens[2], "<") == 0 ||
       strcmp(tokens[2], ">=") == 0 || strcmp(tokens[2], "<=") == 0)) {

    int left_value = eval_expr(&tokens[1], 1);
    int right_value = eval_expr(&tokens[3], 1);
    if (strcmp(tokens[2], "!=") == 0) {
      if (left_value != right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          return;
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], "==") == 0) {
      if (left_value == right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          return;
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], ">") == 0) {
      if (left_value > right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          return;
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], "<") == 0) {
      if (left_value < right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          return;
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], ">=") == 0) {
      if (left_value >= right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          return;
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], "<=") == 0) {
      if (left_value <= right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          return;
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], ">") == 0) {
      if (left_value > right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          return;
        } else {
          printf("false\n");
          return;
        }
      }
    } else if (strcmp(tokens[2], "<") == 0) {
      if (left_value < right_value) {
        if (count > 5) {
          handle_line(tokens[5]);
          return;
        } else {
          printf("false\n");
          return;
        }
      }
    }
  }
  // 算数运算符
  if (count >= 3 && strcmp(tokens[1], "=") == 0) {
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
}

void handle_line(char *line) {
  char *tokens[MAX_TOKENS];
  int count = tokenize(line, tokens);

#ifdef DEBUG
  // 调试输出
  printf("count = %d\n", count);
  for (int i = 0; i < count; i++) {
    printf("token[%d] = '%s'\n", i, tokens[i]);
  }
#endif

  if (count == 0)
    return;

  int start = 0;

  int has_pipe = 0;
  for (int i = 0; i < count; i++) {
    if (strcmp(tokens[i], "|") == 0) {
      has_pipe = 1;
      break;
    }
  }
  if (has_pipe == 1) {
    // Handle if statements
    execute(tokens, count);
    return;
  }

  if (strcmp(tokens[0], "if") == 0 || strcmp(tokens[0], "while") == 0) {
    // 找到 if/while 语句结束的位置 (在 } 和 ; 之后)
    int end = 0;
    for (int i = 0; i < count; i++) {
      if (strcmp(tokens[i], "}") == 0) {
        end = i + 1;
        break;
      }
    }
    // 执行 if/while 语句
    execute(tokens, end);
    // 如果后面还有语句，继续处理
    if (end < count && strcmp(tokens[end], ";") == 0) {
      end++;
      if (end < count) {
        // 处理剩余的语句
        for (int i = end; i < count; i++) {
          if (strcmp(tokens[i], ";") == 0) {
            execute(&tokens[end], i - end);
            end = i + 1;
          }
        }
      }
    }
    return;
  }

  for (int i = 0; i < count; i++) {
    if (i == count - 1 && (strcmp(tokens[i], ";") != 0)) {
      printf("error: missing ';' at the end\n");
      return;
    } else if ((strcmp(tokens[i], ";")) == 0) {
      execute(&tokens[start], i - start);
      start = i + 1;
    }
  }
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