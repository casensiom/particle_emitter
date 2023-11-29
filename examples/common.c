#include "../particle_emitter.h"

void
link_list_unittest() {

    // move_first_to_empty
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        LinkedList *from = &a;
        LinkedList *to = NULL;

        linked_list_move_item(&from, &to);
        assert(to != NULL);
        assert(to == &a);
        assert(a.prev == NULL);
        assert(a.next == NULL);
        assert(from != NULL);
        assert(from == &b);
        assert(b.prev == NULL);
        assert(b.next == &c);
        assert(c.prev == &b);
        assert(c.next == NULL);
    }
    // move_mid_to_empty
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        LinkedList *from = &b;
        LinkedList *to = NULL;

        linked_list_move_item(&from, &to);
        assert(to != NULL);
        assert(to == &b);
        assert(b.prev == NULL);
        assert(b.next == NULL);
        assert(from != NULL);
        assert(from == &c);
        assert(a.prev == NULL);
        assert(a.next == &c);
        assert(c.prev == &a);
        assert(c.next == NULL);
    }

    // move_last_to_empty
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        LinkedList *from = &c;
        LinkedList *to = NULL;

        linked_list_move_item(&from, &to);
        assert(to != NULL);
        assert(to == &c);
        assert(c.prev == NULL);
        assert(c.next == NULL);
        assert(from == NULL);
        assert(a.prev == NULL);
        assert(a.next == &b);
        assert(b.prev == &a);
        assert(b.next == NULL);
    }


    // move_first_to_first
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        LinkedList d;
        LinkedList e;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        d.prev = NULL;
        d.next = &e;
        e.prev = &d;
        e.next = NULL;

        LinkedList *from = &a;
        LinkedList *to = &d;

        linked_list_move_item(&from, &to);
        assert(from != NULL);
        assert(from == &b);
        assert(b.prev == NULL);
        assert(b.next == &c);
        assert(c.prev == &b);
        assert(c.next == NULL);

        assert(to != NULL);
        assert(to == &a);
        assert(a.prev == NULL);
        assert(a.next == &d);
        assert(d.prev == &a);
        assert(d.next == &e);
        assert(e.prev == &d);
        assert(e.next == NULL);
    }



    // move_mid_to_mid
    {
        LinkedList a;
        LinkedList b;
        LinkedList c;
        LinkedList d;
        LinkedList e;
        a.prev = NULL;
        a.next = &b;
        b.prev = &a;
        b.next = &c;
        c.prev = &b;
        c.next = NULL;

        d.prev = NULL;
        d.next = &e;
        e.prev = &d;
        e.next = NULL;

        LinkedList *from = &b;
        LinkedList *to = &e;

        linked_list_move_item(&from, &to);
        assert(from != NULL);
        assert(from == &c);
        assert(a.prev == NULL);
        assert(a.next == &c);
        assert(c.prev == &a);
        assert(c.next == NULL);

        assert(to != NULL);
        assert(to == &b);
        assert(d.prev == NULL);
        assert(d.next == &b);
        assert(b.prev == &d);
        assert(b.next == &e);
        assert(e.prev == &b);
        assert(e.next == NULL);
    }
}
