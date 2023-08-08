#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include <str_lexer.h>
#include <cmap.h>
#include <cvec.h>
#include <jobject.h>

static char *ft_strdup(const char *str)
{
	char *s;

	s = malloc(strlen(str) + 1);
	strcpy(s, str);
	return (s);	
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
		if (!(wait_v & detect(s, str)))
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
		if (!(wait_v & detect(s, str)))
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
