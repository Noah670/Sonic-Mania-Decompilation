#ifndef OBJ_PROJECTILE_H
#define OBJ_PROJECTILE_H

#include "../SonicMania.h"

// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectProjectile;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntityProjectile;

// Object Struct
extern ObjectProjectile *Projectile;

// Standard Entity Events
void Projectile_Update(void);
void Projectile_LateUpdate(void);
void Projectile_StaticUpdate(void);
void Projectile_Draw(void);
void Projectile_Create(void* data);
void Projectile_StageLoad(void);
void Projectile_EditorDraw(void);
void Projectile_EditorLoad(void);
void Projectile_Serialize(void);

// Extra Entity Functions


#endif //!OBJ_PROJECTILE_H