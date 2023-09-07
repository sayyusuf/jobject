#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include <str_lexer.h>
#include <cmap.h>
#include <cvec.h>
#include <jobject.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>


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
	s = strdup(info->key);
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
	s = strndup(*text, i + 1);
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
	s = strndup(*text, i);
	vec = data_struct;
	cvec_push(vec, &s);
	(*text) += i;
	return (LEXER_SUCCESS);
}

void destroy(void *data_addr)
{
	char *s = *((char **)data_addr);
	free(s);
}

void test(void *data_addr ,void * any)
{
	(void) any;
	char *s = *((char **)data_addr);
	printf("%s\n", s);
}

int main()
{
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

	char buff[20000];
	int fd;
	int res;
	fd = open("res.json", O_RDONLY);
	assert(0 < fd);
	res = read(fd, buff, 20000);
	assert(0 < res);
	buff[res] = 0;

	size_t index;
	cvec_t	vec;
	cvec_init(&vec, sizeof(char *), 4);
	printf("lexer_satus %d\n",str_lexer(buff, &vec, rules, &index));

	assert(!cvec_iter(&vec, NULL, test));
	printf("vec len %ld\n", vec.size);	
	JObject_t obj;
	if (!JObject_parser(&vec, &obj))	
	{
		char * s= JObject_stringify(&obj);
		write(open("tmp.json", O_CREAT | O_WRONLY | O_TRUNC, 0777), s, strlen(s));
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
