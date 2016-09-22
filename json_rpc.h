#ifndef __JSON_RPC_H__
#define __JSON_RPC_H__

#include "jsmn.h"

#define API_MAX_LINE_LENGTH 4096
#define MAX_METHOD_LENGTH 32
#define MAX_PARAMS_LENGTH ( API_MAX_LINE_LENGTH - MAX_METHOD_LENGTH )

#define COMMAND(name) { #name , ##name }

typedef enum
{
    success = 1,
    success_no_data,
    success_no_response,
    fail_no_method,
    fail,
} return_t;


class Argument
{
public:
    Argument();
    void reset();

    int argc;
    char argv[64][16];

    int pop_int();
    unsigned int pop_uint();
    float pop_float();
    
    char * pop_string();
private:
    int _index;
};

typedef int(*method)(Argument* args, char* ret);

typedef struct
{
    char command_name[MAX_METHOD_LENGTH];
    method function;
} command_t;

int json_rpc_get_value(const char *key, const char* buffer, const jsmntok_t *tokens, const int count, char *result, const int max_length);

int json_rpc_get_method(const char* buffer, const jsmntok_t *tokens, const int count, char *method);

int json_rpc_get_params(const char* buffer, const jsmntok_t *tokens, const int count, char *params);

int json_rpc_get_id(const char* buffer, const jsmntok_t *tokens, const int count, int *id);

return_t json_rpc_handle_command(const char* command, char *resp);

#endif //__JSON_RPC_H__
