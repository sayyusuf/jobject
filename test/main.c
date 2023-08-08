#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include <str_lexer.h>
#include <cmap.h>
#include <cvec/cvec.h>
#include <jobject.h>
#include <assert.h>

int main()
{
	const char *s1 = "[   { \"dd\": 555555,  \"hello\":  \"hi\"    }, true, 0.23234545345, null]";
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

	assert(!cvec_iter(&vec, NULL, test));
	
	JObject_t obj;
	if (!JObject_parser(&vec, &obj))	
	{
		char * s= JSON_stringify(&obj);
		printf("%s\n", s);
		free(s);
	}
	else
		printf("parser error\n");
	JObject_destroy(&obj);
	//assert(!cvec_iter(&vec, NULL, test));
	cvec_destroy(&vec, destroy);
	return (0);

}
