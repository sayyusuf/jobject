#include "jobject.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

JObject_t
JObject_new(enum jtype type)
{
	JObject_t obj;
	JObject_init(&obj, type);
	return (obj);
}

int
JObject_init(JObject_t *obj, enum jtype type)
{
	if (!obj)
		return (-1);
	switch (type)
	{
		case null_e:
			break;
		case boolean_e:
			obj->data.boolean = 0;
			break;
		case number_e:
			obj->data.number = 0;
			break;
		case string_e:
			obj->data.string = NULL;
			break;
		case array_e:
			if (cvec_init(&obj->data.array, sizeof(*obj), 4) < 0)
				return (-1);
			break;
		case map_e:
			if (cmap_init(&obj->data.map, (int (*)(void *, void *))strcmp, sizeof(*obj)) < 0)
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
		return ;
	switch (obj->type)
	{
		case array_e:
			cvec_destroy(&obj->data.array, (void (*)(void *))JObject_destroy);
			break;
		case map_e:
			cmap_destroy(&obj->data.map, free, (void (*)(void *))JObject_destroy);
			break;
		case string_e:
			free(obj->data.string);
			break;
		default:
			break;
	}
};

int
JObject_match(JObject_t *obj, void *any, Jobject_match_t *match)
{
	int (*f)(JObject_t *obj, void *any);

	if (!obj || !match)
		return (-1);
	switch (obj->type)
	{
		case undef_e:
			f = match->undef;		
			break;
		case null_e:
			f = match->null;
			break;
		case boolean_e:
			f = match->boolean;
			break;
		case number_e:
			f = match->number;
			break;
		case string_e:
			f = match->string;
			break;
		case array_e:
			f = match->array;	
			break;
		case map_e:
				f = match->map;
			break;
		default:
			return (-1);
	}
	if (!f)
		return (-1);
	return (f(obj, any));
}

//********************************************************************************

JObject_t
JObject_boolean_new(void)
{
	return (JObject_new(boolean_e));
}

uint8_t
JObject_boolean_get(JObject_t *obj, int *ret)
{
	if (!ret || !obj || obj->type != boolean_e)
		return (0);
	return (obj->data.boolean);
}

int
JObject_boolean_set(JObject_t *obj, uint8_t value)
{
	if (obj && obj->type == boolean_e)
		obj->data.boolean = value;
	else
		return (-1);
	return (0);
}

//**********************************************************
JObject_t
JObject_number_new(void)
{
	return (JObject_new(number_e));
}

double
JObject_number_get(JObject_t *obj)
{
	if (!obj || obj->type != number_e)
		return (0);
	return (obj->data.number);
}

int
JObject_number_set(JObject_t *obj, double value)
{
	if (obj && obj->type == number_e)
		obj->data.boolean = value;
	else
		return (-1);
	return (0);
}
//********************************************************
JObject_t
JObject_string_new(void)
{
	return (JObject_new(string_e));
}


const char  *
JObject_string_get(JObject_t *obj)
{
	if (!obj || obj->type != string_e)
		return (NULL);
	return (obj->data.string);
}

int
JObject_string_set(JObject_t *obj, const char *str)
{
	if (obj && obj->type == string_e && str)
	{
		if (obj->data.string)
			free(obj->data.string);
		obj->data.string =  strdup(str);
		if (!obj->data.string)
			return (-1);
	}
	else
		return (-1);
	return (0);
}

//*****************************************************
JObject_t
JObject_array_new(void)
{
	return (JObject_new(array_e));
}

size_t 
JObject_array_size(JObject_t *obj)
{
	if (!obj || obj->type != array_e)
		return (0);
	return (obj->data.array.size);
}

int
JObject_array_push(JObject_t *obj, JObject_t *addr)
{
	if (!obj || obj->type != array_e || !addr || addr->type == undef_e)
		return (-1);
	return (cvec_push(&obj->data.array, addr));
}

JObject_t
JObject_array_pop(JObject_t *obj)
{
	JObject_t	ret;

	if (!obj || obj->type != array_e)
		return JObject_new(undef_e);
	if (cvec_pop(&obj->data.array, &ret) < 0)
		return JObject_new(undef_e);
	return (ret);
}

JObject_t
JObject_array_get(JObject_t *obj, size_t index)
{
	JObject_t	ret;

	if (!obj || obj->type != array_e)
		return JObject_new(undef_e);
	if (cvec_get(&obj->data.array, &ret, index) < 0)
		return JObject_new(undef_e);
	return (ret);
}

int
JObject_array_insert(JObject_t *obj, JObject_t *addr, size_t index)
{
	if (!obj || obj->type != array_e || !addr || addr->type == undef_e)
		return (-1);
	return (cvec_insert(&obj->data.array, addr, index));
}

int
JObject_array_erase(JObject_t *obj, size_t index)
{
	if (!obj || obj->type != array_e)
		return (-1);
	return (cvec_erase(&obj->data.array, index, (void (*)(void *))JObject_destroy));
}

int
JObject_array_iter(JObject_t *obj, void *any,  void (*f)(JObject_t *elemnt, void *any))
{
	if (!obj || obj->type != array_e || !f)
		return (-1);
	return (cvec_iter(&obj->data.array, any, (void (*)(void *, void *))f));
}


//*****************************************************************
JObject_t
JObject_map_new(void)
{
	return (JObject_new(map_e));
}

int
JObject_map_insert(JObject_t *map, const char *key, JObject_t *value)
{
	if (!map || map->type != map_e || !key || !value || value->type == undef_e)
		return (-1);
	if (cmap_insert(&map->data.map, strdup(key), value) < 0)
		return (-1);
	return (0);
}

JObject_t
JObject_map_find(JObject_t* map, const char *key)
{
	JObject_t ret;
	if (!map || map->type != map_e || !key)
		return (JObject_new(undef_e));
	if (cmap_find(&map->data.map, (void *)key, &ret) < 0)
		return (JObject_new(undef_e));
	return (ret);
}

static void
map_str_del(void *element_addr)
{
	char *s = *((char **)element_addr);
	free(s);
}

int
JObject_map_erase(JObject_t *map, const char *key)
{
	if (!map || map->type != map_e || !key)
		return (-1);
	return (cmap_erase(&map->data.map, (void *)key, (void (*)(void *))map_str_del, (void (*)(void *))JObject_destroy));
}

int
JObject_map_iter(JObject_t *map, void *any,  void(*f)(const char *key, JObject_t *val_addr, void *any))
{
	if (!map || map->type != map_e || !f)
		return (-1);
	return (cmap_iter(&map->data.map, any, (void(*)(void *, void *, void *))f));
}

//****************************************************************



