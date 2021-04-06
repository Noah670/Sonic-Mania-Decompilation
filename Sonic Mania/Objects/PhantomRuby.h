#ifndef OBJ_PHANTOMRUBY_H
#define OBJ_PHANTOMRUBY_H

#include "../SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    ushort aniFrames;
    ushort sfx_L[7];
    ushort sfx_R[7];
} ObjectPhantomRuby;

// Entity Class
typedef struct {
    RSDK_ENTITY
    void(*state)(void);
    Vector2 startPos;
    int timer;
    int flag;
    int field_6C;
    int sfx;
    int field_74;
    int field_78;
    AnimationData data1;
    AnimationData data2;
} EntityPhantomRuby;

// Object Struct
extern ObjectPhantomRuby *PhantomRuby;

// Standard Entity Events
void PhantomRuby_Update(void);
void PhantomRuby_LateUpdate(void);
void PhantomRuby_StaticUpdate(void);
void PhantomRuby_Draw(void);
void PhantomRuby_Create(void* data);
void PhantomRuby_StageLoad(void);
void PhantomRuby_EditorDraw(void);
void PhantomRuby_EditorLoad(void);
void PhantomRuby_Serialize(void);

// Extra Entity Functions
void PhantomRuby_PlaySFX(byte sfxID);
void PhantomRuby_Unknown2(EntityPhantomRuby *ruby);
void PhantomRuby_Unknown3(void);
void PhantomRuby_Unknown4(void);
void PhantomRuby_Unknown5(void);
void PhantomRuby_Unknown6(void);
void PhantomRuby_Unknown7(void);
void PhantomRuby_Unknown8(void);
void PhantomRuby_Unknown9(void);
void PhantomRuby_Unknown10(void);

#endif //!OBJ_PHANTOMRUBY_H
