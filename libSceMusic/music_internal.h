#pragma once

#include "music.h"

#ifdef __cplusplus
extern "C" {
#endif

#define sceShellSvcGetSvcClient() reinterpret_cast<IPMI::Client*>(sceShellSvcGetSvcObj());

#define SCE_MUSIC_INIT_TYPE_INTERNAL_APPLICATION	0
#define SCE_MUSIC_INIT_TYPE_MUSIC_PLAYER			1

struct SceMusicCoreServiceParams {
	uint32_t unk_00;	// not used (set to 0)
	uint32_t initType;	// 0 is app, 1 is music player
	uint32_t unk_08;	// bgm priority?
};

struct SceMusicCoreMessage64 {
	uint32_t param1;
	uint32_t param2;
};

int32_t _sceMusicCoreSendMsgEmpty(uint32_t eventId);
int32_t _sceMusicCoreSendMsg64(uint32_t eventId, uint32_t param1, uint32_t param2);
int32_t _sceMusicCoreSendMsg32(uint32_t eventId, uint32_t param);
int32_t _sceMusicCoreSendMsg8(uint32_t eventId, uint8_t param);

#ifdef __cplusplus
}
#endif