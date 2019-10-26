#include "memcached.h"
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

void test_single_thread_fifo(void);

int main () {
    test_single_thread_fifo();
    return 0;
}

void test_single_thread_fifo() {
    for (int i = 0; i < 100000; i++) {
        char key[16];
        snprintf(key, sizeof key, "%s%d", "key", i);
        item *it = do_item_alloc(key, sizeof(key), 0, 0, 20);
        item *orig_head = get_heads()[it->slabs_clsid];
        item *next_orig_head = get_heads()[it->slabs_clsid]->next;
        do_item_link(it, i % 10);


        assert(get_heads()[it->slabs_clsid] == orig_head
                || get_heads()[it->slabs_clsid] == next_orig_head);
    }
}
