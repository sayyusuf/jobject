#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "../str_lexer/str_lexer.h"
#include "../cmap/cmap.h"
#include "../cvec/cvec.h"
#include "../cjson.h"

static char *ft_strdup(const char *str)
{
	char *s;

	s = malloc(strlen(str) + 1);
	strcpy(s, str);
	return (s);	
}



static char *ft_strndup(const char *str, size_t n)
{
	size_t	len;
	char *s;

	len = strlen(str);
	if (n > len)
		n = len;
	s = malloc(n + 1);
	strncpy(s, str, n);
	return (s);	
}

int	keys_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info);
int	empty_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info);
int	numbers_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info);
int	quotes_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info);

const rule_t *rules[] = {
	&(rule_t){.keys = (char *[]){" ", "\t", "\n",NULL}, .fp = empty_f},
	&(rule_t){.keys = (char *[]){"[", "]", "{", "}", ":", ",", "true", "false", "null" ,NULL}, .fp = keys_f},
	&(rule_t){.keys = (char *[]){"\"",NULL}, .fp = quotes_f},
	&(rule_t){.keys = (char *[]){".", "0", "1", "2", "3", "4", "5","6", "7", "8", "9",NULL}, .fp = numbers_f},
	NULL
};


int	keys_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	cvec_t *vec;

	vec = data_struct;
	char *s;
	(*index) += strlen(info->key);
	(*text) += *index;
	*index = 0;
	s = ft_strdup(info->key);
	cvec_push(vec, &s);
	return (LEXER_SUCCESS);
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

int	quotes_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	char	*s;
	size_t	i;
	cvec_t	*vec;

	(*text) += *index;
	*index = 0;
	i = 1;
	while ((*text)[i] && !is_similar((const char **)info->keys, (*text) + i))
		++i;
	if(!(*text)[i])
		return (-LEXER_ERROR);
	s = ft_strndup(*text, i + 1);
	vec = data_struct;
	cvec_push(vec, &s);
	(*text) += i + 1;
	return (LEXER_SUCCESS);
}

int	numbers_f(const char **text, unsigned int *index, void *data_struct, const rule_info_t *info)
{
	char	*s;
	size_t	i;
	cvec_t	*vec;

	(*text) += *index;
	*index = 0;
	i = 0;
	while ((*text)[i] && is_similar((const char **)info->keys, (*text) + i))
		++i;
	if(!(*text)[i])
		return (-LEXER_ERROR);
	s = ft_strndup(*text, i);
	vec = data_struct;
	cvec_push(vec, &s);
	(*text) += i;
	return (LEXER_SUCCESS);
}

void test(void *data_addr ,void * any)
{
	char *s = *((char **)data_addr);
	printf("%s\n", s);
}

void destroy(void *data_addr)
{
	char *s = *((char **)data_addr);
	free(s);
}



bool_t bool_parser(const char *s)
{
	if (!strcmp("true", s))
		return (1);
	else
		return (0);
}

number_t number_parser(const char *s)
{
	number_t	num;

	sscanf(s, "%lf", &num);
	return (num);
}

int detect_object(char *)
{

}

string_t string_parser(const char *s)
{
	string_t str;
	str = ft_strdup(s + 1);
	str[strlen(str) - 1] = 0;
	return (str);
}

map_t map_parser(cvec_t *vec, JObject_t *map);
{
	J_map map;
	cvec_init(&map, sizeof(JObject_t), 4);
}

int JObject_parser(cvec_t *vec, JObject_t *obj)
{
	char *s;
	s = NULL;

	cvec_get(vec, &s, 0);
	cvec_erase(vec, 0, NULL);
	if (!strcmp(s, "["))
	{
		free(s);
		JObject_init(obj, array_e);
		while (1)
		{
			JObject_t any;
			if (cvec_get(vec, &s, 0)< 0)
				return (-1);
			if (strcmp(s, "]"))
				return (0);
			if (!strcmp(s, ","))
				return (-1);
			if (!strcmp("true", s) || !strcmp("false", s))
			{
				JObject_init(&any, boolean_e);
				any.data.boolean = bool_parser(s);
			}
			else if (s[0] >= '0' || s[0 <= '9'])
			{
				JObject_init(&any, number_e);
				any.data.number = number_parser(s);
			}
			else if (s[0] == '\"')
			{
				JObject_init(&any, string_e);
				any.data.string = string_parser(s);
			}
			else if (!strcmp("[", s) || !strcmp("{", s))
			{

			}
			
		}
		
	}

}


int main()
{
	const char *s1 = "[    {       \"hello\":  \"hi\"    }, true, 0.23234545345 ]";
/*
	size_t index;

	JObject_t array;
	JObject_t array2;
	JObject_t b;
	JObject_t str;
	JObject_t map;
	JObject_t msg;
	JObject_t num;

	b = JObject_boolean_new();
	str = JObject_string_new();
	array = JObject_array_new();
	array2 = JObject_array_new();
	map = JObject_map_new();
	msg = JObject_string_new();
	num = JObject_string_new();

	JObject_number_set(&num, 0.55555552323);
	JObject_string_set(&msg, "hello");
	
	JObject_map_insert(&map, "hi", &msg);
	JObject_map_insert(&map, "array", &array2);
	JObject_map_insert(&map, "num", &num);

	JObject_array_push(&array, &b);
	JObject_array_push(&array, &str);
	JObject_array_push(&array, &map);


	index = 0;
	char *s = JSON_stringify(&array);
	printf("%s\n", s);
	free(s);


	JObject_destroy(&array);
*/


	size_t index;
	cvec_t	vec;
	cvec_init(&vec, sizeof(char *), 4);
	str_lexer(s1, &vec, rules, &index);


	cvec_iter(&vec, NULL, test);
	cvec_destroy(&vec, destroy);


	return (0);

}