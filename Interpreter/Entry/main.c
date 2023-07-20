#include "../IO/io.h"
#include "../Lexer/lexer.h"
#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../YVM/yvm.h"
#include "../YVM/vm_main.h"

/*          === TODO ===
    1. parsers for expr, decl and stmt
*/

// fix: (a1=b1=c1=f1, hello); print(a1=(b1=(c1=f1)), a2=(b2=(c2=f2)));
// fix: if (not (a = foo(b, c))) exit(bar());

// TODO: create ref to vars into heap in bytecode (!!!!!!!!!!!!!!!!!!!!!!!!). this will give a high speed

int dis_flag = 0;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Inline mode is not available\nUse: 'yel [filename] [OPTIONS ... ]'\n");
        return -1;
    }

    int file_field = 1;

    if (argc == 3) {
        if (strcmp(argv[2], "-d") == 0 || strcmp(argv[2], "--disassembly") == 0) {
            dis_flag = 1;
            file_field = 1;
        }
        else if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "--disassembly") == 0) {
            dis_flag = 1;
            file_field = 2;
        }
        else {
            puts("Unknown argument\nUse: 'yel [filename] [OPTIONS ... ]'");
            return -1;
        }
    }

    YelSource source = yel_stdio_read_file(argv[file_field]);
    if (source.source_text == NULL)
        return 0;

    YelTokens yeltokens = yel_parse_tokens(&source);
    if (yeltokens.error)
        goto _yel_end;

    //print_tokens();
    //getchar();
    
    yel_init_data_seg();

    YelByteCode bytecode;
    yel_gen_opcode(&yeltokens, &bytecode);

    if (yeltokens.error) 
        goto _yel_end;

    return 0;

    if (dis_flag) {
        print_disassembly_bytecode(bytecode);
        goto _yel_end;
    }

    OPCODEWORD* stack = yel_init_stack(65536UL);

    yvm_main(stack, &bytecode, 65536UL);

_yel_end:
    yel_free_tokens(&yeltokens);
    __builtin_free(source.source_text);
    __builtin_free(bytecode.opcode);
    yel_free_data_seg();

    return 0;
}