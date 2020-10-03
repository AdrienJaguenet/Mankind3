#include <stdlib.h>

/* Binary heap implementation for priority queues */
/* TODO: Fibonacci heap for da sweet O(1) operations */

typedef struct HeapNode HeapNode;
struct HeapNode {
	void *data;
	int cmp_value;
};

typedef struct {
	HeapNode *nodes;
	int nodes_no, allocated_nodes;
} Heap;

void init_Heap(Heap * heap);
void resize_Heap(Heap * heap, int new_size);
void insert_HeapNode(Heap * heap, void *data, int cmp_value);
void *extract_HeapNode(Heap * heap);
void clean_Heap(Heap * heap);
