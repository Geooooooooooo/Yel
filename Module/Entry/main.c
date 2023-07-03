#include "../IO/io.h"
#include "../Lexer/lexer.h"
#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../Dependencies/dependencies.h"
#include "../VM/yvm.h"

// TODO: fix SYNTAXER for: (a1=b1=c1=f1, hello); print(a1=(b1=(c1=f1)), a2=(b2=(c2=f2)));

// TODO: if (not (a = foo(b, c)))
//          exit(bar());
// fix error

// fix: not a != (10 + 10);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Inline mode is not available\nUse: 'yel [filename]'\n");
        return -1;
    }

    YelSource source = yel_stdio_read_file(argv[1]);
    if (source.source_text == NULL)
        return 0;

    YelTokens yeltokens = yel_parse_tokens(&source);
    if (yeltokens.error)
        goto _yel_end;
    
    yel_init_data_seg();

    YelByteCode bytecode;
    yel_gen_opcode(&yeltokens, &bytecode);

    if (yeltokens.error) 
        goto _yel_end;

    //print_disassembly_bytecode(bytecode);
    //puts("");

    OPCODEWORD* stack = yel_init_stack(256);
    yel_run(stack, &bytecode, 256);

    //print_const_int_data();

_yel_end:
    yel_free_tokens(&yeltokens);
    __builtin_free(source.source_text);
    __builtin_free(bytecode.opcode);
    yel_free_data_seg();

    return 0;
}