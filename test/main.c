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
	size_t		i;

	i = 0;
	root = data_struct;
	JObject_init(&array, j_array);
	str = info->key;
	(*index) += strlen(str);
	(*text) += *index;
	*index = 0;
	 if (str_parser(*text, &array, rules, &i) < 0)
	 	return (-1);
	*text = &((*text)[i]);


	return 0;
}

int	map_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	(void)data_struct;
	const char *str;
	JObject_t	map;
	JObject_t	*root;
	size_t		i;

	i = 0;
	root = data_struct;
	JObject_init(&map, j_map);
	str = info->key;
	(*index) += strlen(str);
	(*text) += *index;
	*index = 0;
	 if (str_parser(*text, &map, rules, &i) < 0)
	 	return (-1);
	*text = &((*text)[i]);
	(void)data_struct;
	const char *str;

	return 0;
}


static char* ft_strdup(const char *str)
{
	char *s;

	s = malloc(strlen(str) + 1);
	strcpy(s, str);
	return (s);	
}
int main()
{
	const char *s1 = "[    {       \"hello\":  \"hi\"    }  ]";


	JObject_t array;
	JObject_t array2;
	JObject_t b;
	JObject_t str;
	JObject_t map;
	JObject_t msg;
	JObject_t num;
	size_t index;

	JObject_init(&b, j_bool);
	JObject_init(&str, j_string);
	JObject_init(&array, j_array);
	JObject_init(&array2, j_array);
	JObject_init(&map, j_map);
	JObject_init(&msg, j_string);
	JObject_init(&num, j_number);

	num.j_number = 0.55552323;
	msg.j_string = ft_strdup("hello");

	cmap_insert(&map.j_map, ft_strdup("hi"), &msg);
	cmap_insert(&map.j_map, ft_strdup("array"), &array2);
	cmap_insert(&map.j_map, ft_strdup("num"), &num);

	cvec_push(&array.j_array, &b);
	cvec_push(&array.j_array, &str);
	cvec_push(&array.j_array, &map);

	index = 0;
	char *s = JSON_stringify(&array);
	printf("%s\n", s);
	free(s);

	JObject_destroy(&array);
	//str_parser(s1, &array, rules, &index);
	return (0);

}