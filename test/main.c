#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "../str_parser/str_parser.h"
#include "../cmap/cmap.h"
#include "../cvec/cvec.h"
#include "../cjson.h"

rule_t *rules[] = {
	&(rule_t){.keys = (char *[]){" ", "\t", "\n",NULL}, .fp = empty_f},
	&(rule_t){.keys = (char *[]){"[",NULL}, .fp = array_f},
	&(rule_t){.keys = (char *[]){"{",NULL}, .fp = map_f},
	&(rule_t){.keys = (char *[]){"]", "}" ,NULL}, .fp = ret_f},
};


int	ret_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	(*text) += *index;
	*index = 0;
	return (PARSER_BREAK);
}

int	empty_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
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

int	array_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	(void)data_struct;
	const char *str;
	JObject_t	array;
	JObject_t	*root;
	size_t		index;

	index = 0;
	root = data_struct;
	JObject_init(&array, j_array);
	str = info->key;
	(*index) += strlen(str);
	(*text) += *index;
	*index = 0;
	 if (str_parser(*text, &array, rules, index) < 0)
	 	return (-1);
	do
	{
		if (!**text)
			return (-1);
		str = is_similar((char *[]){"]", NULL}, *text);
	}
	while (!str);
	(*index) += strlen(str);
	return 0;
}

int	map_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	(void)data_struct;
	const char *str;

	JObject_init(data_struct, j_map);
	str = info->key;
	(*index) += strlen(str);
	(*text) += *index;
	*index = 0;

	do
	{
		if (!**text)
			return (-1);
		str = is_similar((char *[]){"}", NULL}, *text);
	}
	while (!str);
	(*index) += strlen(str);
	return 0;
}

int main()
{
	const char *s1 = "[    {       \"hello\":  \"hi\"    }  ]";


	JObject_t array;
	size_t index;

	index = 0;
	JObject_init(&array, j_array);
	str_parser(s1, &array, rules, &index);


}