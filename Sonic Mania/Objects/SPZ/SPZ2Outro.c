// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: SPZ2Outro Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "SonicMania.h"

ObjectSPZ2Outro *SPZ2Outro;

void SPZ2Outro_Update(void)
{
    RSDK_THIS(SPZ2Outro);
    if (!self->activated) {
        SPZ2Outro_StartCutscene();
        self->activated = true;
    }
}

void SPZ2Outro_LateUpdate(void) {}

void SPZ2Outro_StaticUpdate(void) {}

void SPZ2Outro_Draw(void) {}

void SPZ2Outro_Create(void *data)
{
    RSDK_THIS(SPZ2Outro);

    INIT_ENTITY(self);
    CutsceneRules_SetupEntity(self, &self->size, &self->hitbox);
    self->active = ACTIVE_NEVER;
}

void SPZ2Outro_StageLoad(void)
{
    SPZ2Outro->field_8 = 0;

    SPZ2Outro->weatherTV = NULL;
    foreach_all(TVFlyingBattery, tvFlyingBattery)
    {
        SPZ2Outro->tvFlyingBattery = (Entity *)tvFlyingBattery;
        foreach_break;
    }

    SPZ2Outro->weatherTV = NULL;
    foreach_all(WeatherTV, weatherTV)
    {
        SPZ2Outro->weatherTV = (Entity *)weatherTV;
        foreach_break;
    }
}

void SPZ2Outro_StartCutscene(void)
{
    RSDK_THIS(SPZ2Outro);

    CutsceneSeq_StartSequence(self, SPZ2Outro_Cutscene_SetupFBZTV, SPZ2Outro_Cutscene_ExitStageRight, SPZ2Outro_Cutscene_AsSeenOnTV,
                              SPZ2Outro_Cutscene_FBZFlyAway, StateMachine_None);
#if RETRO_USE_PLUS
    if (RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->objectID)
        RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->skipType = SKIPTYPE_RELOADSCN;
#endif

    foreach_active(HUD, hud) { hud->state = HUD_State_GoOffScreen; }
}

bool32 SPZ2Outro_Cutscene_SetupFBZTV(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    EntityTVFlyingBattery *tvFlyingBattery = (EntityTVFlyingBattery *)SPZ2Outro->tvFlyingBattery;
    EntityWeatherTV *weatherTV             = (EntityWeatherTV *)SPZ2Outro->weatherTV;

    if (!host->timer) {
        foreach_all(EggPrison, prisonPtr)
        {
            SPZ2Outro->prison = prisonPtr;
            foreach_break;
        }

        EntityEggPrison *prison     = SPZ2Outro->prison;
        prison->notSolid            = true;
        prison->drawOrder           = Zone->playerDrawHigh;
        tvFlyingBattery->position.y = 0x81E0000;
        tvFlyingBattery->active     = ACTIVE_NORMAL;
        weatherTV->stateDraw        = WeatherTV_StateDraw_Unknown14;
        weatherTV->timer2           = 0;
        weatherTV->state            = WeatherTV_State_Outro;
        RSDK.AddDrawListRef(Zone->drawOrderHigh, RSDK.GetEntityID(weatherTV));
        WeatherTV->useHighLayer = true;
        camera->targetPtr       = NULL;
        player1->camera         = NULL;
#if RETRO_USE_PLUS
        Player->disableP2KeyCheck = true;
#endif
        CutsceneSeq_LockAllPlayerControl();
        player1->stateInput = StateMachine_None;
        player1->jumpPress  = false;
        player1->jumpHold   = false;
        player1->up         = false;
        player1->down       = false;
        player1->left       = false;
        player1->right      = false;
        if (player2->objectID == Player->objectID) {
            player2->stateInput = StateMachine_None;
            player2->jumpPress  = false;
            player2->jumpHold   = false;
            player2->up         = false;
            player2->down       = false;
            player2->left       = false;
            player2->right      = false;
        }
    }

    if (player2->objectID == Player->objectID) {
        player2->stateInput = StateMachine_None;
        player2->jumpPress  = false;
        player2->jumpHold   = false;
        player2->down       = false;
        player2->left       = false;
        player2->right      = false;
    }

    if (host->timer == 15) {
        RSDK.SetSpriteAnimation(player1->aniFrames, ANI_LOOKUP, &player1->animator, true, 0);
        player1->state     = Player_State_LookUp;
        player1->up        = true;
        player1->direction = player1->position.x > weatherTV->position.x;
        if (player2->objectID == Player->objectID) {
            RSDK.SetSpriteAnimation(player2->aniFrames, ANI_LOOKUP, &player2->animator, true, 0);
            player2->state     = Player_State_LookUp;
            player2->up        = true;
            player2->direction = player2->position.x > weatherTV->position.x;
        }
    }

    if (host->timer == 120) {
        Zone->playerBoundActiveR[0] = false;
        if (!player2->onGround || player2->state == Player_State_FlyIn || player2->state == Player_State_JumpIn || player2->state == Player_State_None
            || player2->state == Player_State_StartJumpIn) {
            SPZ2Outro->flag = true;
        }
        else {
            Zone->playerBoundActiveR[1] = false;
        }
        return true;
    }
    return false;
}

