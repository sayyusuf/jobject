#include <jobject.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
	
static char *
J_bool_stringify(bool_t b)
{
	if (b)
		return (strdup("true"));
	return (strdup("false"));
}

static char *
J_null_stringify(void)
{
		return (strdup("null"));
}

static char *
J_number_stringify(number_t num)
{
	char output[50];

	snprintf(output, 50, "%f", num);
	return (strdup(output));	
}

static char *
J_string_stringify(string_t str)
{
	char *s;
	size_t len;

	len = 0;
	if (str)
		len = strlen(str);
	s = malloc(len + 3);
	*s = 0;
	strcat(s, "\"");
	if (str)
		strcat(s, str);
	strcat(s, "\"");
	return (s);	
}

static void
arry_callback(void *element_addr, void *any)
{
	JObject_t *obj = ((JObject_t *)element_addr);
	cvec_t *vec = any;
	char *s;
	if (vec->size > 1)
	{
		s = strdup(",");
		cvec_push(vec, &s);
	}
	s = JObject_stringify(obj);
	cvec_push(vec, &s);
}

static void
str_size(void *element_addr, void *any)
{
	char *s = *((char **)element_addr);
	size_t	*i = any;

	(*i) += strlen(s);
}

static void
str_join(void *element_addr, void *any)
{
	char *s = *((char **)element_addr);
	char 	*str = any;
	strcat(str, s);
}

static void
str_del(void *element_addr)
{
	char *s = *((char **)element_addr);
	free(s);
}


static char *
J_array_stringify(cvec_t *arry)
{

	cvec_t	vec;
	char *s;
	size_t	len;

	cvec_init(&vec, sizeof(char *), 4);
	s = strdup("[");
	cvec_push(&vec, &s);
	cvec_iter(arry, &vec, arry_callback);
	s = strdup("]");
	cvec_push(&vec, &s);
	len = 0;
	cvec_iter(&vec, &len, str_size);
	s = malloc(len + 1);
	*s = 0;
	cvec_iter(&vec, s, str_join);
	cvec_destroy(&vec, str_del);
	return (s);	
}


static void
map_callback(void *key, void *val_addr, void *any)
{
	JObject_t *obj = ((JObject_t *)val_addr);
	cvec_t *vec = any;
	char *str;
	char *s;
	char *tmp;
	size_t len;

	if (vec->size > 1)
	{
		s = strdup(",");
		cvec_push(vec, &s);
	}
	tmp = J_string_stringify(key);
	s = JObject_stringify(obj);
	len = strlen(s) + strlen(tmp) + 2;
	str = malloc(len);
	*str = 0;
	strcat(str, tmp);
	strcat(str, ":");
	strcat(str, s);
	free(s);
	free(tmp);
	cvec_push(vec, &str);
}

static char *
J_map_stringify(cmap_t *map)
{

	cvec_t	vec;
	char *s;
	size_t	len;

	cvec_init(&vec, sizeof(char *), 4);
	s = strdup("{");
	cvec_push(&vec, &s);
	cmap_iter(map, &vec, map_callback);
	s = strdup("}");
	cvec_push(&vec, &s);
	len = 0;
	cvec_iter(&vec, &len, str_size);
	s = malloc(len + 1);
	*s = 0;
	cvec_iter(&vec, s, str_join);
	cvec_destroy(&vec, str_del);
	return (s);	
}


char *
JObject_stringify(JObject_t *obj)
{
	if (!obj)
		return (NULL);
	switch (obj->type)
	{
		case null_e:
			return (J_null_stringify());
		case boolean_e:
			return (J_bool_stringify(obj->data.boolean));
		case number_e:
			return (J_number_stringify(obj->data.number));
		case string_e:
			return (J_string_stringify(obj->data.string));
		case array_e:
			return (J_array_stringify(&obj->data.array));
		case map_e:
			return (J_map_stringify(&obj->data.map));
		default:
			return (NULL);
	}
}
