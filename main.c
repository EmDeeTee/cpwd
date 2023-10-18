#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "sha256.h"
#include "service.h"

// TODO: We will never have more than one Service loaded
Service services[128] = {0};
size_t services_count = 0;

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

void parse_cmd_args(int count, char** args) {
  if (count < 2) {
    // TODO: show usage
    exit_with_msg("Not enough arguments");
  }

  char* cmd = args[1];
  //cpwd store youtube 123
  if (strcmp(cmd, "store") == 0) {
    if (count < 4) {
      exit_with_msg("store needs 2 arguments\nservice name and password to hash");
    }
    char* name = args[2];
    unsigned char* pass_raw = (unsigned char*)args[3];
    
    services[services_count++] = (Service)
      {
	.name = strdup(name),
	.hash = hash_from_str(pass_raw)
      };
  }
  //cpwd get youtube
  else if (strcmp(cmd, "int")) {
    assert(0 && "Not implement");
  }
}

int main(int argc, char** argv) {
  parse_cmd_args(argc, argv);
  printf("Service name = %s\nService hash = %s\n", services[0].name, services[0].hash);
}
