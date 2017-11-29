/*
** 		_
**     |
** |___|___
**     |   |
**	  _|
**
*/

typedef enum {
    ST_ELEMTYPE_VAR,
    ST_ELEMTYPE_PARAM,
    ST_ELEMTYPE_FUN,
    ST_ELEMTYPE_BUILTIN,
    ST_ELEMTYPE_CLASS
} st_elemtype_t;

typedef enum {
    ST_DATATYPE_ERROR,
    ST_DATATYPE_VOID,
    ST_DATATYPE_INT,
    ST_DATATYPE_DOUBLE,
    ST_DATATYPE_STRING,
    ST_DATATYPE_BOOL
} st_datatype_t;

typedef union {
    int ival;
    double *dval;
    char *strval;
} st_value_t;

typedef struct symtab_t symtab_t;

typedef struct symtab_elem_t {
    char *id;
    st_elemtype_t elem_type;
    st_datatype_t data_type;
    st_value_t value;
    struct symtab_elem_t *first_param;
    struct symtab_elem_t *last_param;
    struct symtab_elem_t *next_param;
    struct symtab_elem_t *prev_param;
    struct symtab_elem_t *lptr;
    struct symtab_elem_t *rptr;
    tListItem *first_instr;
    symtab_t *local_table;
} symtab_elem_t;

struct symtab_t{
    symtab_elem_t *elements[TABLE_SIZE];
    size_t size;
};
