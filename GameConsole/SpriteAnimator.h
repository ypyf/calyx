#ifndef __animator_h__
#define __animator_h__

#include <d3d9.h>

namespace calyx {
#define MAX_BOB_ANIMATIONS 100
    struct SpriteAnimator
    {
        int state;          // the state of the object (general)
        int anim_state;     // an animation state variable, up to you
        int attr;           // attributes pertaining
        // to the object (general)
        float x, y;            // position bitmap will be displayed at
        float xv, yv;          // velocity of object
        int width, height;  // the width and height of the bob
        int width_fill;     // internal, used to force 8*x wide surfaces
        int counter_1;      // general counters
        int counter_2;
        int max_count_1;    // general threshold values;
        int max_count_2;
        int varsI[16];      // stack of 16 integers
        float varsF[16];    // stack of 16 floats
        int curr_frame;     // current animation frame
        int num_frames;     // total number of animation frames
        int curr_animation; // index of current animation
        int anim_counter;   // used to time animation transitions
        int anim_index;     // animation element index
        int anim_count_max; // number of cycles before animation
        int* animations[MAX_BOB_ANIMATIONS]; // animation sequences

        IDirect3DTexture9** images; // the bitmap images
        // DD surfaces

    } BOB, * BOB_PTR;

}   // namespace calyx
#endif // __animator_h__