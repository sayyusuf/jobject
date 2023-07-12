#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "../str_lexer/str_lexer.h"
#include "../cmap/cmap.h"
#include "../cvec/cvec.h"
#include "../cjson.h"
#include <assert.h>

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
	s[n] = 0;
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


int	is_null(const char *s)
{
	if (!s || strcmp(s, "null"))
		return (0);
	return (1);
}

int	is_boolean(const char *s)
{
	if (!s || (strcmp(s, "true") && strcmp(s, "false")))
		return (0);
	return (1);
}

int	is_number(const char *s)
{
	int	dc;

	if (!s)
		return (0);
	dc = 0;
	while (*s)
	{
		if (!isdigit(*s))
		{
			if (*s == '.' && !dc)
				++dc;
			else
				return (0);
		}
		++s;
	}
	return (1);
}


int	is_string(const char *s)
{
	size_t	i;

	if (!s)
		return (0);
	if (s[0] != '\"')
		return (0);
	i = 1;
	while (s[i])
		++i;
	if (i < 2 || s[i - 1] != '\"')
		return (0);
	return (1);
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


string_t string_parser(const char *s)
{
	string_t str;
	str = ft_strdup(s + 1);
	str[strlen(str) - 1] = 0;
	return (str);
}

int JObject_parser(cvec_t *vec, JObject_t *obj);

int	get_any(JObject_t *obj, cvec_t *vec)
{
	char *s;

	if (!vec || !obj)
		return (-1);
	if (cvec_get(vec, &s, 0)< 0)
		return (-1);
	if (is_null(s))
	{
		cvec_erase(vec, 0, NULL);
		JObject_init(obj, null_e);
		free(s);
	}
	else if (is_boolean(s))
	{
		cvec_erase(vec, 0, NULL);
		JObject_init(obj, boolean_e);
		obj->data.boolean = bool_parser(s);
		free(s);
	}
	else if (is_number(s))
	{
		cvec_erase(vec, 0, NULL);
		JObject_init(obj, number_e);
		obj->data.number = number_parser(s);
		free(s);
	}
	else if (is_string(s))
	{
		cvec_erase(vec, 0, NULL);
		JObject_init(obj, string_e);
		obj->data.string = string_parser(s);
		free(s);
	}
	else if (!strcmp("[", s) || !strcmp("{", s))
		return JObject_parser(vec, obj);
	else
		return (-1);
	return (0);
}

/*		if (!strcmp(s, ","))
		{
			free(s);
			if (!f)
				return (-1);
			if (cvec_get(vec, &s, 0)< 0)
				return (-1);
		}*/



int detect(const char *s, const char *arr[])
{
	size_t	i;
	if (!arr || !s)
		return (-1);
	i = 0;
	while (arr[i])
	{
		if (!strcmp(arr[i], s))
			return (1 << i);
		++i;
	}
	return (1 << i); 
}

int array_parser(cvec_t *vec, JObject_t *obj)
{
	char	*s;
	int 	res;
	int		wait_v;
	const char *str[] = {"]", ",", NULL};

	JObject_t any;

	wait_v = 1 | 4;
	if (JObject_init(obj, array_e) < 0)
		return (-1);
	while (1)
	{
		if (cvec_get(vec, &s, 0)< 0)
			return (-1);
		if (!(wait_v && detect(s, str)))
		{
			free(s);
			return (-1);
		}
		switch (detect(s, str))
		{
			case 1:
				cvec_erase(vec, 0, NULL);
				free(s);
				return (0);
				break;
			case 2:
				cvec_erase(vec, 0, NULL);
				free(s);
				wait_v = 4;
				break;

			case 4:
				res = get_any(&any, vec);
				if (res < 0)
					return (-1);
				if (JObject_array_push(obj, &any) < 0)
					return (-1);
				wait_v = 1 | 2;
				break;
			default:
				break;
		}
	}
	return (0);
}

int map_parser(cvec_t *vec, JObject_t *obj)
{
	int		f;
	char	*s;
	char 	*key;
	int 	res;
	int		wait_v;
	const char *str[] = {"}", ",", ":", NULL};

	if (JObject_init(obj, map_e) < 0)
		return (-1);

	JObject_t any;
	key = NULL;
	f = 0;
	wait_v = 1 | 8;
	while (1)
	{
		if (cvec_get(vec, &s, 0)< 0)
			return (-1);
		if (!(wait_v && detect(s, str)))
		{
			if (key)
				free(key);
			cvec_erase(vec, 0, NULL);
			free(s);
			return (-1);
		}
		switch (detect(s, str))
		{
			case 1:
				if (key)
					free(key);
				cvec_erase(vec, 0, NULL);
				free(s);
				return (0);
			case 2:
				cvec_erase(vec, 0, NULL);
				free(s);
				wait_v = 8;
				break;
			case 4:
				cvec_erase(vec, 0, NULL);
				free(s);
				wait_v = 8;
				break;
			case 8:
				if (!f)
				{
					cvec_erase(vec, 0, NULL);
					if (!is_string(s))
					{
						free(s);
						return (-1);
					}
					key = string_parser(s);
					free(s);
					f = 1;
					wait_v = 4;
					break;;
				}
				res = get_any(&any, vec);
				if (res < 0)
					return (-1);
				res = JObject_map_insert(obj, key, &any);
				free(key);
				key = NULL;
				if (res < 0)
					return (-1);
				wait_v = 1 | 2;
				f = 0;
				break;
			default:
				break;
		}
	}
	return (0);
}

int JObject_parser(cvec_t *vec, JObject_t *obj)
{
	char	*detect;
	int		res;

	res = 0;
	detect = NULL;
	if (cvec_get(vec, &detect, 0) < 0)
		return (-1); 
	cvec_erase(vec, 0, NULL);
	if (!strcmp(detect, "["))
		res = array_parser(vec, obj);
	else if (!strcmp(detect, "{"))
		res = map_parser(vec, obj);
	else
		res = -1;
	free(detect);
	return (res);
}


int main()
{
	const char *s1 = "[    {       \"hello\":  \"hi\"    }, true, 0.23234545345, null ]";
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

	JObject_t obj;
	JObject_parser(&vec, &obj);
	char * s= JSON_stringify(&obj);
	JObject_destroy(&obj);
	printf("%s\n", s);
	free(s);
	//assert(!cvec_iter(&vec, NULL, test));
	cvec_destroy(&vec, destroy);
	return (0);

}