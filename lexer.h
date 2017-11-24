/*
** 		_
**     |
** |___|___
**     |   |
**	  _|
**
*/

typedef struct Token_t
{
    int Token;
    char *buffer;
    struct Token_t *next;
}

void setSourceFile(FILE *f);
void returnToken(int Token, char *buffer);
Tint getNextToken(char **buffer);
int lexer(string *buffer);
