#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define Glue_(A, B) A##B
#define Glue(A, B) Glue_(A, B)

// TODO: Make the allocators customizable

// ~Stack
#define StackNodeIdent(T) Glue(T, _Node)
#define StackNode(T)                                                           \
    typedef struct StackNodeIdent(T) {                                         \
        T value;                                                               \
        struct StackNodeIdent(T) *previous;                                    \
    }                                                                          \
    StackNodeIdent(T);

#define StackIdent(T) Glue(T, _Stack)
#define Stack(T)                                                               \
    typedef struct {                                                           \
        StackNodeIdent(T) * last;                                              \
        size_t count;                                                          \
    } StackIdent(T);

#define StackNodeAllocIdent(T) Glue(T, _stack_node_alloc)
#define StackNodeAlloc(T)                                                      \
    StackNodeIdent(T) * StackNodeAllocIdent(T)()                               \
    {                                                                          \
        return malloc(sizeof(StackNodeIdent(T)));                              \
    }

#define StackPushIdent(T) Glue(T, _stack_push)
#define StackPush(T)                                                           \
    void StackPushIdent(T)(StackIdent(T) *stack, T value)                      \
    {                                                                          \
        StackNodeIdent(T) *node = StackNodeAllocIdent(T)();                    \
        node->value = value;                                                   \
        StackNodeIdent(T) *previous = stack->last;                             \
        stack->last = node;                                                    \
        stack->last->previous = previous;                                      \
        stack->count++;                                                        \
    }

#define StackPopIdent(T) Glue(T, _stack_pop)
#define StackPop(T)                                                            \
    StackNodeIdent(T) * StackPopIdent(T)(StackIdent(T) * stack)                \
    {                                                                          \
        if (stack->count == 0) return NULL;                                    \
        StackNodeIdent(T) *node = stack->last;                                 \
        if (node) {                                                            \
            stack->last = stack->last->previous;                               \
        }                                                                      \
        stack->count--;                                                        \
        return node;                                                           \
    }

#define StackPrototype(T)                                                      \
    StackNode(T)  \
    Stack(T)  \
    StackNodeAlloc(T)  \
    StackPush(T)  \
    StackPop(T)

// ~DLL
#define DllNodeIdent(T) Glue(T, _DllNode)
#define DllNode(T)                                                             \
    typedef struct DllNodeIdent(T)                                             \
    {                                                                          \
        T value;                                                               \
        struct DllNodeIdent(T) * next;                                         \
        struct DllNodeIdent(T) * previous;                                     \
    }                                                                          \
    DllNodeIdent(T);

#define DllIdent(T) Glue(T, _Dll)
#define Dll(T)                                                                 \
    typedef struct {                                                           \
        DllNodeIdent(T) * start;                                               \
        DllNodeIdent(T) * end;                                                 \
        size_t count;                                                          \
    } DllIdent(T);

#define DllNodeAllocIdent(T) Glue(T, _dll_node_alloc)

// TODO: Maybe I should memset it to zero?
#define DllNodeAlloc(T)                                                        \
    DllNodeIdent(T) * DllNodeAllocIdent(T)()                                   \
    {                                                                          \
        DllNodeIdent(T) *node = malloc(sizeof(DllNodeIdent(T)));               \
        return node;                                                           \
    }

#define DllPushIdent(T) Glue(T, _dll_push)
#define DllPush(T)                                                             \
    void DllPushIdent(T)(DllIdent(T) * dll, T value)                           \
    {                                                                          \
        DllNodeIdent(T) *node = DllNodeAllocIdent(T)();                        \
        node->value = value;                                                   \
        if (dll->count == 0) {                                                 \
            dll->start = node;                                                 \
            dll->end = node;                                                   \
            dll->start->next = dll->end;                                       \
            dll->start->previous = NULL;                                       \
            dll->end->next = NULL;                                             \
            dll->end->previous = dll->start;                                   \
            dll->count++;                                                      \
            return;                                                            \
        }                                                                      \
        DllNodeIdent(T) *previous_end = dll->end;                              \
        dll->end = node;                                                       \
        dll->end->previous = previous_end;                                     \
        previous_end->next = dll->end;                                         \
        dll->count++;                                                          \
    }

#define DllPopIdent(T) Glue(T, _dll_pop)
#define DllPop(T)                                                              \
    DllNodeIdent(T) * DllPopIdent(T)(DllIdent(T) * dll)                        \
    {                                                                          \
        if (dll->count == 0) return NULL;                                      \
        DllNodeIdent(T) *node = dll->end;                                      \
        dll->end = dll->end->previous;                                         \
        dll->end->next = NULL;                                                 \
        dll->count--;                                                          \
        return node;                                                           \
    }

// NOTE: This function returns null, when out of bounds
#define DllGetIdent(T) Glue(T, _dll_get)
#define DllGet(T)                                                              \
    DllNodeIdent(T) * DllGetIdent(T)(DllIdent(T) * dll, size_t idx)            \
    {                                                                          \
        if (idx >= dll->count) return NULL;                                    \
        DllNodeIdent(T) *current = dll->start;                                 \
        for (size_t i = 0; i < idx; ++i) {                                     \
            current = current->next;                                           \
        }                                                                      \
        return current;                                                        \
    }

