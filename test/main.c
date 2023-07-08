#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../str_parser/str_parser.h"
#include "../cmap/cmap.h"
#include "../cvec/cvec.h"
#include "../cjson.h"


int	empty_p(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	(void)data_struct;
    const char *str;

	str = info->key;
    do {
        (*index) += strlen(str);
        str = (is_similar((const char **)info->keys, &(*text)[*index]));
    } while (str);
    (*text) += *index;
    *index = 0;
    return 0;
}

int	array_p(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	(void)data_struct;
    const char *str;

	JObject_init(data_struct, j_array);
	str = info->key;
    (*index) += strlen(str);
    (*text) += *index;
    *index = 0;
    return 0;
}

int main()
{
    const char *s1 = "[    {       \"hello\":  \"hi\"    }]";

    rule_t *rules[] = {
        &(rule_t){.keys = (char *){" ", "\t", "\n",NULL}, .fp = empty_p},
        &(rule_t){.keys = (char *){"[",NULL}, .fp = array_p},
    };

    JObject_t obj;
    
    str_parser(s1, &obj, rules);


}