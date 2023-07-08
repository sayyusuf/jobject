#ifndef CJSON_H
#define CJSON_H

#include <stdint.h>

#include "cmap/cmap.h"
#include "cvec/cvec.h"


typedef double  J_number;
typedef uint8_t J_bool;
typedef char *  J_string;

typedef struct J_object_s
{
    enum jtype {j_bool, j_number, j_string, j_array, j_map};
    union
    {
        J_bool      jbool;
        J_number    j_number;
        J_string    j_string;
        cvec_t      j_array;
        cmap_t      j_map;
    };
} JObject_t;

int JSON_parse(JObject_t *obj, const char *str);


#endif