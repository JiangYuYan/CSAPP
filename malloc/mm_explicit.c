/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  Blocks are never coalesced or reused.  The size of
 * a block is found at the first aligned word before the block (we need
 * it for realloc).
 *
 * This code is correct and blazingly fast, but very bad usage-wise since
 * it never frees anything.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

#define BESTFIT
/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

// #define NEXT_FIT

/* $begin mallocmacros */
/* Basic constants and macros */
#define WSIZE       4       /* Word and header/footer size (bytes) */ //line:vm:mm:beginconst
#define DSIZE       8       /* Double word size (bytes) */
#define CHUNKSIZE  (1<<12)  /* Extend heap by this amount (bytes) */  //line:vm:mm:endconst 

#define MAX(x, y) ((x) > (y)? (x) : (y))  
#define MIN(x, y) ((x) < (y)? (x) : (y))

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (DSIZE-1)) & ~0x7)

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc)) //line:vm:mm:pack

/* Read and write a word at address p */
#define GET(p)       (*(unsigned int *)(p))            //line:vm:mm:get
#define PUT(p, val)  (*(unsigned int *)(p) = (val))    //line:vm:mm:put

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_FREE(p)  (!GET_ALLOC(p))

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)       ((char *)(bp) - WSIZE)                      //line:vm:mm:hdrp
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //line:vm:mm:ftrp

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //line:vm:mm:nextblkp
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //line:vm:mm:prevblkp

/* Given free block ptr bp, compute address of next free ptr */
#define NEXT_FREE(bp)  (*(void **)(bp))
#define PREV_FREE(bp)  (*((void **)(bp)+1))
#define PUT_NEXT_FREE(bp, next_ptr)  (*(void **)(bp) = (void *)(next_ptr))
#define PUT_PREV_FREE(bp, prev_ptr)  (*((void **)(bp) + 1) = (void *)(prev_ptr))
/* $end mallocmacros */

/* Global variables */
static void **root_listp;
static char *heap_listp = 0;  /* Pointer to first block */  
#ifdef NEXT_FIT
static char *rover;           /* Next fit rover */
#endif

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);

static inline void add_free_ptr(void *bp);
static inline void coalesce_free_neighbor(void *bp);

#ifndef NDEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#define dbg_checkheap() mm_checkheap(1)
#else
#define dbg_printf(...)
#define dbg_checkheap()
#endif

/*
 * mm_init - Called when a new trace starts.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((root_listp = mem_sbrk(DSIZE)) == (void *)-1) //line:vm:mm:begininit
        return -1;
    *root_listp = 0;
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) //line:vm:mm:begininit
        return -1;
    PUT(heap_listp, 0);                          /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */ 
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */ 
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));     /* Epilogue header */
    heap_listp += (2*WSIZE);                     //line:vm:mm:endinit  
    /* $end mminit */

#ifdef NEXT_FIT
    rover = heap_listp;
#endif
    /* $begin mminit */

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) 
        return -1;
    dbg_checkheap();
    return 0;
}

/*
 * malloc - Allocate a block by incrementing the brk pointer.
 *      Always allocate a block whose size is a multiple of the alignment.
 */
void *malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;      

    /* $end mmmalloc */
    if (heap_listp == 0){
        mm_init();
    }
    /* $begin mmmalloc */
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= 2*DSIZE)                                          //line:vm:mm:sizeadjust1
        asize = 3*DSIZE;                                        //line:vm:mm:sizeadjust2
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); //line:vm:mm:sizeadjust3

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) == NULL) {  //line:vm:mm:findfitcall
        extendsize = MAX(asize,CHUNKSIZE);                 //line:vm:mm:growheap1
        if ((bp = extend_heap(extendsize/WSIZE)) == NULL)  
            return NULL;
    }

    /* No fit found. Get more memory and place the block */
    place(bp, asize);                                 //line:vm:mm:growheap3
    dbg_checkheap();
    return bp;
}

/*
 * free - We don't know how to free a block.  So we ignore this call.
 *      Computers have big memories; surely it won't be a problem.
 */
void free(void *ptr){
    /* $end free */
    if (ptr == NULL) 
        return;

    /* $begin free */
    size_t size = GET_SIZE(HDRP(ptr));

    /* $end free */
    if (heap_listp == 0){
        mm_init();
    }
    /* $begin mmfree */

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));

    coalesce(ptr);

    dbg_checkheap();
}
/* $end free */