bool32 SPZ2Outro_Cutscene_ExitStageRight(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(camera);

    if (!host->timer) {
        RSDK.SetSpriteAnimation(player1->aniFrames, 0, &player1->animator, true, 0);
        player1->state = Player_State_Ground;
        player1->up    = false;
        player1->right = true;
        if (player2) {
            if (player2->objectID == Player->objectID && !SPZ2Outro->flag) {
                RSDK.SetSpriteAnimation(player2->aniFrames, ANI_IDLE, &player2->animator, true, 0);
                player2->state      = Player_State_Ground;
                player2->up         = false;
                player2->stateInput = Player_ProcessP2Input_AI;
            }
        }
    }

    int posX = ((ScreenInfo->width + ScreenInfo->position.x) << 16) + 0x100000;
    if (player1->position.x > posX) {
        player1->right = false;
        if (player2->objectID != Player->objectID || SPZ2Outro->flag || player2->position.x > posX) {
            EntityShield *shield = RSDK_GET_ENTITY(player1->playerID + Player->playerCount, Shield);
            if (shield->objectID == Shield->objectID) {
                player1->shield = SHIELD_NONE;
                destroyEntity(shield);
            }
            return true;
        }
    }
    return false;
}

void SPZ2Outro_DrawLayerCB(void)
{
    EntityWeatherTV *weatherTV = (EntityWeatherTV *)SPZ2Outro->weatherTV;
    int x                      = (weatherTV->position.x >> 16) - ScreenInfo->position.x;
    int y                      = (weatherTV->position.y >> 16) - ScreenInfo->position.y;
    RSDK.SetClipBounds(0, x - 96, y - 64, x + 96, y + 64);
}

