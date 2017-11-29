/*
** 		_
**     |
** |___|___
**     |   |
**	  _|
**
*/

typedef enum {
    t_function;
    t_ID;
} elem_type;

typedef struct symtab_t symtab_t;

typedef struct symtab_elem_t {
    string name;
    char id;
    type_t type;
    struct symtab_elem_t *lptr;
    struct symtab_elem_t *rptr;
} symtab_elem_t;

struct symtab_t{
    symtab_elem_t *elements[TABLE_SIZE];
    size_t size;
};

int symTableSearch(symtab_elem_t Root, elem_type typ, string nazov,char k);
void symTableInsert(symtab_elem_t* Root, char k, string nazov, elem_type typ);
