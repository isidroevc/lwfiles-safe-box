#include <stdio.h>
#include <sqlite3.h> 
#include "lib/storage.c"
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
struct Storage* test_create_storage(char* filename);
int main(int argc, char* argv[]) {
  char* object_name = "new_object.jpg";
  char* id;
  int rc;
  struct Storage *storage = test_create_storage("database.db");
  rc = storage_create_object(storage, object_name ,strlen(object_name) , "./main.c", id);
  test_storage_destroy(storage);
  fprintf(stderr, "%s", id);
}


struct Storage* test_create_storage(char* filename) {
  struct Storage* storage = storage_create(filename);
  return storage;
}
void test_storage_destroy(struct Storage* storage);
void test_storage_destroy(struct Storage* storage) {
  storage_destroy(storage);
}





