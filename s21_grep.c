#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 256

typedef struct {
  bool e, i, v, c, l, n, h, s, f, o;
} Flags;

typedef struct {
  bool multifile, error;
  int patterns_count, files_count;
} Params;


void check_flags(int argc, char *argv[], Flags *flags, Params *params,
                 char **pattern);
void process_file_pattern(Params *params, char **pattern, char *optarg);
void add_pattern(char **pattern, char *line);
char *escape_regex(char *src);
void replace_linebreak(char **line);
void process_file(Flags flags, Params *params, const char *filename,
                  char *pattern);
void regex_compile(Flags flags, Params *params, regex_t *regex, char *pattern);
void process_flag_o(Flags flags, Params *params, FILE *file, regex_t *regex,
                    const char *filename);
void process_file_lines(Flags flags, Params *params, FILE *file, regex_t *regex,
                        const char *filename);
int main(int argc, char *argv[]) {
  char *pattern = (char *)calloc(SIZE, sizeof(char));
  Flags flags = {false};
  Params params = {false, false, 0, 0};
  if (argc < 2) {
    fprintf(stderr, "Missing arguments\n");
    params.error = true;
  } else {
    check_flags(argc, argv, &flags, &params, &pattern);
  }
  
  if (!params.error) {
    
    if (!flags.f && !flags.e) add_pattern(&pattern, argv[optind++]);
    
    if (argc - optind > 1) params.multifile = true;
    
    for (int i = optind; i < argc; i++) {
      process_file(flags, &params, argv[i], pattern);
    }

  } else fprintf(stderr, "Usage: ./s21_grep [flag] [pattern] [file]\n");

  free(pattern);

  return params.error;
}

void check_flags(int argc, char *argv[], Flags *flags, Params *params,
                 char **pattern) {
  int option;

  while ((option = getopt(argc, argv, "e:ivclnhsf:o")) != -1 &&
         !(params->error)) {
    switch (option) {
    case 'e':
      flags->e = true;
      add_pattern(pattern, optarg);
      break;
    case 'i':
      flags->i = true;
      break;
    case 'v':
      flags->v = true;
      break;
    case 'c':
      flags->c = true;
      break;
    case 'l':
      flags->l = true;
      break;
    case 'n':
      flags->n = true;
      break;
    case 'h':
      flags->h = true;
      break;
    case 's':
      flags->s = true;
      break;
    case 'f':
      flags->f = true;
      params->files_count++;
      process_file_pattern(params, pattern, optarg);
      break;
    case 'o':
      flags->o = true;
      break;
    default:
      params->error = true;
      break;
    }
  }
  
  if (flags->l) flags->c = false, flags->n = false, flags->o = false;

  if (flags->c) flags->n = false, flags->o = false;

  if (optind >= argc || argv[optind][0] == '-' || strlen(argv[optind]) == 0) {
    params->error = true;
  }

}

void process_file_pattern(Params *params, char **pattern, char *optarg) {
 
  FILE *file = fopen(optarg, "r");

  if (file != NULL) {
    char *line = NULL;
    size_t length = 0;
    while ((getline(&line, &length, file)) != -1) {
      add_pattern(pattern, line);
    }
    
    free(line);
    
    fclose(file);
  } else {
    params->error = true;
    fprintf(stderr, "s21_grep: %s: No such file or directory\n", optarg);
  }
}

char *escape_regex(char *src) {

  char *dest = malloc(strlen(src) * 2 + 1);
  char *d = dest;
  
  while (*src) {
    if (strchr(".^$*+?()[]{}|\\", *src)) {
      *d++ = '\\';
    }
    *d++ = *src++;
  }
  *d = '\0';
  return dest;
}

void add_pattern(char **pattern, char *line) {
  line[strcspn(line, "\n")] = '\0'; 
  
  if (strlen(line) == 0)
    return;
  char *escaped = escape_regex(line);
  size_t needed = strlen(*pattern) + strlen(escaped) + 10;
  static size_t buffer_size = SIZE;
  
  if (needed > buffer_size) {
    while (buffer_size < needed)
      buffer_size *= 2;
    *pattern = realloc(*pattern, buffer_size);
  }
  
  if ((*pattern)[0] != '\0') {
    strcat(*pattern, "\\|");
  }
  strcat(*pattern, "\\(");
  strcat(*pattern, line);
  strcat(*pattern, "\\)");
 
  free(escaped);
}

void replace_linebreak(char **line) {
  int len = strlen(*line);
  if (len > 0 && (*line)[len - 1] == '\n') {
    (*line)[len - 1] = '\0';
  }
}

void process_file(Flags flags, Params *params, const char *filename,
                  char *pattern) {
  FILE *file = fopen(filename, "r");
  
  if (file != NULL) {
    regex_t regex;
  
    regex_compile(flags, params, &regex, pattern);
    
    if (!params->error && !(flags.o && flags.v)) {
      
      if (flags.o) {
        process_flag_o(flags, params, file, &regex, filename);
      } else {
        process_file_lines(flags, params, file, &regex, filename);
      }
    }
    
    regfree(&regex);

    fclose(file);
  
  } else if (!flags.s) {
    fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
  }
}

void regex_compile(Flags flags, Params *params, regex_t *regex, char *pattern) {
  int cflags = flags.i ? REG_ICASE : 0;
  cflags |= REG_NOSUB;

  if (regcomp(regex, pattern, cflags)) {
    params->error = true;
    fprintf(stderr, "Error compiling regex: %s\n", pattern);
  }
}

void process_flag_o(Flags flags, Params *params, FILE *file, regex_t *regex,
                    const char *filename) {
  regmatch_t match;
  char *line = NULL;
  size_t length = 0;
  int line_count = 0;
  
  while ((getline(&line, &length, file)) != -1) {
    line_count++;
    replace_linebreak(&line);
    char *copy_line = strdup(line);
    char *original = copy_line;
    int offset = 0;
    
    while (regexec(regex, copy_line, 1, &match, 0) == 0) {
      if (match.rm_so == match.rm_eo) {
        copy_line++;
        continue;
      }

      if (params->multifile && !flags.h) printf("%s:", filename);
      
      if (flags.n) printf("%d:", line_count);
      
      printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
             original + offset + match.rm_so);
      offset += match.rm_eo;
      copy_line = original + offset;
    }
    
    free(original);
  }
  
  free(line);
}

void process_file_lines(Flags flags, Params *params, FILE *file, regex_t *regex,
                        const char *filename) {
  char *line = NULL;
  size_t length = 0;
  int line_count = 0, match_count = 0;
  
  while ((getline(&line, &length, file)) != -1) {
    line_count++;
    replace_linebreak(&line);
    
    int status = regexec(regex, line, 0, NULL, 0);
    
    if ((!status) ^ flags.v) {
      if (flags.l) {
        printf("%s\n", filename);
        break;
      }
      match_count++;
      if (!flags.c) {
        if (params->multifile && !flags.h)
          printf("%s:", filename);
        if (flags.n)
          printf("%d:", line_count);
        printf("%s\n", line);
      }
    }
  }
  if (flags.c) {
    if (params->multifile && !flags.h)
      printf("%s:", filename);
    printf("%d\n", match_count);
  }
  
  free(line);
}
