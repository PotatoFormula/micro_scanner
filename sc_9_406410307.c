#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 32

typedef enum token_types {
    BEGIN,
    END,
    READ,
    WRITE,
    ID,
    INTLITERAL,
    LPAREN,
    RPAREN,
    SEMICOLON,
    COMMA,
    ASSIGNOP,
    PLUOP,
    MINUSOP,
    SCANEOF
} token;

enum fsm_states {
    START_STATE,
    IDENTIFIER_STATE,
    NUMBER_STATE,
    ASSIGN_STATE
};
enum fsm_states state;

int isAlphabet(unsigned char c) {
    if ( c >= 65 && c <= 90) {
        return 1;
    } else if (c >= 97 && c <= 122) {
        return 1;
    } else {
        return 0;
    }
}

int isNumber(unsigned char c) {
    return (c >= '0' && c <= '9') ? 1 : 0;
}

int main(int argc, char *argv[]) {
    // check user input enough argument
    if (argc < 3) {
        printf("USAGE:\n >%s {input_file} {output_file}\n", argv[0]);
        argv[1] = "./file1.txt";
        argv[2] = "./file2.txt";
        printf("Running with defalut values:\n >%s %s %s\n", argv[0], argv[1], argv[2]);
    }

    // open file
    FILE *fin, *fout;
    if ((fin = fopen(argv[1], "r")) == NULL) {
        printf("File: \"%s\" open failed!\n", argv[1]);
        return 1;
    }
    if ((fout = fopen(argv[2], "w")) == NULL) {
        printf("File: \"%s\" open failed!\n", argv[2]);
        return 1;
    }

    int character;
    state = START_STATE;
    char buffer_str[33]; /* 1 more for '\0' */
    char *buffer_ptr;
    while ((character = fgetc(fin)) != EOF)
    {
        switch (state)
        {
        case START_STATE:
            if (isAlphabet(character)) {
                state = IDENTIFIER_STATE;
                ungetc(character, fin);
            } else if (isNumber(character)) {
                state = NUMBER_STATE;
                ungetc(character, fin);
            }

            switch (character)
            {
            case ':':
                state = ASSIGN_STATE;
                break;
            
            case '(':
                fprintf(fout, "\"(\" is LPAREN\n");
                break;

            case ')':
                fprintf(fout, "\")\" is RPAREN\n");
                break;

            case ';':
                fprintf(fout, "\";\" is SEMICOLON\n");
                break;

            case ',':
                fprintf(fout, "\",\" is COMMA\n");
                break;

            case '+':
                fprintf(fout, "\"+\" is PLUOP\n");
                break;

            case '-':
                fprintf(fout, "\"-\" is MINUSOP\n");
                break;
            
            default:
                break;
            }
            break;
        
        case IDENTIFIER_STATE:
            // put whole word into buffer_str
            for (buffer_ptr = buffer_str; ( isAlphabet(character) || isNumber(character) ) && buffer_ptr < buffer_str+sizeof(buffer_str); buffer_ptr++) {
                *buffer_ptr = character;
                character = getc(fin);
            }
            *buffer_ptr = '\0';
            ungetc(character, fin);

            if (strcmp(buffer_str, "begin") == 0) {
                fprintf(fout, "\"%s\" is BEGIN\n", buffer_str);
            } else if (strcmp(buffer_str, "end") == 0) {
                fprintf(fout, "\"%s\" is END\n", buffer_str);
            } else if (strcmp(buffer_str, "read") == 0) {
                fprintf(fout, "\"%s\" is READ\n", buffer_str);
            } else if (strcmp(buffer_str, "write") == 0) {
                fprintf(fout, "\"%s\" is WRITE\n", buffer_str);
            } else {
                fprintf(fout, "\"%s\" is ID\n", buffer_str);
            }

            state = START_STATE;
            break;

        case NUMBER_STATE:
            for (buffer_ptr = buffer_str; isNumber(character) && buffer_ptr < buffer_str+sizeof(buffer_str); buffer_ptr++) {
                *buffer_ptr = character;
                character = getc(fin);
            }
            *buffer_ptr = '\0';
            ungetc(character, fin);

            fprintf(fout, "\"%s\" is INTLITERAL\n", buffer_str);

            state = START_STATE;
            break;

        case ASSIGN_STATE:
            if (character == '=') { fprintf(fout, "\":=\" is ASSIGNOP\n"); }
            state = START_STATE;
            break;

        default:
            printf("undefined fsm_state\n");
            break;
        }
    }

    // I don't know how to represent EOF, so I use '\0'
    fprintf(fout, "\"\\0\" is SCANEOF\n");

    fclose(fin);
    fclose(fout);

    return 0;
}