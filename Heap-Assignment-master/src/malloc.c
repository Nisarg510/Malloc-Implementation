/*
    Nisarg Shah
    1001553132
 
    Hemantha Govindu
    1001531660
 
 */
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)
#define BLOCK_DATA(b)      ((b) + 1)
#define BLOCK_HEADER(ptr)   ((struct _block *)(ptr) - 1)


static int atexit_registered = 0;
static int num_mallocs       = 0;
static int num_frees         = 0;
static int num_reuses        = 0;
static int num_grows         = 0;
static int num_splits        = 0;
static int num_coalesces     = 0;
static int num_blocks        = 0;
static int num_requested     = 0;
static int max_heap          = 0;

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.  Registered
 *  via atexit()
 *
 *  \return none
 */
void printStatistics( void )
{
  printf("\nheap management statistics\n");
  printf("mallocs:\t%d\n", num_mallocs );
  printf("frees:\t\t%d\n", num_frees );
  printf("reuses:\t\t%d\n", num_reuses );
  printf("grows:\t\t%d\n", num_grows );
  printf("splits:\t\t%d\n", num_splits );
  printf("coalesces:\t%d\n", num_coalesces );
  printf("blocks:\t\t%d\n", num_blocks );
  printf("requested:\t%d\n", num_requested );
  printf("max heap:\t%d\n", max_heap );
}

struct _block
{
   size_t  size;         /* Size of the allocated _block of memory in bytes */
   struct _block *prev;  /* Pointer to the previous _block of allcated memory   */
   struct _block *next;  /* Pointer to the next _block of allcated memory   */
   bool   free;          /* Is this _block free?                     */
   char   padding[3];
   
};


struct _block *freeList = NULL; /* Free list to track the _blocks available */
struct _block *next_fit = NULL;

struct _block *best_fit = NULL;

struct _block *worst_fit = NULL;

/*
 * \brief findFreeBlock
 *
 * \param last pointer to the linked list of free _blocks
 * \param size size of the _block needed in bytes
 *
 * \return a _block that fits the request or NULL if no free _block matches
 *
 * \TODO Implement Next Fit
 * \TODO Implement Best Fit
 * \TODO Implement Worst Fit
 */
struct _block *findFreeBlock(struct _block **last, size_t size)
{
   struct _block *curr = freeList;
    
#if defined FIT && FIT == 0
   
    /* First fit */
    
   while (curr && !(curr->free && curr->size >= size))
   {
      *last = curr;
       
       num_blocks++;
      curr  = curr->next;
   }
    
    
#endif

#if defined BEST && BEST == 0
   
    //printf("TODO: Implement best fit here\n");
    
    if(best_fit == NULL)
    {
        best_fit = curr;
    }
    
    while (curr != NULL)
    {
        //num_frees++;
        
        if(curr->free)
        {
            num_blocks++;
        }
    
          if( curr->size >= size)
          {
                if( best_fit->size < curr->size)
                {
                    best_fit = curr ;
                }
            
                else
                {
                //
                }
          }
        
        
        
        curr = curr->next;
        
    }
    
    *last = best_fit;
    

    
    
#endif

#if defined WORST && WORST == 0
  // printf("TODO: Implement worst fit here\n");
    
    if(worst_fit == NULL)
    {
        worst_fit = curr;
    }
    
    while (curr != NULL)
    {
       /* if(curr->free)
        {
            num_frees++;
        }*/
        if(curr->free && curr->size >= size)
        {
            
            if( worst_fit->size >   curr->size)
            {
                worst_fit = curr ;
            }
            
            else
            {
                //
            }
        }
        
        curr = curr->next;
        
    }
    
    *last = worst_fit;
   

#endif

#if defined NEXT && NEXT == 0
   //printf("TODO: Implement next fit here\n");
    if(next_fit != NULL)
    {
        curr = next_fit;
    }
   
    while(curr && !(curr->free && curr->size >= size))
    {
        *last = curr;
        curr  = curr->next;
        
    }
    
    next_fit = curr;
    

    #endif

    

   return curr;
}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free _block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated _block of NULL if failed
 */
