#pragma once

#include <stdint.h>

typedef int64_t Key;
typedef int32_t Hash;

typedef struct HashMapNode HashMapNode;

struct HashMapNode {
	void *value;
	Key key;
	HashMapNode *next;
};

typedef struct {
	HashMapNode **data;
	int array_size;
	int size;
} HashMap;

Hash hash(HashMap *, Key);

void init_HashMap(HashMap *);

void insert_into_HashMap(HashMap *, Key, void *);

void *get_from_HashMap(HashMap *, Key);

void delete_from_HashMap(HashMap *, Key);

void delete_HashMap(HashMap *);

void for_each_in_HashMap(HashMap *,
						 void (*fn)(Key key, void *data, void *extra),
						 void *extra);
