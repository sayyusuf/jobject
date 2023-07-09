#include "cjson.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int
JObject_init(JObject_t *obj, enum jtype type)
{
	if (!obj)
		return (-1);
	switch (type)
	{
		case j_bool:
			obj->j_bool = 0;
			break;
		case j_number:
			obj->j_number = 0;
			break;
		case j_string:
			obj->j_string = NULL;
			break;
		case j_array:
			if (cvec_init(&obj->j_array, sizeof(*obj), 4) < 0)
				return (-1);
			break;
		case j_map:
			if (cmap_init(&obj->j_map, (int (*)(void *, void *))strcmp, sizeof(obj->j_map)) < 0)
				return (-1);
			break;
		default:
			return (-1);
	}
	obj->type = type;
	return (0);
}

void
JObject_destroy(JObject_t *obj)
{
	if (!obj)
		return (-1);
	switch (obj->type)
	{
		case j_array:
			cvec_destroy(&obj->j_array, (void (*)(void *))JObject_destroy);
			break;
		case j_map:
			cmap_destroy(&obj->j_map, free, (void (*)(void *))JObject_destroy);
			break;
		case j_string:
			free(obj->j_string);
			break;
		default:
			break;
	}
};

static ft_strdup(const char *str)
{
	char *s;

	s = malloc(strlen(str) + 1);
	strcpy(s, str);
	return (s);	
}

char *
J_bool_stringify(J_bool b)
{
	if (b)
		return (ft_strdup("true"));
	return (ft_strdup("false"));
}

char *
J_number_stringify(J_number num)
{
	char output[50];

	snprintf(output, 50, "%f", num);
	return (ft_strdup(output));	
}

char *
J_string_stringify(J_string str)
{
	return (ft_strdup(str));	
}

void arry_callback(void *element_addr, void *any)
{
	JObject_t *obj = ((JObject_t *)element_addr);
	cvec_t *vec = any;
	char *s = JSON_stringify(obj);
	cvec_push(vec, &s);
}

void str_size(void *element_addr, void *any)
{
	const *s = *((char **)element_addr);
	size_t	*i = any;

	(*i) += strlen(s);
}

void str_join(void *element_addr, void *any)
{
	char *s = *((char **)element_addr);
	char 	*str = any;
	strcat(str, s);
}

void str_del(void *element_addr)
{
	char *s = *((char **)element_addr);
	free(s);
}


char *
J_array_stringify(cvec_t *arry)
{

	cvec_t	vec;
	char *s;
	size_t	len;

	cvec_init(&vec, sizeof(char *), 4);
	s = ft_strdup("[");
	cvec_push(&vec, &s);
	cvec_iter(arry, &vec, arry_callback);
	s = ft_strdup("]");
	cvec_push(&vec, &s);
	len = 0;
	cvec_iter(&vec, &len, str_size);
	s = malloc(len + 1);
	cvec_iter(&vec, s, str_join);
	cvec_destroy(&vec, str_del);
	return (s);	
}


void	map_callback(void *key, void *val_addr, void *any)
{
	JObject_t *obj = ((JObject_t *)val_addr);
	cvec_t *vec = any;
	char *str;

	char *s = JSON_stringify(obj);
	str = malloc(strlen(s) + strlen(key) + 2);
	strcat(str, key);
	strcat(str, ":");
	strcat(str, s);
	free(s);
	cvec_push(vec, &str);
}

char *
J_map_stringify(cmap_t *map)
{

	cvec_t	vec;
	char *s;
	size_t	len;

	cvec_init(&vec, sizeof(char *), 4);
	s = ft_strdup("{");
	cvec_push(&vec, &s);
	cmap_iter(map, &vec, map_callback);
	s = ft_strdup("}");
	cvec_push(&vec, &s);
	len = 0;
	cvec_iter(&vec, &len, str_size);
	s = malloc(len + 1);
	cvec_iter(&vec, s, str_join);
	cvec_destroy(&vec, str_del);
	return (s);	
}


char *
JSON_stringify(JObject_t *obj)
{
	if (!obj)
		return (-1);
	switch (obj->type)
	{
		case j_bool:
			return (J_bool_stringify(obj->j_bool));
		case j_number:
			return (J_number_stringify(obj->j_number));
		case j_string:
			return (J_string_stringify(obj->j_string));
		case j_array:
			return (J_array_stringify(&obj->j_array));
		case j_map:
			return (J_map_stringify(&obj->j_map));
		default:
			return (NULL);
	}
}
