#include "cjson.h"
#include <string.h>

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