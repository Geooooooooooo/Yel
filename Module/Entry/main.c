#include "../IO/io.h"
#include "../Lexer/lexer.h"
#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../Dependencies/dependencies.h"
#include "../VM/yvm.h"

// TODO: fix SYNTAXER for: (a1=b1=c1=f1, hello); print(a1=(b1=(c1=f1)), a2=(b2=(c2=f2)));

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No input file\nUse: 'yel <filename>'\n");
        return -1;
    }

    Source source = yel_stdio_read_file(argv[1]);
    if (source.source_text == NULL) {
        return 0;
    }

    YelTokens token_array = yel_parse_tokens(&source);
    if (token_array.error) {
        goto _yel_end;
    }
    
    yel_init_data_seg();

    OPCODES opcodes;
    yel_gen_opcode(&token_array, &opcodes);

    if (token_array.error) goto _yel_end;

    puts("Disassembled code:\n");
    for (size_t i = 0; i < opcodes.len; i++) {
        print_disassembly_opcode(opcodes.codes[i], i);
    }

_yel_end:
    yel_free_tokens(&token_array);
    __builtin_free(source.source_text);
    __builtin_free(opcodes.codes);
    yel_free_data_seg();

    return 0;
}