/* 
 * coalesce_free_neighbor
*/
static inline void coalesce_free_neighbor(void *bp) {
    void **prev_free_ptr = PREV_FREE(bp);
    void **next_free_ptr = NEXT_FREE(bp);
    assert(prev_free_ptr);
    PUT_NEXT_FREE(prev_free_ptr, next_free_ptr);
    if (next_free_ptr) {
        PUT_PREV_FREE(next_free_ptr, prev_free_ptr);
    }
}

/* 
 * add_free_ptr
*/
static inline void add_free_ptr(void *bp) {
    void *ptr = NEXT_FREE(root_listp);
    PUT_NEXT_FREE(root_listp, bp);
    PUT_NEXT_FREE(bp, ptr);
    PUT_PREV_FREE(bp, root_listp);
    if (ptr) {
        PUT_PREV_FREE(ptr, bp);
    }
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
/* $begin free */
static void *coalesce(void *bp) 
{
    void *ptr0 = PREV_BLKP(bp);
    void *ptr1 = NEXT_BLKP(bp);
    size_t prev_alloc = GET_ALLOC(FTRP(ptr0));
    size_t next_alloc = GET_ALLOC(HDRP(ptr1));
    size_t size = GET_SIZE(HDRP(bp));
    

    if (prev_alloc && next_alloc) {            /* Case 1 */
        //
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        coalesce_free_neighbor(ptr1);

        size += GET_SIZE(HDRP(ptr1));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        coalesce_free_neighbor(ptr0);

        size += GET_SIZE(HDRP(ptr0));
        PUT(FTRP(bp), PACK(size, 0));
        bp = PREV_BLKP(bp);
        PUT(HDRP(bp), PACK(size, 0));
    }

    else {                                     /* Case 4 */
        if (PREV_FREE(ptr0) == ptr1) {
            PUT_NEXT_FREE(ptr1, NEXT_FREE(ptr0));
            coalesce_free_neighbor(ptr1);
        } else if (PREV_FREE(ptr1) == ptr0) {
            PUT_NEXT_FREE(ptr0, NEXT_FREE(ptr1));
            coalesce_free_neighbor(ptr0);
        } else {
            coalesce_free_neighbor(ptr0);
            coalesce_free_neighbor(ptr1);
        }
        size += GET_SIZE(HDRP(ptr0)) + 
            GET_SIZE(FTRP(ptr1));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    add_free_ptr(bp);
    /* $end free */

    return bp;
}

/*
 * realloc - Change the size of the block by mallocing a new block,
 *      copying its data, and freeing the old block.  I'm too lazy
 *      to do better.
 */
void *realloc(void *oldptr, size_t size)
{
    size_t oldsize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0) {
        free(oldptr);
        return NULL;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if(oldptr == NULL) {
        return malloc(size);
    }

    if (size <= 2*DSIZE)                                          //line:vm:mm:sizeadjust1
        size = 3*DSIZE;                                        //line:vm:mm:sizeadjust2
    else
        size = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); //line:vm:mm:sizeadjust3
    
    oldsize = GET_SIZE(HDRP(oldptr));
    if (size <= oldsize) {
        newptr = oldptr;
        oldsize -= size;
        if (oldsize >= 3 * DSIZE) {
            PUT(FTRP(oldptr), PACK(oldsize, 0));
            PUT(HDRP(oldptr), PACK(size, 1));
            PUT(FTRP(oldptr), PACK(size, 1));
            PUT(HDRP(NEXT_BLKP(oldptr)), PACK(oldsize, 0));
            coalesce(NEXT_BLKP(oldptr));
        }
    } else {
        newptr = malloc(size-DSIZE);

        /* If realloc() fails the original block is left untouched  */
        if(!newptr) {
            return 0;
        }

        /* Copy the old data. */
        memcpy(newptr, oldptr, oldsize);

        /* Free the old block. */
        free(oldptr);
    }

    

    return newptr;
}

/* 
 * extend_heap - Extend heap with free block and return its block pointer
 */
/* $begin mmextendheap */
static void *extend_heap(size_t words) 
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; //line:vm:mm:beginextend
    if ((long)(bp = mem_sbrk(size)) == -1) 
        return NULL;                                        //line:vm:mm:endextend

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */   //line:vm:mm:freeblockhdr
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */   //line:vm:mm:freeblockftr
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */ //line:vm:mm:newepihdr

    /* Coalesce if the previous block was free */
    return coalesce(bp);                                          //line:vm:mm:returnblock
}
/* $end mmextendheap */

