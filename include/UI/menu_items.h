#ifndef MENU_ITEMS_H_INCLUDED
#define MENU_ITEMS_H_INCLUDED

#include "physics.h"
#include "Audio/audio_manager.h"
#include "FastDelegate.h"
#include "FastDelegateBind.h"

using namespace fastdelegate;

typedef FastDelegate1<int&,void> std_clbck;
typedef SDL_Surface* surfp;

class menu_obj : public cActor {
    protected:
        /*
        surfp std;
        surfp hover;
        surfp clicked;
        */

        collision_zone click_box;
        std_clbck callback;
        vector<Uint8> _event_binds;

        /*
        bool click_state;
        bool hover_state;
        */

        virtual void call_clbck();
        virtual void change_img(surfp);
    public:
        virtual void set_ID(int);
        virtual void reg_callback(std_clbck);
        virtual void show();
        virtual void hide();

        /* Derived from cActor */
        virtual int set_priority(int);
        virtual sDisplay_info* get_display();
        virtual bool check();
        virtual void check_events(event_vector**, int* load, Uint8* key_states);
        virtual vector<Uint8>* event_binds();
};

class menu_button: public menu_obj {
    protected:
        sDisplay_info curr_info;

        SDL_Surface* std;
        SDL_Surface* hover;
        SDL_Surface* clicked;

        collision_zone click_box;

        std_clbck callback;
        std_fuse click_delay;

        bool click_state;
        bool hover_state;

        void call_clbck();
        void change_img(surfp);
    public:
        menu_button(int,int,SDL_Surface*,SDL_Surface*,SDL_Surface*);
        virtual ~menu_button();

        int set_priority(int);
        void set_ID(int);
        void set_typeID(int);
        void reg_callback(std_clbck);

        vector<Uint8>* event_binds();

        bool check();
        void check_events(event_vector** events, int* load, Uint8* key_states);
        sDisplay_info* get_display();
        void show();
        void hide();
};

class mute_button : public menu_button {
    private:
        bool muted;
        int saved_vol;
        void call_clbck();
        void change_img(surfp);
    public:
        mute_button(int x, int y, surfp std, surfp hover, surfp clicked);
        ~mute_button();
        int music_vol();
};

class menu_slider : public menu_obj {
    protected:
        surfp scale;
        surfp s_load;
        surfp slider;

        bool click_state;

    public:
        menu_slider(int x, int y, int w, surfp scale, surfp s_load, surfp slider);

        int set_priority(int);
        void set_ID(int);
        void set_typeID(int);
        void reg_callback(std_clbck);

        vector<Uint8>* event_binds();

        bool check();
        void check_events(event_vector** events, int* load, Uint8* key_states);
        sDisplay_info* get_display();
        void show();
        void hide();
};

void build_click_box(int x, int y, SDL_Surface* std, collision_zone& click_box);
void move_click_box(int x, int y, collision_zone& click_box);
#endif // MENU_ITEMS_H_INCLUDED
