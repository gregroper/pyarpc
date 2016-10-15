#include "json_rpc.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_TOKENS 16

extern command_t api_command_table[];

extern int api_num_commands;

static jsmn_parser p;

static jsmntok_t tokens[MAX_TOKENS];

int json_buffer_index = 0;


Argument::Argument() : _index(0) {}
int Argument::pop_int() { assert(_index < argc); return atoi(argv[_index++]); }

unsigned int Argument::pop_uint() { assert(_index < argc); return strtoul(argv[_index++], NULL, 0); }

float Argument::pop_float() { assert(_index < argc); return (float)atof(argv[_index++]); }

char * Argument::pop_string() { assert(_index < argc); return argv[_index++]; }

void Argument::reset() { _index = 0; }

int json_rpc_get_value(const char *key, const char* buffer, const jsmntok_t *tokens, const int count, char *result, const int max_length)
{
    for (int i = 0; i < count; i++)
    {
        if (tokens[i].type == JSMN_STRING)
        {
            if (0 == strncmp(key, &buffer[tokens[i].start], tokens[i].end - tokens[i].start))
            {
                if (result != NULL)
                {
                    memset(result, 0, max_length);
                    strncpy(result, &buffer[tokens[i + 1].start], tokens[i + 1].end - tokens[i + 1].start);
                }
                return i+1;
            }
        }
    }
    return -1;
}


int json_rpc_get_method(const char* buffer, const jsmntok_t *tokens, const int count, char *method)
{
    return json_rpc_get_value("method", buffer, tokens, count, method, MAX_METHOD_LENGTH);
}


int json_rpc_get_params(const char* buffer, const jsmntok_t *tokens, const int count, char *params)
{
    return json_rpc_get_value("params", buffer, tokens, count, params, MAX_PARAMS_LENGTH);
}


int json_rpc_get_id(const char* buffer, const jsmntok_t *tokens, const int count, int *id)
{
    char cid[8];
    if (json_rpc_get_value("id", buffer, tokens, count, cid, 8))
    {
        *id = atoi(cid);
        return 1;
    }
    return 0;
}


return_t json_rpc_handle_command(const char* command, char *resp)
{
    static Argument args;
    
    char method[MAX_METHOD_LENGTH];
    int id;
    
    args.reset();
    jsmn_init(&p);
    int token_count = jsmn_parse(&p, command, strlen(command), tokens, MAX_TOKENS);

    json_rpc_get_method(command, tokens, token_count, method);
    int params_idx = json_rpc_get_params(command, tokens, token_count, NULL);
    json_rpc_get_id(command, tokens, token_count, &id);

    for (int i = 0; i < api_num_commands; i++)
    {
        if (0 == strcmp(api_command_table[i].command_name, method))
        {
            args.argc = 0;
            //Parse the params:
            if (params_idx!= -1 && tokens[params_idx].type == JSMN_ARRAY )
            {
                args.argc = tokens[params_idx].size;
                for (int j = params_idx + 1, k = 0; j < params_idx + args.argc + 1; j++, k++)
                {
                    unsigned len = tokens[j].end - tokens[j].start;

                    strncpy(&args.argv[k][0], &command[tokens[j].start], len);
                    args.argv[k][len] = '\0';
                }
            }

            return (return_t)api_command_table[i].function(&args, resp);
        }
    }
    return fail_no_method;
}