/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
/* $begin mmplace */
/* $begin mmplace-proto */
static void place(void *bp, size_t asize)
/* $end mmplace-proto */
{
    size_t csize = GET_SIZE(HDRP(bp));   
    void** prev_ptr = PREV_FREE(bp);
    void** next_ptr = NEXT_FREE(bp);
    if ((csize - asize) >= (3*DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));

        PUT_NEXT_FREE(prev_ptr, bp);
        PUT_NEXT_FREE(bp, next_ptr);
        PUT_PREV_FREE(bp, prev_ptr);
        if (next_ptr) {
            PUT_PREV_FREE(next_ptr, bp);
        }
    }
    else { 
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));

        PUT_NEXT_FREE(prev_ptr, next_ptr);
        if (next_ptr) {
            PUT_PREV_FREE(next_ptr, prev_ptr);
        }
    }
}
/* $end mmplace */

/* 
 * find_fit - Find a fit for a block with asize bytes 
 */
/* $begin mmfirstfit */
/* $begin mmfirstfit-proto */
static void *find_fit(size_t asize)
/* $end mmfirstfit-proto */
{
    /* $end mmfirstfit */

    /* $begin mmfirstfit */
    /* First-fit search */
    void *bp;
    
#ifdef BESTFIT
    void *min_bp;
    size_t min_size = (size_t)(-1);
    size_t curr_size;
    for (bp = NEXT_FREE(root_listp); bp != 0; bp = NEXT_FREE(bp)) {
        if (GET_FREE(HDRP(bp))) {
            curr_size = GET_SIZE(HDRP(bp));
            if (asize <= curr_size && curr_size <= 2 * asize) {
                return bp;
            }
            if (2 * asize < curr_size && curr_size < min_size) {
                min_bp = bp;
                min_size = curr_size;
            }
        }
    }
    if (min_size != (size_t)(-1)) {
        return min_bp;
    }
#else
    for (bp = NEXT_FREE(root_listp); bp != 0; bp = NEXT_FREE(bp)) {
        if (GET_FREE(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
#endif
    return NULL; /* No fit */
}
/* $end mmfirstfit */

/*
 * calloc - Allocate the block and set it to zero.
 */
void *calloc (size_t nmemb, size_t size)
{
  size_t bytes = nmemb * size;
  void *newptr;

  newptr = malloc(bytes);
  memset(newptr, 0, bytes);

  return newptr;
}

/*
 * mm_checkheap - There are no bugs in my code, so I don't need to check,
 *      so nah!
 */
void mm_checkheap(int verbose){
	/*Get gcc to be quiet. */
    if (verbose)
        assert(1);
    
    void* bp = heap_listp;

    // prologue block
    assert(GET_SIZE(HDRP(bp)) == DSIZE);
    assert(GET_ALLOC(HDRP(bp)) == 1);

    bp = NEXT_BLKP(bp);
    while (GET_SIZE(HDRP(bp)) > 0) {
        // block’s address alignment
        assert((size_t)bp % DSIZE == 0);

        // heap boundaries
        assert(mem_heap_lo() < (size_t)bp);
        assert((size_t)bp <= mem_heap_hi());

        // footer == header
        assert(GET(HDRP(bp)) == GET(FTRP(bp)));

        // minimum size
        assert(GET_SIZE(HDRP(bp)) > DSIZE);

        // Check coalescing: no two consecutive free blocks in the heap.
        if (GET_ALLOC(HDRP(bp)) == 0)
            assert(GET_ALLOC(HDRP(PREV_BLKP(bp))) != 0);

        // Check next and prev pointer
        if (GET_FREE(HDRP(bp))) {
            if ((long)NEXT_FREE(bp)) {
                assert(bp == PREV_FREE(NEXT_FREE(bp)));
            }
            assert(bp == NEXT_FREE(PREV_FREE(bp)));
        }
        

        bp = NEXT_BLKP(bp);
    }
    // epilogue block
    assert(GET_SIZE(HDRP(bp)) == 0);
    assert(GET_ALLOC(HDRP(bp)) == 1);
}
