#include "SonicMania.h"

#if RETRO_USE_PLUS
ObjectPBL_TargetBumper *PBL_TargetBumper;

void PBL_TargetBumper_Update(void)
{
    RSDK_THIS(PBL_TargetBumper);
    StateMachine_Run(self->state);
}

void PBL_TargetBumper_LateUpdate(void)
{
    RSDK_THIS(PBL_TargetBumper);
    int32 x = self->position.x;
    int32 y = self->height;
    int32 z = self->position.y;

    Matrix *mat = &PBL_Camera->matrix1;
    self->depth3D = mat->values[2][1] * (y >> 16) + mat->values[2][2] * (z >> 16) + mat->values[2][0] * (x >> 16) + mat->values[2][3];
    if (self->depth3D >= 0x4000) {
        int32 depth = ((mat->values[0][3] << 8) + ((mat->values[0][2] * (z >> 8)) & 0xFFFFFF00) + ((mat->values[0][0] * (x >> 8)) & 0xFFFFFF00)
                     + ((mat->values[0][1] * (self->height >> 8)) & 0xFFFFFF00));
        depth /= self->depth3D;
    }
}

void PBL_TargetBumper_StaticUpdate(void) {}

void PBL_TargetBumper_Draw(void)
{
    RSDK_THIS(PBL_TargetBumper);
    if (self->depth3D >= 0x4000) {
        uint32 colour = RSDK.GetPaletteEntry(0, (self->field_60 & 0xFF) - 80);
        RSDK.SetDiffuseColour(PBL_TargetBumper->sceneIndex, (colour >> 16) & 0xFF, (colour >> 8) & 0xFF, colour & 0xFF);
        RSDK.Prepare3DScene(PBL_TargetBumper->sceneIndex);
        RSDK.MatrixScaleXYZ(&self->matrix2, self->scale.x, self->scale.y, 256);
        RSDK.MatrixTranslateXYZ(&self->matrix2, self->position.x, self->height, self->position.y, false);
        RSDK.MatrixRotateY(&self->matrix3, self->angle);
        RSDK.MatrixMultiply(&self->matrix1, &self->matrix3, &self->matrix2);
        RSDK.MatrixMultiply(&self->matrix1, &self->matrix1, &PBL_Camera->matrix1);
        RSDK.MatrixMultiply(&self->matrix3, &self->matrix3, &PBL_Camera->matrix3);
        RSDK.AddModelTo3DScene(PBL_TargetBumper->modelFrames, PBL_TargetBumper->sceneIndex, PBL_TargetBumper->drawType, &self->matrix1,
                               &self->matrix3, 0xFFFFFF);
        RSDK.Draw3DScene(PBL_TargetBumper->sceneIndex);
    }
}

void PBL_TargetBumper_Create(void *data)
{
    RSDK_THIS(PBL_TargetBumper);
    if (!SceneInfo->inEditor) {
        self->visible       = true;
        self->drawOrder     = 4;
        self->active        = ACTIVE_BOUNDS;
        self->updateRange.x = 0x400000;
        self->updateRange.y = 0x400000;
        self->scale.x       = 0x100;
        self->scale.y       = 0x100;
        self->state         = PBL_TargetBumper_Unknown2;
        self->field_60      = 2 * (RSDK.GetEntityID(self) % 3);
        RSDK.SetModelAnimation(PBL_TargetBumper->modelFrames, &self->animator, 96, 0, true, 0);
    }
}

void PBL_TargetBumper_StageLoad(void)
{
    PBL_TargetBumper->modelFrames = RSDK.LoadMesh("Pinball/TargetBumper.bin", SCOPE_STAGE);
    PBL_TargetBumper->sceneIndex  = RSDK.Create3DScene("View:TargetBumper", 256u, SCOPE_STAGE);
    RSDK.SetDiffuseIntensity(PBL_TargetBumper->sceneIndex, 9, 9, 9);
    RSDK.SetSpecularIntensity(PBL_TargetBumper->sceneIndex, 15, 15, 15);
    PBL_TargetBumper->drawType        = S3D_FLATCLR_SHADED_BLENDED_SCREEN;
    PBL_TargetBumper->hitbox.left     = -16;
    PBL_TargetBumper->hitbox.top      = -6;
    PBL_TargetBumper->hitbox.right    = 12;
    PBL_TargetBumper->hitbox.bottom   = 6;
    PBL_TargetBumper->sfxTargetBumper = RSDK.GetSFX("Pinball/TargetBumper.wav");
    PBL_TargetBumper->sfxFlipper      = RSDK.GetSFX("Pinball/Flipper.wav");
}

