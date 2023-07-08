#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../str_parser/str_parser.h"
#include "../cmap/cmap.h"
#include "../cvec/cvec.h"
#include "..//cjson.h"




int main()
{
    const char *s1 = "[    {       \"hello\":  \"hi\"    }]";

    rule_t *rules[] = {
        &(rule_t){.keys = (char *){"[",NULL}, .fp = NULL},
    };

    JObject_t obj;
    
    parser(s1, &obj, rules);


}