// State where they are on da TV and jump onto FBZ
bool32 SPZ2Outro_Cutscene_AsSeenOnTV(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(camera);
    EntityTVFlyingBattery *tvFlyingBattery = (EntityTVFlyingBattery *)SPZ2Outro->tvFlyingBattery;
    EntityWeatherTV *weatherTV             = (EntityWeatherTV *)SPZ2Outro->weatherTV;

    if (!host->timer) {
        RSDK.SetDrawLayerProperties(Zone->playerDrawLow, false, SPZ2Outro_DrawLayerCB);
        RSDK.SetSpriteAnimation(player1->aniFrames, ANI_RUN, &player1->animator, true, 0);
        player1->drawOrder       = Zone->playerDrawLow;
        player1->state           = Player_State_None;
        player1->nextAirState    = StateMachine_None;
        player1->nextGroundState = StateMachine_None;
        player1->position.x      = weatherTV->position.x - 0x700000;
        player1->position.y      = weatherTV->position.y + 0x140000;
        host->storedTimer           = player1->position.y;
        player1->onGround        = false;
        player1->right           = false;
        player1->velocity.x      = 0x10000;
        player1->velocity.y      = 0;
        player1->scale.x         = 0x100;
        player1->scale.y         = 0x100;
        player1->drawFX          = FX_SCALE | FX_FLIP;
        if (player2) {
            if (player2->objectID == Player->objectID && !SPZ2Outro->flag) {
                RSDK.SetSpriteAnimation(player2->aniFrames, ANI_RUN, &player2->animator, true, 0);
                player2->drawOrder       = Zone->playerDrawLow;
                player2->state           = Player_State_None;
                player2->nextAirState    = StateMachine_None;
                player2->nextGroundState = StateMachine_None;
                player2->position.x      = player1->position.x - 0x100000;
                player2->position.y      = player1->position.y;
                player2->onGround        = false;
                player2->stateInput      = StateMachine_None;
                player2->right           = false;
                player2->velocity.x      = 0x10000;
                player2->velocity.y      = 0;
                player2->scale.x         = 0x100;
                player2->scale.y         = 0x100;
                player2->drawFX          = FX_SCALE | FX_FLIP;
            }
        }
    }

    EntityPlayer *players[2] = { player1, player2 };

    bool32 flag = true;
    for (int i = 0; i < 2; ++i) {
        if (i == 1 && (player2->objectID != Player->objectID || SPZ2Outro->flag)) {
            //do nothing, if P1 set the flag, then that'll count 
        }
        else {
            EntityPlayer *playerPtr = players[i];
            if (playerPtr->position.x >= weatherTV->position.x && !playerPtr->velocity.y && playerPtr->animator.animationID == ANI_RUN) {
                RSDK.SetSpriteAnimation(playerPtr->aniFrames, ANI_JUMP, &playerPtr->animator, true, 0);
                RSDK.PlaySfx(Player->sfxJump, false, 255);
                playerPtr->velocity.y = -0x20000;
            }
            if (playerPtr->position.y <= tvFlyingBattery->position.y - 0x80000 && playerPtr->animator.animationID == ANI_JUMP) {
                RSDK.PlaySfx(Player->sfxGrab, false, 255);
                RSDK.SetSpriteAnimation(playerPtr->aniFrames, ANI_CLING, &playerPtr->animator, true, 0);
                playerPtr->velocity.y = 0;
                playerPtr->direction  = FLIP_X;
            }

            if (playerPtr->animator.animationID == ANI_CLING)
                playerPtr->position.y = tvFlyingBattery->position.y - 0x80000;
            if (flag && playerPtr->animator.animationID == ANI_CLING) {
                flag = true;
            }
            else {
                flag = false;
            }
        }
    }

    return flag != false;
}

bool32 SPZ2Outro_Cutscene_FBZFlyAway(EntityCutsceneSeq *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(camera);
    EntityTVFlyingBattery *tvFlyingBattery = (EntityTVFlyingBattery *)SPZ2Outro->tvFlyingBattery;

    if (host->timer == 30)
        Zone_StartFadeOut(10, 0x000000);

    int volume = 0;
    if (host->timer - 30 < 0)
        volume = 0;
    else if (host->timer - 30 > 90)
        volume = -90;
    else if (host->timer - 30 >= 0)
        volume = 30 - host->timer;
    else
        volume = 0;

    RSDK.SetChannelAttributes(TVFlyingBattery->flyoverChannel, (volume + 90) / 90.0, 0.0, 1.0);
    if (player1->animator.animationID == ANI_CLING)
        player1->position.y = tvFlyingBattery->position.y - 0x80000;
    if (player2->objectID == Player->objectID && !SPZ2Outro->flag && player2->animator.animationID == ANI_CLING)
        player2->position.y = tvFlyingBattery->position.y - 0x80000;

    return false;
}

#if RETRO_INCLUDE_EDITOR
void SPZ2Outro_EditorDraw(void)
{
    RSDK_THIS(SPZ2Outro);
    if (showGizmos())
        CutsceneRules_DrawCutsceneBounds(self, &self->size);
}

void SPZ2Outro_EditorLoad(void) {}
#endif

void SPZ2Outro_Serialize(void) { RSDK_EDITABLE_VAR(SPZ2Outro, VAR_VECTOR2, size); }
