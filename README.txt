Author: Jainul N Vaghasia (jnv3@uw.edu)

Write up for Phd Admissions application

1.
The way the caching is set up in this system is as follows:
    (a) There are doubly linkedlists of items such that items are added to the tail and evicted from the head in order to implement FIFO policy
    (b) These linkedlists are organized into several slabs (can be thought of as a block of cache lines)
    (c) The slabs offer some optimizations through rearrangements that were not explored in this work

Major changes made in order to replace LRU were to change the do_item_alloc(), item_link_q(), and do_item_update() methods.
    Previously, do_item_alloc() evicted from tail of the linkedlist or replaced expired items in the vicinity but now it evicts from the head.
    item_link_q() now links to the tail.
    do_item_update() does nothing now because we do not move around nodes anymore.
    Note that do_item_flush_expired() has undefined behavior right now.

(Extra Credit) Even though we did not run any tests, we know from past experience that FIFO usually performs better than LRU but has more cache
misses instead. This is because LRU maintains a cache_lock for rearranging nodes which FIFO does not need to. FIFO pays by usually evicting the wrong
kind of item.

2.
It should be noticable that the LRU policy is not scalable for the fact that it acquires global cache_locks in order to rearrange
the list. We can instead use a granular locking policy with a log for LRU rearrangements when reading
(writing can still use a global lock, nobody likes writing). Rearrangements can then eventually happen. The key idea here is to reduce
critical section of global locks as much as possible.

The pipeline would look like

    Read(x) -----> Return(Value) ---> Record Log ----> Perform in batches

If the log becomes too large, acquire a global cache_lock and truncate it. We can even have multiple logs if we face high contention writing
to the log. This is all pretty generic in database literature.

Perhaps the similar methods mentioned in part 1 will have to be changed to allow for the log. When do_item_update(), we need to ensure
that the log is the one being updated and not the cache directly.

One thing to definitely evaluate for is the hit that cache hit rate takes. It is possible for a write to evict a wrong item before rearrangements
from the log make it through. But as long as the log is truncated fairly regularly, this might not be a too large of a hit. Otherwise, normal benchmarking
and stress test are highly recommended. Perhaps a cache dump walk through might be helpful to make sure the additional buffers are all atomic.
