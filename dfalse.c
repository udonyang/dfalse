#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

typedef enum token_e {
  LCOMMENT = '{',
  RCOMMENT = '}',
  LCODE = '[',
  RCODE = ']',
  VARADR = 256,
  VALUE = 257,
  ASSIGN = ':',
  RVAL = ';',
  APPLY = '!',
  PLUS = '+',
  MINUS = '-',
  MULTIPLE = '*',
  DIVIDE = '/',
  NEGATE = '_',
  ISEQUAL = '=',
  GREATER = '>',
  AND = '&',
  OR = '|',
  NOT = '~',
  DUPLICATE = '$',
  DELETE = '%',
  SWAP = '\\',
  ROT = '@',
  IF = '?',
  WHILE = '#',
  TOINT = '.',
  QUOTE = '"',
  TOCHAR = ',',
  GETC = '^',
  __BOUND__ = 300
} token_e;

typedef struct token_t {
  token_e type;
  const char* data;
  size_t size;

  const char* head;
  int line;
} token_t;

static const char* sys_msg()
{
  return strerror(errno);
}

static void err_msg(const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

static char* loadfile(const char* filename)
{
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    err_msg(sys_msg());
    goto err_0;
  }

  if (fseek(file, 0, SEEK_END) == -1) {
    err_msg(sys_msg());
    goto err_1;
  }
  long size = ftell(file);
  rewind(file);

  char* foo = calloc(size+2, 1);
  if (foo == NULL) {
    err_msg(sys_msg());
    goto err_1;
  }

  if (fread(foo, 1, size, file) != size) {
    err_msg(sys_msg());
    goto err_1;
  }

  fclose(file);
  return foo;
err_1:
  fclose(file);
err_0:
  return NULL;
}

static void set_token(token_t* token, const token_e type, const char* data, const size_t size,
               const char*head, const int line)
{
  token->type = type;
  token->data = data;
  token->size = size;
  token->head = head;
  token->line = line;
}

static void token_err(token_t* token, const char* msg)
{
  char prefix[BUFSIZ];
  sprintf(prefix, "%d:%d:", token->line, token->data-token->head+1);
  err_msg("%s %s from", prefix, msg);

  char foo[BUFSIZ];
  int len = token->data-token->head;
  snprintf(foo, len+token->size+1, "%s", token->head);
  err_msg("%s %s", prefix, foo);

  for (int i = 0; i < len; i++)
    foo[i] = ' ';
  foo[len] = '^';
  foo[len+1] = '\0';
  err_msg("%s %s", prefix, foo);
}

static int lexer(char* foo, token_t** tokens, size_t* size)
{
  token_t* bud = calloc(strlen(foo)+1, sizeof(token_t));
  if (bud == NULL) {
    err_msg(sys_msg());
    goto err_0;
  }

  char* head = foo;
  int line = 1;
  size_t size_ = 0;
  while (*foo)
    if (isspace(*foo)) {
      while (*foo && isspace(*foo)) {
        if (*foo == '\n') {
          head = foo+1;
          line++;
        }
        foo++;
      }
    }
    else if (islower(*foo)) {
      set_token(bud+size_++, VARADR, foo, 1, head, line);
      foo++;
    }
    else if (isdigit(*foo)) {
      char* start = foo;
      while (*foo && isdigit(*foo))
        foo++;
      set_token(bud+size_++, VALUE, start, foo-start, head, line);
    }
    else
      set_token(bud+size_++, *foo, foo++, 1, head, line);

  set_token(bud+size_, __BOUND__, foo, 0, head, line);
  *tokens = bud;
  *size = size_;
  return 0;
err_1:
  free(bud);
err_0:
  return -1;
}

typedef int action_f(token_t* first, token_t* last);

static token_t* parse_co(token_t* first, token_t* last,
                      int (*isok)(token_t*),
                      token_t* (*action)(token_t*, token_t*))
{
  token_t* it = first;
  while (it < last && isok(it))
    it++;
  return action(first, it);
}

static int pass(token_t* token)
{
  return 1;
}

static token_t* do_nothing(token_t* first, token_t* last)
{
  return last;
}

static int is_not_rcomment(token_t* token)
{
  return token->type != RCOMMENT;
}

static token_t* do_comment(token_t* first, token_t* last)
{
  if (last->type != RCOMMENT) {
    token_err(first-1, "missing match }");
    goto err_0;
  }
  return last+1;
err_0:
  return NULL;
}

static int is_not_rcode(token_t* token)
{
  return token->type != RCODE;
}

static token_t* do_code(token_t* first, token_t* last)
{
  if (last->type != RCODE) {
    token_err(first-1, "missing match ]");
    goto err_0;
  }
  // TODO
  return last+1;
err_0:
  return NULL;
}

static int parse(token_t* first, token_t* last)
{
  while (first < last) {
    switch (first->type) {
      case LCOMMENT: {
        first = parse_co(first+1, last, is_not_rcomment, do_comment);
        break;
      }
      case RCOMMENT: {
        token_err(first, "missing match {");
        first = NULL;
        break;
      }
      case LCODE: {
        first = parse_co(first+1, last, is_not_rcode, do_code);
        break;
      }
      case RCODE: {
        token_err(first, "missing match [");
        first = NULL;
        break;
      }
      case VARADR: {
        break;
      }
      case VALUE: {
        break;
      }
      case ASSIGN: {
        break;
      }
      case RVAL: {
        break;
      }
      case APPLY: {
        break;
      }
      case PLUS: {
        break;
      }
      case MINUS: {
        break;
      }
      case MULTIPLE: {
        break;
      }
      case DIVIDE: {
        break;
      }
      case NEGATE: {
        break;
      }
      case ISEQUAL: {
        break;
      }
      case GREATER: {
        break;
      }
      case AND: {
        break;
      }
      case OR: {
        break;
      }
      case NOT: {
        break;
      }
      case DUPLICATE: {
        break;
      }
      case DELETE: {
        break;
      }
      case SWAP: {
        break;
      }
      case ROT: {
        break;
      }
      case IF: {
        break;
      }
      case WHILE: {
        break;
      }
      case TOINT: {
        break;
      }
      case QUOTE: {
        break;
      }
      case TOCHAR: {
        break;
      }
      case GETC: {
        break;
      }
      default: {
        token_err(first, "unknown token");
        first = NULL;
      }
    }

    if (first == NULL)
      goto err_0;
  }
  return 0;
err_0:
  return -1;
}

int main(int argc, char* argv[])
{
  char* foo = loadfile(argv[1]);
  if (foo == NULL) {
    err_msg("load file failed");
    goto err_0;
  }

  token_t* tokens;
  size_t size;
  if (lexer(foo, &tokens, &size) != 0) {
    err_msg("lexer failed");
    goto err_1;
  }

  if (parse(tokens, tokens+size) != 0) {
    err_msg("parse failed");
    goto err_2;
  }

  free(tokens);
  free(foo);
  return 0;
err_2:
  free(tokens);
err_1:
  free(foo);
err_0:
  return -1;
}
