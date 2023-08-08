#ifndef CJSON_H
#define CJSON_H

#include <stdint.h>

#include "cmap/cmap.h"
#include "cvec/cvec.h"


typedef double  number_t;
typedef uint8_t bool_t;
typedef char *  string_t;
typedef	cmap_t	map_t;
typedef	cvec_t	array_t;


enum jtype {undef_e ,null_e, boolean_e ,number_e, string_e, array_e, map_e};

typedef struct J_object_s
{
    union
    {
        bool_t		boolean;
        number_t	number;
        string_t	string;
        array_t		array;
        map_t		map;
    } data;
	enum jtype	type;
} JObject_t;

typedef struct
{
	int (*undef)(JObject_t *obj, void *any);
	int (*null)(JObject_t *obj, void *any);
	int (*boolean)(JObject_t *obj, void *any);
	int (*number)(JObject_t *obj, void *any);
	int (*string)(JObject_t *obj, void *any);
	int (*array)(JObject_t *obj, void *any);
	int (*map)(JObject_t *obj, void *any);
}Jobject_match_t;

typedef struct
{
	struct
	{
		JObject_t (*new)(void);
		uint8_t (*get)(JObject_t *obj, int *ret);
		int (*set)(JObject_t *obj, uint8_t value);
	} boolean;
	struct
	{
		JObject_t (*new)(void);
		double (*get)(JObject_t *obj);
		int (*set)(JObject_t *obj, double value);
	} number;
	struct
	{
		JObject_t	(*new)(void);
		const char  *(*get)(JObject_t *obj);
		int (*set)(JObject_t *obj, const char *str);
	}string;

	struct
	{
		JObject_t (*new)(void);
		size_t (*size)(JObject_t *obj);
		int (*push)(JObject_t *obj, JObject_t *addr);
		JObject_t (*pop)(JObject_t *obj);
		JObject_t (*get)(JObject_t *obj, size_t index);
		int (*insert)(JObject_t *obj, JObject_t *addr, size_t index);
		int (*erase)(JObject_t *obj, size_t index);
		int (*iter)(JObject_t *obj, void *any,  void (*f)(JObject_t *elemnt, void *any));
	}array;
	struct
	{
		int (*insert)(JObject_t *map, const char *key, JObject_t *value);
		JObject_t (*find)(JObject_t* map, const char *key);
		int (*erase)(JObject_t *map, const char *key);
		int (*iter)(JObject_t *map, void *any,  void(*f)(const char *key, JObject_t *val_addr, void *any));
	}map;
	
	int	(*init)(JObject_t *obj, enum jtype type);
	JObject_t (*new)(enum jtype type);
	void (*destroy)(JObject_t *obj);

}Jdata_t;


JObject_t
JObject_new(enum jtype type);

int
JObject_init(JObject_t *obj, enum jtype type);

void
JObject_destroy(JObject_t *obj);


JObject_t
JObject_boolean_new(void);

uint8_t
JObject_boolean_get(JObject_t *obj, int *ret);

int
JObject_boolean_set(JObject_t *obj, uint8_t value);


JObject_t
JObject_number_new(void);

double
JObject_number_get(JObject_t *obj);

int
JObject_number_set(JObject_t *obj, double value);


JObject_t
JObject_string_new(void);

const char  *
JObject_string_get(JObject_t *obj);

int
JObject_string_set(JObject_t *obj, const char *str);


JObject_t
JObject_array_new(void);

size_t 
JObject_array_size(JObject_t *obj);

int
JObject_array_push(JObject_t *obj, JObject_t *addr);

JObject_t
JObject_array_pop(JObject_t *obj);

JObject_t
JObject_array_get(JObject_t *obj, size_t index);

int
JObject_array_insert(JObject_t *obj, JObject_t *addr, size_t index);

int
JObject_array_erase(JObject_t *obj, size_t index);

int
JObject_array_iter(JObject_t *obj, void *any,  void (*f)(JObject_t *elemnt, void *any));


JObject_t
JObject_map_new(void);

int
JObject_map_insert(JObject_t *map, const char *key, JObject_t *value);

JObject_t
JObject_map_find(JObject_t* map, const char *key);

int
JObject_map_erase(JObject_t *map, const char *key);

int
JObject_map_iter(JObject_t *map, void *any,  void(*f)(const char *key, JObject_t *val_addr, void *any));



int
JObject_parser(cvec_t *vec, JObject_t *obj);

char *
JObject_stringify(JObject_t *obj);


int
JObject_init(JObject_t *obj, enum jtype type);

void
JObject_destroy(JObject_t *obj);

#endif