struct _block *growHeap(struct _block *last, size_t size)
{
   /* Request more space from OS */
   struct _block *curr = (struct _block *)sbrk(0);
   struct _block *prev = (struct _block *)sbrk(sizeof(struct _block) + size);

   assert(curr == prev);

   /* OS allocation failed */
   if (curr == (struct _block *)-1)
   {
      return NULL;
   }
    num_grows++;

   /* Update freeList if not set */
   if (freeList == NULL)
   {
      freeList = curr;
   }

   /* Attach new _block to prev _block */
   if (last)
   {
      last->next = curr;
   }

    max_heap+=size;
   /* Update _block metadata */
   curr->size = size;
   curr->next = NULL;
   curr->free = false;
   return curr;
}

/*
 * \brief malloc
 *
 * finds a free _block of heap memory for the calling process.
 * if there is no free _block that satisfies the request then grows the
 * heap and returns a new _block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process
 * or NULL if failed
 */
void *malloc(size_t size)
{

    num_mallocs++;
    num_requested += size;
    
   if( atexit_registered == 0 )
   {
      atexit_registered = 1;
      atexit( printStatistics );
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);

   /* Handle 0 size */
   if (size == 0)
   {
      return NULL;
   }

   /* Look for free _block */
   struct _block *last = freeList;
   struct _block *next = findFreeBlock(&last, size);

   /* TODO: Split free _block if possible */
    if(next != NULL && next->size > size)
    {
     //   printf("zzzzz");
        
        if(num_mallocs > 1)
        {
            num_reuses++;       //the blocks are reused if the we dont request any new block and we have a block that can fit in any existing blocks
        }
        num_splits++;
        
        size_t mem_left;
        mem_left = next->size - size;
        next->size = mem_left;
        struct _block *new = (struct _block *)sbrk(sizeof(struct _block) + mem_left);     //making a new block using the syntax provided in the skeleton code.
        
        if(next->next == NULL)
        {
            return NULL;
        }
        
        else
        {
            return next->next;
        }
        
        num_blocks++;
        new->size = mem_left;
        new->free = true;
        //num_blocks++;
        
    }
    
    /* Could not find free _block, so grow heap */
   if (next == NULL)
   {
      next = growHeap(last, size);
       
       num_blocks++;
       //num_grows++;
   }
    
   

   /* Could not find free _block or grow heap, so just return NULL */
   if (next == NULL)
   {
      return NULL;
   }
   
   /* Mark _block as in use */
   next->free = false;

   /* Return data address associated with _block */
   return BLOCK_DATA(next);
}

void *calloc(size_t nmemb, size_t size)
{
    while(nmemb != 0 && size !=0)
    {
        struct _block *calloc_ptr = malloc(nmemb*size);
        memset(calloc_ptr,0,sizeof(calloc_ptr));
        return calloc_ptr;
        
        
    }
    
    return NULL;
}


void *realloc(void *ptr, size_t size)
{
    while(size != 0 )
    {
        struct _block *relloc_ptr = malloc(size);
        memcpy(relloc_ptr, ptr ,size );
        free(ptr);
        return relloc_ptr;
    }
    
    return NULL;
    
}
/*
 * \brief free
 *
 * frees the memory _block pointed to by pointer. if the _block is adjacent
 * to another _block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void free(void *ptr)
{
   if (ptr == NULL)
   {
      return;
   }
    
    num_frees++;

   /* Make _block as free */
   struct _block *curr = BLOCK_HEADER(ptr);
   assert(curr->free == 0);
   curr->free = true;
    
   

   /* TODO: Coalesce free _blocks if needed */
    
    curr = freeList;
    while ( curr )
    {
         struct _block *next_block = curr->next;
        if( curr && next_block && curr->free && next_block->free)
        {
            num_blocks--;
            num_coalesces++;
            curr->size += next_block->size; //Need to increase the block size. adding the current block size and the next block size.
           // printf("It works!");
            curr->next = next_block->next; //final step in joining two blocks. assignning the nect pointer to the next_block one
        }
        curr = curr->next; //Moving to the next free block.
    }
     
    
    
    
}

/* vim: set expandtab sts=3 sw=3 ts=6 ft=cpp: --------------------------------*/
