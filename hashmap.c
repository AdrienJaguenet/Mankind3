#include "hashmap.h"
#include "utilities.h"

#include <stdlib.h>

Hash hash(HashMap * map, Key key)
{
	if (key < 0) {
		return -(key % map->array_size);
	}
	return key % map->array_size;
}

void init_HashMap(HashMap * map)
{
	map->array_size = 32;
	map->data = calloc(sizeof(void *), map->array_size);
	map->size = 0;
}

void resize_HashMap(HashMap * map, int newSize)
{
	INFO("Resizing hashmap to size %d", newSize);
	HashMapNode **oldNodes = map->data;
	int oldSize = map->size;
	map->data = malloc(sizeof(HashMapNode *) * newSize);
	map->size = 0;
	map->array_size = newSize;
	for (int i = 0; i < oldSize; ++i) {
		HashMapNode *tmp = oldNodes[i];
		while (tmp) {
			insert_into_HashMap(map, tmp->key, tmp->value);
			tmp = tmp->next;
		}
	}
	free(oldNodes);
}

void insert_into_HashMap(HashMap * map, Key key, void *value)
{
	/* Keep the load factor under 0.75 (3/4) */
	if (3 * map->array_size < 4 * map->size) {
		resize_HashMap(map, map->array_size * 2);
	}
	Hash h = hash(map, key);
	HashMapNode *list = map->data[h];
	HashMapNode *node = calloc(sizeof(HashMapNode), 1);
	HashMapNode *tmp = list;
	while (tmp) {
		if (tmp->key == key) {
			tmp->value = value;
			return;
		}
		tmp = tmp->next;
	}
	node->key = key;
	node->value = value;
	node->next = list;
	map->data[h] = node;
}

void *get_from_HashMap(HashMap * map, Key key)
{
	Hash h = hash(map, key);
	HashMapNode *list = map->data[h];
	HashMapNode *tmp = list;
	while (tmp) {
		if (tmp->key == key) {
			return tmp->value;
		}
		tmp = tmp->next;
	}
	return NULL;
}

void delete_from_HashMap(HashMap * map, Key key)
{
	Hash h = hash(map, key);
	HashMapNode *list = map->data[h];
	HashMapNode *previous = NULL;
	HashMapNode *tmp = list;
	while (tmp) {
		if (tmp->key == key) {
			if (previous) {
				previous->next = tmp->next;
			}
			free(tmp);
		}
		previous = tmp;
		tmp = tmp->next;
	}
}

void delete_HashMap(HashMap * map)
{
	for (int i = 0; i < map->array_size; ++i) {
		HashMapNode *list = map->data[i];
		while (list) {
			HashMapNode *next = list->next;
			free(list);
			list = next;
		}
	}
	free(map->data);
}

void for_each_in_HashMap(HashMap * map,
						 void (*fn)(Key key, void *value, void *extra),
						 void *extra)
{
	for (int i = 0; i < map->array_size; ++i) {
		HashMapNode *list = map->data[i];
		while (list) {
			fn(list->key, list->value, extra);
			list = list->next;
		}
	}
}
