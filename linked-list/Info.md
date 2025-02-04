# Linked List in Linux Kernel

The linked list is a very important data structure that allows a large number of storage with efficient manipulation of data. Most of the kernel code has been written with the help of this data structure. So in the Linux kernel, no need to implement our own Linked List or no need to use 3rd party library. It has a built-in Linked List which is the Doubly Linked List. It is defined in defined in /lib/modules/$(uname -r)/build/include/linux/list.h.

Normally we used to declare a linked list as like the below snippet.
```
struct my_list{
     int data,
     struct my_list *prev;
     struct my_list *next;
};
```

But if want to Implement it in Linux, then you could write like the below snippet.
```
struct my_list{
     struct list_head list;     //linux kernel list implementation
     int data;
};
```

Where struct list_head is declared in list.h.
```
struct list_head {
     struct list_head *next;
     struct list_head *prev;
};
```

## Inialize Linked List Head

Before creating any node in the linked list, we should create a linked list’s head node first. So below macro is used to create a head node.
```
LIST_HEAD(linked_list);
```
* This macro will create the head node structure in the name of the "linked_list" and it will initialize that to its own address

Above macro is being implemented as follows in the list.h
```
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};
```

## Create Node in Linked List

You have to create your linked list node dynamically or statically. Your linked list node should have the member defined in struct list_head. Using the below inline function, we can initialize that struct list_head.
```
INIT_LIST_HEAD(struct list_head *list);
```

For Example, My node is like this.
```
struct my_list{
     struct list_head list;     //linux kernel list implementation
     int data;
};

struct my_list new_node;
```

So we have to initialize the list_head variable using INIT_LIST_HEAD inline function.
```
INIT_LIST_HEAD(&new_node.list);
new_node.data = 10;
```

## Add Node to Linked List

### Add after Head Node

After creating that node, we need to add that node to the linked list. So we can use this inline function to do that.
```
inline void list_add(struct list_head *new, struct list_head *head);
```

Example:
```
list_add(&new_node.list, &etx_linked_list);
```

### Add before Head Node

Insert a new entry before the specified head. This is useful for implementing queues.
```
inline void list_add_tail(struct list_head *new, struct list_head *head);
```

Example:
```
list_add_tail(&new_node.list, &etx_linked_list);
```

## Delete Node from Linked List

### list_del

It will delete the entry node from the list. This function removes the entry node from the linked list by disconnecting prev and next pointers from the list, but it doesn’t free any memory space allocated for the entry node.
```
inline void list_del(struct list_head *entry);
```

### list_del_init

It will delete the entry node from the list and reinitialize it. This function removes the entry node from the linked list by disconnecting prev and next pointers from the list, but it doesn’t free any memory space allocated for the entry node.
```
inline void list_del_init(struct list_head *entry);
```

## Replace Node in Linked List

### list_replace 

This function is used to replace the old node with the new node.
```
inline void list_replace(struct list_head *old, struct list_head *new);
```

If old was empty, it will be overwritten.

### list_replace_init

This function is used to replace the old node with the new node and reinitialize the old entry.
```
inline void list_replace_init(struct list_head *old, struct list_head *new);
```

If old was empty, it will be overwritten.

## Moving Node in Linked List

### list_move

This will delete one list from the linked list and again adds to it after the head node.
```
inline void list_move(struct list_head *list, struct list_head *head);
```

### list_move_tail

This will delete one list from the linked list and again adds it before the head node.
```
inline void list_move_tail(struct list_head *list, struct list_head *head);
```

## Rotate Node in Linked List

### list_rotate_left

This will rotate the list to the left.
```
inline void list_rotate_left(struct list_head *head);
```

## Test the Linked List Entry

### list_is_last

This tests whether list is the last entry in the list head.
```
inline int list_is_last(const struct list_head *list, const struct list_head *head);
```

### list_empty

It tests whether a list is empty or not.
```
inline int list_empty(const struct list_head *head);
```

It returns 1 if it is empty otherwise 0.

### list_is_singular

This will test whether a list has just one entry.
```
inline int list_is_singular(const struct list_head *head);
```

It returns 1 if it has only one entry otherwise 0.

## Split Linked List into two parts

### list_cut_position

This cut a list into two.
This helper moves the initial part of head, up to and including entry, from head to list. You should pass on entry an element you know is on head. list should be an empty list or a list you do not care about losing its data.
```
inline void list_cut_position(struct list_head *list, struct list_head *head, struct list_head *entry);
```
Where,
* struct list_head * list – a new list to add all removed entries
* struct list_head * head– a list with entries
* struct list_head * entry– an entry within the head could be the head itself and if so we won’t cut the list

## Join Two Linked Lists

### list_splice

This will join two lists, this is designed for stacks.
```
inline void list_splice(const struct list_head *list, struct list_head *head);
```
Where,
* const struct list_head * list – the new list to add.
* struct list_head * head – the place to add it in the first list.

## Traverse Linked List

### list_entry

This macro is used to get the struct for this entry.
```
list_entry(ptr, type, member);
```
Where,
* ptr– the struct list_head pointer.
* type – the type of the struct this is embedded in.
* member – the name of the list_head within the struct.

### list_for_each

This macro is used to iterate over a list.
```
list_for_each(pos, head);
```
Where,
* pos –  the &struct list_head to use as a loop cursor.
* head –  the head for your list.

### list_for_each_entry

This is used to iterate over a list of the given type.
```
list_for_each_entry(pos, head, member);
```
Where,
* pos – the type * to use as a loop cursor.
* head – the head for your list.
* member – the name of the list_head within the struct.

### list_for_each_entry_safe

This will iterate over the list of given type-safe against the removal of list entry.
```
list_for_each_entry_safe ( pos, n, head, member);
```
Where, 
* pos – the type * to use as a loop cursor.
* n – another type * to use as temporary storage
* head – the head for your list.
* member – the name of the list_head within the struct.

### list_for_each_prev

This will be used to iterate over a list backward.
```
list_for_each_prev(pos, head);
```
Where,
* pos – the &struct list_head to use as a loop cursor.
* head – the head for your list.

### list_for_each_entry_reverse

This macro is used to iterate backward over the list of the given type.
```
list_for_each_entry_reverse(pos, head, member);
```
Where,
* pos – the type * to use as a loop cursor.
* head  the head for your list.
* member – the name of the list_head within the struct.
