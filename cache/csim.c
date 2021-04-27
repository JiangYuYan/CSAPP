#include "cachelab.h"
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
struct Line {
  int valid;
  long tag;
};
int hexcharToInt(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'z') return c - 'a' + 10;
  if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
  return 0;
}
int main(int argc, char *argv[])
{
  int opt, s = 0, E = 0, b = 0;
  bool verbose = false;
  FILE* fileptr;
  while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
    switch (opt) {
      case 'h':
        break;
      case 'v':
        verbose = true;
        break;
      case 's':
        s = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        if ((fileptr = fopen(optarg, "r")) == NULL) {
          printf("No such file.\n");
          exit(1);
        }
        break;
    }
  }
  
  if (s == 0 || E == 0 || b == 0) {
    printf("please input option s, E and b.\n");
    exit(2);
  }
  int S = 1 << s;
  struct Line *cache;
  if ((cache = (struct Line *)malloc((S * E) * sizeof(struct Line))) == NULL)
    printf("Failed to allocate memory for cache.\n");
  for (int i = 0; i < S * E; ++i) {
    cache[i].valid = 0;
  }
  int hit_count = 0, miss_count = 0, eviction_count = 0;
  char line[100];
  char instruction;
  long address;
  while (fgets(line, 100, fileptr)) {
    instruction = line[1];
    if (instruction == ' ') continue;
    address = 0;
    char curr_c = line[3];
    int id = 3;
    for (; curr_c != ','; curr_c = line[++id]) {
      address = address * 16 + hexcharToInt(curr_c);
    }
    for (; curr_c != '\n'; curr_c = line[++id]) {
      // find \n;
    }
    line[id] = '\0';
    printf("%s", line+1);
    address = address >> b;
    long tag = address >> s;
    int set = address ^ (tag << s);
    int tail_id = (set + 1) * E;
    int min_valid = E+1, min_valid_id = set * E, max_valid = 0;
    bool isHit = false;
    int hitId = 0;
    for (int i = set * E; i < tail_id; ++i) {
      int curr_valid = cache[i].valid;
      if (curr_valid < min_valid) {
        min_valid = curr_valid;
        min_valid_id = i;
      }
      if (curr_valid) {
        max_valid = max_valid > curr_valid ? max_valid : curr_valid;
        if (cache[i].tag == tag) {
          ++hit_count;
          if (verbose) printf(" hit");
          isHit = true;
          hitId = i;
        }
      }
    }
    if (!isHit) {
      ++miss_count;
      if (verbose) printf(" miss");
      if (min_valid) {
        ++eviction_count;
        if (verbose) printf(" evicion");
        for (int i = set * E; i < tail_id; ++i) {
          if (i != min_valid_id) {
            --(cache[i].valid);
          }
        }
        cache[min_valid_id].valid = E;
      } else {
        cache[min_valid_id].valid = max_valid + 1;
      }
      cache[min_valid_id].tag = tag;
    } else {
      int hit_valid = cache[hitId].valid;
      for (int i = set * E; i < tail_id; ++i) {
        if (cache[i].valid > hit_valid) {
          --(cache[i].valid);
        }
      }
      cache[hitId].valid = max_valid;
    }
    if (instruction == 'M') {
      ++hit_count;
      if (verbose) printf(" hit");
    }
    printf("\n");
  }

  fclose(fileptr);
  printSummary(hit_count, miss_count, eviction_count);
  return 0;
}