#define DllRemoveIdent(T) Glue(T, _dll_remove)
#define DllRemove(T)                                                           \
    DllNodeIdent(T) * DllRemoveIdent(T)(DllIdent(T) * dll, size_t idx)         \
    {                                                                          \
        if (idx >= dll->count) return DllPopIdent(T)(dll);                     \
        DllNodeIdent(T) *node = DllGetIdent(T)(dll, idx);                      \
                                                                               \
        if (idx == 0) {                                                        \
            dll->start = node->next;                                           \
            dll->start->previous = NULL;                                       \
        } else if (idx == dll->count - 1) {                                    \
            dll->end = node->previous;                                         \
            dll->end->next = NULL;                                             \
        } else {                                                               \
            node->previous->next = node->next;                                 \
            node->next->previous = node->previous;                             \
        }                                                                      \
                                                                               \
        dll->count--;                                                          \
        return node;                                                           \
    }

#define DllInsertIdent(T) Glue(T, _dll_insert)
#define DllInsert(T)                                                           \
    void DllInsertIdent(T)(DllIdent(T) * dll, size_t idx, T value)             \
    {                                                                          \
        if (idx >= dll->count) {                                               \
            DllPushIdent(T)(dll, value);                                       \
            return;                                                            \
        }                                                                      \
                                                                               \
        DllNodeIdent(T) *node = DllNodeAllocIdent(T)();                        \
        node->value = value;                                                   \
        if (idx == 0) {                                                        \
            dll->start->previous = node;                                       \
            node->previous = NULL;                                             \
            node->next = dll->start;                                           \
            dll->start = node;                                                 \
            dll->count++;                                                      \
            return;                                                            \
        }                                                                      \
                                                                               \
        DllNodeIdent(T) *previous_node = DllGetIdent(T)(dll, idx);             \
        node->next = previous_node;                                            \
        previous_node->previous->next = node;                                  \
        node->previous = previous_node->previous;                              \
        previous_node->previous = node;                                        \
                                                                               \
        dll->count++;                                                          \
    }

#define DllPrototype(T)                                                        \
    DllNode(T) Dll(T) DllNodeAlloc(T) DllPush(T) DllPop(T) DllGet(T)           \
        DllRemove(T) DllInsert(T)

#define DllIter(T, DLL, N)                                                     \
    for (DllNodeIdent(T) *N = (DLL)->start; N != NULL; N = N->next)
#define DllIterRev(T, DLL, N)                                                  \
    for (DllNodeIdent(T) *N = (DLL)->end; N != NULL; N = N->previous)
#define DllIterFrom(S) for (; (S) != NULL; (S) = (S)->next)
#define DllIterFromRev(S) for (; (S) != NULL; (S) = (S)->previous)

// TODO: ~Dynamic Array
#define DaInitCapacity 256

#define DaIdent(T) Glue(T, _Da)
#define Da(T)                                                                  \
    typedef struct {                                                           \
        T *data;                                                               \
        size_t count;                                                          \
        size_t capacity;                                                       \
    } DaIdent(T);

#define DaPushIdent(T) Glue(T, _da_push)
#define DaPush(T)                                                              \
    void DaPushIdent(T)(DaIdent(T) * da, T value)                              \
    {                                                                          \
        if (da->count >= da->capacity) {                                       \
            da->capacity                                                       \
                = da->capacity == 0 ? DaInitCapacity : da->capacity * 2;       \
            da->data = realloc(da->data, da->capacity * sizeof(*da->data));    \
        }                                                                      \
        da->data[da->count++] = value;                                         \
    }

#define DaPushManyIdent(T) Glue(T, _da_push_many)
#define DaPushMany(T)                                                          \
    void DaPushManyIdent(T)(DaIdent(T) * da, T * values, size_t values_count)  \
    {                                                                          \
        if (da->count + values_count > da->capacity) {                         \
            if (da->capacity == 0) {                                           \
                da->capacity = DaInitCapacity;                                 \
            }                                                                  \
                                                                               \
            while (da->count + values_count > da->capacity) {                  \
                da->capacity *= 2;                                             \
            }                                                                  \
                                                                               \
            da->data = realloc(da->data, da->capacity * sizeof(*da->data));    \
        }                                                                      \
                                                                               \
        memcpy(da->data + da->count, values, values_count * sizeof(*values));  \
        da->count += values_count;                                             \
    }

#define DaPrototype(T) Da(T) DaPush(T) DaPushMany(T)

// ~Hash Table
#define HashTableThreshhold 0.75
// NOTE: I heard that its better to have a prime number as the hash table size
#define HashTableInitSize 67

typedef char* cstr;
typedef int32_t I32;

typedef uint8_t U8;
typedef uint64_t U64;

typedef struct {
    cstr key;
    int value;
} cstr_int_KV;

DllPrototype(cstr_int_KV)

typedef struct {
    cstr_int_KV_Dll *buckets;
    size_t capacity;
    size_t count;
} cstr_int_HashTable;

U64 hash(U8 *buf, size_t buf_count)
{
    U64 hash = 5381;
    for (size_t i = 0; i < buf_count; ++i) {
        hash = ((hash << 5) + hash) + buf[i];
    }
    return hash;
}

void cstr_int_hash_table_resize(cstr_int_HashTable *ht)
{
    if (ht->capacity == 0) {
        ht->capacity = HashTableInitSize;
        ht->buckets = malloc(ht->capacity * sizeof(ht->buckets));
    }

    // TODO: Rehash the hash table
}

void cstr_int_hash_table_insert(cstr_int_HashTable *ht, cstr key, int value)
{
    cstr_int_hash_table_resize(ht);
    U64 hash = hash(key);
}

int main(void) 
{ 
    return 0;
};
