#ifndef OBJ_HONKYTONK_H
#define OBJ_HONKYTONK_H

#include "../SonicMania.h"

// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectHonkytonk;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityHonkytonk;

// Object Struct
extern ObjectHonkytonk *Honkytonk;

// Standard Entity Events
void Honkytonk_Update(void);
void Honkytonk_LateUpdate(void);
void Honkytonk_StaticUpdate(void);
void Honkytonk_Draw(void);
void Honkytonk_Create(void* data);
void Honkytonk_StageLoad(void);
void Honkytonk_EditorDraw(void);
void Honkytonk_EditorLoad(void);
void Honkytonk_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_HONKYTONK_H