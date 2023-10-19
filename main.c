#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>

#include "sha256.h"

#define DB_PATH "cpwd.db"

void exit_with_msg(const char* msg) {
  printf("%s\n", msg);
  exit(1);
}

char* hash_from_str(const unsigned char* str) {
  unsigned char hash[128];
    
  SHA256_CTX ctx = {0};
  sha256_init(&ctx);
  sha256_update(&ctx, str, strlen((char*)str));
  sha256_final(&ctx, hash);

  char hash_str[2 * 32 + 1];
  for (int i = 0; i < 32; i++) {
    sprintf(&hash_str[i*2], "%02x", hash[i]);
  }

  return strdup(hash_str);
}

void print_usage(void) {
  printf("cpwd [command]\n");
  printf("commands:\n");
  printf("\t store [name] [password]\n");
  printf("\t get [name]\n");
}

void store_service(const char* name, const char* hash_str) {
  FILE* fp = fopen(DB_PATH, "a");

  fprintf(fp, "%s:%s\n", name, hash_str);
  fflush(fp);
  fclose(fp);
}

char*  read_service(const char* target_name) {
  FILE* fp = fopen(DB_PATH, "r");
  if (fp == NULL) {
    exit_with_msg("[ERROR] Can't read file at DB_PATH");
  }

  char name[256];
  char hash[256];

  while (fscanf(fp, "%255[^:]:%s\n", name, hash) == 2) {
    if (strcmp(name, target_name) == 0) {
      printf("%s\n", hash);
      fflush(fp);
      fclose(fp);
      return strdup(hash);
    }
  }
  fflush(fp);
  fclose(fp);

  printf("No match found for %s\n", target_name);
  exit(1);
}

void resolve_master_password(void) {
  if (access(DB_PATH, F_OK) == 0) {
    char provided_pass[256];
    char* master_pass = read_service("MASTER");
    printf("Master password pls: ");
    scanf("%s", provided_pass);
    char* provided_hash = hash_from_str((unsigned char*)provided_pass);
    if (strcmp(master_pass, provided_hash) == 0) {
      return;
    } else {
      exit_with_msg("Wrong master password");
    }
  }
  else {
    char user_pass[256];
    printf("Create master password: ");
    scanf("%s", user_pass);
    store_service("MASTER", hash_from_str((unsigned char*)user_pass));
    exit_with_msg("Password created. Re-run cpwd");
  }
}

void parse_cmd_args(int count, char** args) {
  if (count < 2) {
    printf("Not enough arguments\n");
    print_usage();
    exit(1);
  }

  char* cmd = args[1];
  //cpwd store youtube 123
  if (strcmp(cmd, "store") == 0) {
    if (count < 4) {
      exit_with_msg("store needs 2 arguments\nservice name and password to hash");
    }
    char* name = args[2];
    char* pass_hash = hash_from_str((unsigned char*)args[3]);
    
    store_service(name, pass_hash);
  }
  //cpwd get youtube
  else if (strcmp(cmd, "get") == 0) {
    if (count < 3) {
      exit_with_msg("get needs 1 argument\nservice name");
    }
    char* name = args[2];
    read_service(name);
  }
}

int main(int argc, char** argv) {
  resolve_master_password();
  parse_cmd_args(argc, argv);
}
