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
} type_t;

typedef struct symtab_t symtab_t;

typedef struct symtab_elem_t {
    char *id;
    type_t typ;
    struct symtab_elem_t *lptr;
    struct symtab_elem_t *rptr;
} symtab_elem_t;

struct symtab_t{
    symtab_elem_t *elements[TABLE_SIZE];
    size_t size;
};
