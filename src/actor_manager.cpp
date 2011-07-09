#include "actor_manager.h"

#define EVENT_BUFFER_INIT 20

extern bool quit_threads;

cActor_manager* event_listener = NULL;

/* Functions for mantaining order in the priority queue */

bool min_actor(void* actor1, void* actor2) {
    if ( ((cActor* ) actor1)->priority > ((cActor* ) actor2)->priority ) {
        return true;
    }
    return false;
}


bool max_actor(void* actor1, void* actor2) {
    if ( ((cActor* ) actor1)->priority < ((cActor* ) actor2)->priority ) {
        return true;
    }
    return false;
}

template <class T>
T priority_stack<T>::priority_stack(int init_size) {
    levels = new vector < list<T> >(2);
    levels[0] = new list<T>(init_size);
    levels[1] = new list<T>(init_size);

    lvl_it = NULL;
    walk_it = NULL;
    return;
}

void reg_accessor(int (*foo) (T*)) {
    get_priority = foo;
    return;
}

void T priority_stack<T>::insert(T* obj) {
    int priority = get_priority(obj);
    levels[priority]->push_back(obj);
    return;
}

void T priority_stack<T>::insert(T* obj, int priority) {
    levels[priority]->push_back(obj);
    return;
}

T* T priority_stack<T>::remove(T* obj) {
    int priority = get_priority(obj);
    levels[priority]->remove(obj);
    return;
}

T* T priority_stack<T>::remove(T* obj, int priority) {
    levels[priority]->remove(obj);
    return;
}

T* T priority_stack<T>::walk() {
    if ( lvl_it == NULL ) {
        lvl_it = &(levels->begin());
        walk_it = &(*it->begin());
    } else if ( walk_it == NULL ) {
        walk_it = &(*(++lvl_it)->begin());
    }
}

/*-----------------------------------------------*/

cActor_manager::cActor_manager(cScreen_manager* _SM) {
    AM_thread = NULL;
    SM = _SM;
    cID_dispatch* actor_id_manager = new cID_dispatch();

    Draw_Buffer = NULL;

    int i;
    for (i = 0; i < SDL_NUMEVENTS; ++i) {
        Event_Buffer[i] = new event_vector (EVENT_BUFFER_INIT);
        event_buf_load[i] = -1;
    }

    actor_objs = new_pqueue(100, true, min_actor);
    event_listener = this;

    return;
}

void cActor_manager::AM_register(cActor* obj) {
    obj->ID = actor_id_manager->ID_getid();

    pq_insert(actor_objs, obj);

    vector<SDL_EventType>* events = obj->event_binds();
    Uint16 i;
    for (i = 0; i < events->size(); ++i) {
        if ( event_buf_load[events->at(i)] == -1) {
            /* Signals that we want to collect events of this type */
            event_buf_load[events->at(i)] = 0;
        }
    }

    return;
}

void cActor_manager::AM_blit_buffer(int x, int y, SDL_Surface* src, SDL_Rect* clip) {
    apply_surface(x,y,src,Draw_Buffer,clip);
    return;
}

void cActor_manager::AM_blit_buffer(sDisplay_info* sdi) {
    apply_surface(sdi->x, sdi->y, sdi->surf, Draw_Buffer, sdi->clip);
    return;
}

void cActor_manager::AM_flip_buffer() {
    printf("Flipping the Actor Manager Buffer\n");
    SM->SM_blit(0,0,Draw_Buffer,NULL);
    return;
}

void AM_input_events(SDL_Event* event) {
    //printf("Actor Manager recieved event of type: %d\n",event->type);
    cActor_manager* _AM = event_listener;
    int type = event->type;

    if ( _AM->event_buf_load[type] == -1) return;

    _AM->Event_Buffer[type]->at( _AM->event_buf_load[type]++ ) = event;
    return;
}

void cActor_manager::AM_update() {
    cActor* actor_update = NULL;
    sDisplay_info* actor_info;

    /* Send out Event Updates */
    printf("SM1\n");
    actor_update = (cActor*)pq_top( actor_objs );
    //while ( (actor_update = (cActor*)pq_top( actor_objs )) != NULL ) {
        printf("SM1.5\n");
        printf("Address of actor_update: %p\n", actor_update);
        actor_update->check_events(Event_Buffer, event_buf_load);
    //}

    /*------------------------*/
    printf("SM2\n");
    /* Check which objects need re-blitting */
    //while ( (actor_update = (cActor* ) pq_top( actor_objs )) != NULL ) {
        if ( actor_update->check() ) {
            actor_info = actor_update->get_display();
            AM_blit_buffer(actor_info);
        }
    //}

    AM_flip_buffer();
    /*--------------------------------------*/

    return;
}
