#include "heap.h"

#include <string.h>
#include <stdbool.h>

#include "utilities.h"

void init_Heap(Heap * heap)
{
	heap->nodes = malloc(sizeof(HeapNode) * 128);
	heap->allocated_nodes = 128;
	heap->nodes_no = 0;
}

void resize_Heap(Heap * heap, int new_size)
{
	heap->nodes = realloc(heap->nodes, new_size * sizeof(HeapNode));
	heap->allocated_nodes = new_size;
}

void swap_HeapNodes(HeapNode * a, HeapNode * b)
{
	HeapNode temp;
	memcpy(&temp, a, sizeof(HeapNode));
	memcpy(a, b, sizeof(HeapNode));
	memcpy(b, &temp, sizeof(HeapNode));
}

void insert_HeapNode(Heap * heap, void *data, int cmp_value)
{
	if (heap->nodes_no >= heap->allocated_nodes - 1) {
		resize_Heap(heap, heap->allocated_nodes * 2);
	}
	heap->nodes_no++;
	int index = heap->nodes_no;
	HeapNode *node = &heap->nodes[index];
	node->data = data;
	node->cmp_value = cmp_value;

	while (index != 0
		   && heap->nodes[index / 2].cmp_value > heap->nodes[index].cmp_value) {
		swap_HeapNodes(&heap->nodes[index / 2], &heap->nodes[index]);
		index /= 2;
	}
}

void heapify_min(Heap * heap, int index)
{
	int left = 2 * index, right = 2 * index + 1, smallest = index;
	if (left <= heap->nodes_no &&
		heap->nodes[left].cmp_value < heap->nodes[smallest].cmp_value) {
		smallest = left;
	}

	if (right <= heap->nodes_no &&
		heap->nodes[right].cmp_value < heap->nodes[smallest].cmp_value) {
		smallest = right;
	}

	if (smallest != index) {
		swap_HeapNodes(heap->nodes + index, heap->nodes + smallest);
		heapify_min(heap, smallest);
	}
}

void *extract_HeapNode(Heap * heap)
{
	if (heap->nodes_no == 0) {
		return NULL;
	}
	int index = 1;
	HeapNode *root = &heap->nodes[index], *last = &heap->nodes[heap->nodes_no];
	void *data = root->data;
	swap_HeapNodes(root, last);
	heap->nodes_no--;
	heapify_min(heap, 1);
	return data;
}

void clean_Heap(Heap * heap)
{
	if (heap->nodes) {
		free(heap->nodes);
	}
}
