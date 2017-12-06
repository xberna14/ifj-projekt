/**
IFJ PROJEKT 2017
Prekladac jazyka IFJ17
Autori: Dominik Peza (xpezad00)
		Matej Havlas (xhavla06)
        Radoslav Bernath (xberna14)
        Marek Hornak (xhorna13)
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
int num_count = 0;
int E_count = 0;
int id_count = 0;

int dbl_dot_count = 0; // promenna pro kontrolu tecky u doublu
int dot_count = 0; // promenna pro kontrolu spravnosti plne kvalifikovaneho identifikatoru
int quote_count = 0; // promenna pro signalizaci pouziti znaku " uvnitr stringu
int sign_count = 0; // promenna pro signalizaci pouziti nepovinneho znamenka u desetinneho exponentu
int star_count = 0; // promenna pro signalizaci spravneho ukonceni blokoveho komentare
int plus_count = 0; // kontrola jestli je na vstupu +
int minus_count = 0; // kontrola jestli je na vstupu -
int less_count = 0; // kontrolla jestli je na vstupu <
int great_count = 0; // kontrola jestli je na vstupu >
int excl_count = 0;  // kontrola jestli je na vstupu !
int eq_count   = 0; // kontrola jestli je na vstupu =
int quote_count = 0;// kontrola ci je na vstupe /
int mul_count = 0; //kontrola ci je na vstupe *
