#include "cjson.h"
#include <string.h>
#include <stdlib.h>

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

char *
JSON_stringify(JObject_t *obj, void *data)
{
	size_t	string_s;

	string_s = 0;
	
}