void PBL_TargetBumper_HandlePlayerInteractions(void)
{
    RSDK_THIS(PBL_TargetBumper);
    if (self->scale.y >= 128) {
        int32 angle    = self->angle >> 2;
        int32 negAngle = -angle;

        Vector2 pivotPos;
        pivotPos.x = 0;
        pivotPos.y = 0;
        foreach_active(PBL_Player, player)
        {
            int32 distanceX = (player->position.x - self->position.x) >> 8;
            int32 distanceY = (player->position.y - self->position.y) >> 8;
            int32 posX      = player->position.x;
            int32 posY      = player->position.y;
            int32 velStoreX = player->velocity.x;
            int32 velStoreY = player->velocity.y;

            player->position.x = distanceY * RSDK.Sin256(angle) + distanceX * RSDK.Cos256(angle) + self->position.x;
            player->position.y = distanceY * RSDK.Cos256(angle) - distanceX * RSDK.Sin256(angle) + self->position.y;
            player->velocity.x = (player->velocity.y >> 8) * RSDK.Sin256(angle) + (player->velocity.x >> 8) * RSDK.Cos256(angle);
            player->velocity.y = (player->velocity.y >> 8) * RSDK.Cos256(angle) - (player->velocity.x >> 8) * RSDK.Sin256(angle);

            int32 velX = player->velocity.x;
            int32 velY = player->velocity.y;
            switch (RSDK.CheckObjectCollisionBox(self, &PBL_TargetBumper->hitbox, player, &PBL_Player->outerBox, true)) {
                case 0:
                    player->position.x = posX;
                    player->position.y = posY;
                    player->velocity.x = velStoreX;
                    player->velocity.y = velStoreY;
                    break;
                case 1:
                case 4:
                    player->onGround = false;
                    if (abs(velY) < 0x40000)
                        velY <<= 1;
                    player->velocity.y = -velY;
                    Zone_RotateOnPivot(&player->position, &self->position, negAngle);
                    Zone_RotateOnPivot(&player->velocity, &pivotPos, negAngle);
                    self->state      = PBL_TargetBumper_Unknown3;
                    self->velocity.y = -8;
                    PBL_Setup_GiveScore(1000);
                    RSDK.PlaySfx(PBL_TargetBumper->sfxTargetBumper, false, 255);
                    break;
                case 2:
                    if (velX < 0) {
                        player->velocity.x = velX;
                    }
                    else {
                        player->velocity.x = -(velX >> 1);
                        if (player->velocity.x <= -0x10000) {
                            if (player->velocity.x < -0x80000)
                                player->velocity.x = -0x80000;
                            player->velocity.y -= 0x20000;
                        }
                        else {
                            player->velocity.x = -0x10000;
                            player->velocity.y -= 0x20000;
                        }

                        Zone_RotateOnPivot(&player->position, &self->position, negAngle);
                        Zone_RotateOnPivot(&player->velocity, &pivotPos, negAngle);
                        player->onGround = false;
                    }
                    break;
                case 3:
                    if (velX > 0) {
                        player->velocity.x = velX;
                    }
                    else {
                        player->velocity.x = -(velX >> 1);
                        if (player->velocity.x >= 0x10000) {
                            if (player->velocity.x > 0x80000)
                                player->velocity.x = 0x80000;
                            player->velocity.y -= 0x20000;
                        }
                        else {
                            player->velocity.x = 0x10000;
                            player->velocity.y -= 0x20000;
                        }
                    }

                    Zone_RotateOnPivot(&player->position, &self->position, negAngle);
                    Zone_RotateOnPivot(&player->velocity, &pivotPos, negAngle);
                    player->onGround = false;
                    break;
                default: break;
            }
        }
    }
}

void PBL_TargetBumper_Unknown2(void) { PBL_TargetBumper_HandlePlayerInteractions(); }

void PBL_TargetBumper_Unknown3(void)
{
    RSDK_THIS(PBL_TargetBumper);
    self->velocity.y += 2;
    self->scale.y -= self->velocity.y;
    if (self->scale.y <= 0) {
        self->velocity.y = 0;
        self->scale.y    = 0;
        self->state      = StateMachine_None;
    }
}

void PBL_TargetBumper_Unknown4(void)
{
    RSDK_THIS(PBL_TargetBumper);
    self->velocity.y -= 2;
    self->scale.y += self->velocity.y;
    if (self->velocity.y < 0 && self->scale.y <= 0x100) {
        self->active     = ACTIVE_BOUNDS;
        self->velocity.y = 0;
        self->scale.y    = 256;
        self->state      = PBL_TargetBumper_Unknown2;
    }
}

#if RETRO_INCLUDE_EDITOR
void PBL_TargetBumper_EditorDraw(void) {}

void PBL_TargetBumper_EditorLoad(void) {}
#endif

void PBL_TargetBumper_Serialize(void) { RSDK_EDITABLE_VAR(PBL_TargetBumper, VAR_ENUM, angle); }
#endif
