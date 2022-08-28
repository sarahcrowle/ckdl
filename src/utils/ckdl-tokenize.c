#include <kdl/tokenizer.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

static size_t read_func(void *user_data, char *buf, size_t bufsize)
{
    FILE *fp = (FILE*) user_data;
    return fread(buf, 1, bufsize, fp);
}

int main(int argc, char **argv)
{
    FILE *in = NULL;
    if (argc == 1) {
        in = stdin;
    } else if (argc == 2) {
        char const *fn = argv[1];
        if (strcmp(fn, "-") == 0) {
            in = stdin;
        } else {
            in = fopen(fn, "r");
            if (in == NULL) {
                fprintf(stderr, "Error opening file \"%s\": %s\n",
                    fn, strerror(errno));
                return 1;
            }
        }
    } else {
        fprintf(stderr, "Error: Too many arguments\n");
        return 2;
    }

    kdl_tokenizer *tokenizer = kdl_create_stream_tokenizer(&read_func, (void*)in);

    bool have_error = false;
    while (1) {
        kdl_token token;
        kdl_tokenizer_status status = kdl_pop_token(tokenizer, &token);
        if (status == KDL_TOKENIZER_ERROR) {
            fprintf(stderr, "Tokenization error\n");
            have_error = true;
            break;
        } else if (status == KDL_TOKENIZER_EOF) {
            break;
        }

        char const *token_type_name = NULL;
        switch (token.type) {
        case KDL_TOKEN_START_TYPE:
            token_type_name = "KDL_TOKEN_START_TYPE";
            break;
        case KDL_TOKEN_END_TYPE:
            token_type_name = "KDL_TOKEN_END_TYPE";
            break;
        case KDL_TOKEN_WORD:
            token_type_name = "KDL_TOKEN_WORD";
            break;
        case KDL_TOKEN_STRING:
            token_type_name = "KDL_TOKEN_STRING";
            break;
        case KDL_TOKEN_RAW_STRING:
            token_type_name = "KDL_TOKEN_RAW_STRING";
            break;
        case KDL_TOKEN_SINGLE_LINE_COMMENT:
            token_type_name = "KDL_TOKEN_SINGLE_LINE_COMMENT";
            break;
        case KDL_TOKEN_SLASHDASH:
            token_type_name = "KDL_TOKEN_SLASHDASH";
            break;
        case KDL_TOKEN_MULTI_LINE_COMMENT:
            token_type_name = "KDL_TOKEN_MULTI_LINE_COMMENT";
            break;
        case KDL_TOKEN_EQUALS:
            token_type_name = "KDL_TOKEN_EQUALS";
            break;
        case KDL_TOKEN_START_CHILDREN:
            token_type_name = "KDL_TOKEN_START_CHILDREN";
            break;
        case KDL_TOKEN_END_CHILDREN:
            token_type_name = "KDL_TOKEN_END_CHILDREN";
            break;
        case KDL_TOKEN_NEWLINE:
            token_type_name = "KDL_TOKEN_NEWLINE";
            break;
        case KDL_TOKEN_SEMICOLON:
            token_type_name = "KDL_TOKEN_SEMICOLON";
            break;
        case KDL_TOKEN_LINE_CONTINUATION:
            token_type_name = "KDL_TOKEN_LINE_CONTINUATION";
            break;
        default:
            break;
        }

        if (token_type_name == NULL) {
            fprintf(stderr, "Unknown token type %d\n", (int)token.type);
            have_error = true;
            break;
        }

        printf("%s", token_type_name);
        if (token.value.len != 0) {
            kdl_owned_string esc_val = kdl_escape(&token.value, KDL_ESCAPE_ASCII_MODE);
            printf(" \"%s\"", esc_val.data);
            kdl_free_string(&esc_val);
        }
        puts("");
    }

    if (in != stdin) {
        fclose(in);
    }
    if (tokenizer != NULL) {
        kdl_destroy_tokenizer(tokenizer);
    }
    return have_error ? 1 : 0;
}
