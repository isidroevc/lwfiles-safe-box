#include <sqlite3.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <uuid/uuid.h>
struct Storage {
  char* filename;
  sqlite3 *database;
};

char* generate_uuid_string();
struct Storage* storage_create(char* filename);
struct Storage* storage_create(char* filename) {
  struct Storage* new_storage =  calloc(sizeof(struct Storage), 1);
  new_storage->filename = filename;
  int error = sqlite3_open(filename, &new_storage->database);
  if (error) {
    return NULL;
  }
  error = initialize_storage(new_storage);
  if (error) {
    return NULL;
  }
  return new_storage;
}
int initialize_storage(struct Storage *storage);
int initialize_storage(struct Storage *storage) {
  char* error = 0;
  char* statement = "CREATE TABLE IF NOT EXISTS \"objects\" (\"id\" TEXT, \"name\" TEXT, \"content\" BLOB)";
  int result = sqlite3_exec(storage->database, statement, NULL, NULL, &error);
  if (result != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n",error);
    return result;
  }
  return 0;
}
int storage_create_object(struct Storage* storage, char* name, size_t name_size, char*filename, char* created_object_id);
int storage_create_object(struct Storage* storage, char* name, size_t name_size, char*filename, char** created_object_id) {
  FILE* file = fopen((const char*)filename, "r");
  if (file == NULL) {
    return 1;
  }
  size_t file_size;
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char file_content[file_size];
  
  size_t readed_objects = fread(file_content, sizeof(file_content),  1, file);
  if (readed_objects != 1) {
    return 1;
  }
  char* statement_string = "INSERT INTO \"objects\" (\"id\", \"name\", \"content\") VALUES(?,?,?)";
  sqlite3_stmt* preapred_statement;
  int result = sqlite3_prepare_v2(storage->database, statement_string, -1, &preapred_statement, NULL);
  if (result != SQLITE_OK) {
    return result;
  }
  created_object_id = generate_uuid_string();
  fprintf(stderr, "%s", created_object_id);
  result = sqlite3_bind_text(preapred_statement, 1, created_object_id, 36, NULL);
  if (result != SQLITE_OK) {
    return result;
  }
  result = sqlite3_bind_text(preapred_statement, 2, name, name_size, NULL);
  if (result != SQLITE_OK) {
    return result;
  }
  result = sqlite3_bind_blob(preapred_statement, 3, file_content, file_size, NULL);
  if (result != SQLITE_OK) {
    return result;
  }

  result = sqlite3_step(preapred_statement);
  if (result != SQLITE_DONE) {
    return result;
  }
  sqlite3_finalize(preapred_statement);
  fclose(file);
  return result;
}
void storage_destroy(struct Storage* storage);
void storage_destroy(struct Storage* storage) {
  if (storage->database != NULL) {
    sqlite3_close(storage->database);
  }
  free(storage);
}

char* generate_uuid_string() {
  uuid_t binary_uuid;
  uuid_generate(binary_uuid);
  char* uuid = malloc(37);
  uuid_unparse_upper(binary_uuid, uuid);
  return uuid;
}

