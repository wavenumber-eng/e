## Lists

singly linked lists

1st argument is always void * to the next item

```
typedef struct
{
	void *next;	  
..
..

```

### doubly linked lists

1st argument is always void * to the next item
2ns argument is always void * to the previous

typedef struct
{
	void *next;	  
    void *prev;

    ...
    ..