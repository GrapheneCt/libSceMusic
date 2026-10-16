#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SCE_MUSIC_PRX)
#define PRX_INTERFACE __declspec (dllexport)
#else
#define PRX_INTERFACE
#endif

#define SCE_MUSIC_ERROR_INVALID_ARG						0x80101900
#define SCE_MUSIC_ERROR_NOT_INITIALIZED					0x80101901
#define SCE_MUSIC_ERROR_ALREADY_INITIALIZED				0x80101902
#define SCE_MUSIC_ERROR_NOT_READY						0x80101903

#define SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG						0x80100E00
#define SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED					0x80100E01
#define SCE_MUSIC_PLAYER_SERVICE_ERROR_ALREADY_INITIALIZED				0x80100E02

#define SCE_MUSIC_MAX_STRING_LENGTH				0x401
#define SCE_MUSIC_MAX_QUERY_LENGTH				0x801
#define SCE_MUSIC_MAX_VOLUME					0x8000

#define SCE_MUSIC_PLAYER_SERVICE_INIT_TYPE_0	0
#define SCE_MUSIC_PLAYER_SERVICE_INIT_TYPE_1	1
#define SCE_MUSIC_PLAYER_SERVICE_INIT_TYPE_2	2
#define SCE_MUSIC_PLAYER_SERVICE_INIT_TYPE_3	3
#define SCE_MUSIC_PLAYER_SERVICE_INIT_TYPE_4	4

typedef struct SceMusicOpt {
	int32_t unk_00; // usually set to -1
	uint32_t unk_04;
	uint32_t unk_08;
	uint32_t unk_0C;
} SceMusicOpt;

typedef struct SceMusicStatus {
	uint8_t unk_00[0x24];
	uint32_t currentTimeMs;
	uint8_t unk_28[0x18];
} SceMusicStatus;

typedef struct SceMusicInternalError {
	int32_t ec1;
	int32_t ec2;
} SceMusicInternalError;

typedef struct SceMusicPlayerServicePlayStatus {
	int32_t unk_00;
	int32_t internalError;
	int32_t unk_08;
} SceMusicPlayerServicePlayStatus;

typedef struct SceMusicPlayerServicePlayStatusExtension {
	int32_t unk_00;
	int32_t unk_04;
	int32_t unk_08;
	int32_t unk_0C;
	int32_t currentState;		// One of ::SceMusicCommand
	int32_t unk_14;
	int32_t bgmPortPriority;
	int32_t unk_1C;
	int32_t unk_20;
	int32_t unk_24;
	int32_t unk_28;
	uint32_t currentTimeMs;
	uint32_t durationMs;
	uint32_t unk_34;
	uint32_t unk_38;
	uint32_t volume;
	uint32_t shuffleMode;		// One of ::SceMusicShuffleMode
	uint32_t repeatMode;		// One of ::SceMusicRepeatMode
	uint32_t eqMode;			// One of ::SceMusicEQMode
	uint32_t alcMode;			// One of ::SceMusicALCMode
	char unk_50[0x20];
} SceMusicPlayerServicePlayStatusExtension;

typedef struct SceMusicPlayerServiceTrackInfo {
	int32_t unk_00;
	int32_t unk_04;
	uint32_t durationMs;
	char title[0x400];
	char unk_40C[0x400];
	char album[0x400];
	char unk_C0C[0x400];
	char artist[0x400];
	char unk_140C[0x400];
	int32_t unk_180C;
} SceMusicPlayerServiceTrackInfo;

typedef enum SceMusicCommand {
	SCE_MUSIC_COMMAND_DEFAULT = 0,
	SCE_MUSIC_COMMAND_PLAY = 0x1,
	SCE_MUSIC_COMMAND_STOP = 0x2,
	SCE_MUSIC_COMMAND_NEXT = 0x3,
	SCE_MUSIC_COMMAND_PREVIOUS = 0x4,
	SCE_MUSIC_COMMAND_SEEK = 0x11
} SceMusicCommand;

typedef enum SceMusicRepeatMode {
	SCE_MUSIC_REPEAT_MODE_ALL,
	SCE_MUSIC_REPEAT_MODE_ONE,
	SCE_MUSIC_REPEAT_MODE_DISABLE
} SceMusicRepeatMode;

typedef enum SceMusicEQMode {
	SCE_MUSIC_EQ_DISABLE,
	SCE_MUSIC_EQ_HEAVY,
	SCE_MUSIC_EQ_POP,
	SCE_MUSIC_EQ_JAZZ,
	SCE_MUSIC_EQ_UNIQUE
} SceMusicEQMode;

typedef enum SceMusicALCMode {
	SCE_MUSIC_ALC_DISABLE,
	SCE_MUSIC_ALC_ENABLE
} SceMusicALCMode;

typedef enum SceMusicShuffleMode {
	SCE_MUSIC_SHUFFLE_DISABLE,
	SCE_MUSIC_SHUFFLE_ENABLE,
	SCE_MUSIC_SHUFFLE_UNK_2
} SceMusicShuffleMode;

PRX_INTERFACE int32_t sceMusicInternalAppInitialize();
PRX_INTERFACE int32_t sceMusicInternalAppTerminate();
PRX_INTERFACE int32_t sceMusicInternalAppSetUri(const char *path, SceMusicOpt *optParams);
PRX_INTERFACE int32_t sceMusicInternalAppGetUri(char *dst, uint32_t maxlength);
PRX_INTERFACE int32_t sceMusicInternalAppSetPlaybackCommand(uint32_t command, uint32_t fadeinFactor);
PRX_INTERFACE int32_t sceMusicInternalAppGetStatus(SceMusicStatus *status);
PRX_INTERFACE int32_t sceMusicInternalAppSetVolume(uint32_t volume);
PRX_INTERFACE int32_t sceMusicInternalAppSetRepeatMode(uint32_t mode);
PRX_INTERFACE int32_t sceMusicInternalAppSetShuffleMode(uint32_t mode);
PRX_INTERFACE int32_t sceMusicInternalAppSetEQMode(uint32_t mode);
PRX_INTERFACE int32_t sceMusicInternalAppSetALCMode(uint32_t mode);
PRX_INTERFACE int32_t sceMusicInternalAppSetSegment(int32_t segmentStartMs, int32_t fromMs, int32_t toMs);
PRX_INTERFACE int32_t sceMusicInternalAppGetInternalError(SceMusicInternalError *errors);
PRX_INTERFACE int32_t sceMusicInternalAppSetUriExt(const char *path, SceMusicOpt *optParams, uint16_t extArg);
PRX_INTERFACE int32_t sceMusicInternalAppGetInternalError2(SceMusicInternalError *errors);

PRX_INTERFACE int32_t sceMusicPlayerServiceInitialize(uint32_t initType);
PRX_INTERFACE int32_t sceMusicPlayerServiceTerminate();
// SELECT mrid FROM tbl_Music WHERE license_type = 0 AND album_name_for_sort = X'FF' ORDER BY track_num , title_for_sort ASC
PRX_INTERFACE int32_t sceMusicPlayerServiceSetDbQuery(const char *query);
PRX_INTERFACE int32_t sceMusicPlayerServiceGetDbQuery(char *dst, uint32_t maxlength);
PRX_INTERFACE int32_t sceMusicPlayerServiceGetUri(char *dst, uint32_t maxlength);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetUri(const char *path, SceMusicOpt *optParams);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetPlaybackCommand(uint32_t command, uint32_t fadeinFactor);
PRX_INTERFACE int32_t sceMusicPlayerServiceGetPlayStatusExtension(SceMusicPlayerServicePlayStatusExtension* infoBuffer);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetVolume(uint32_t volume);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetRepeatMode(uint32_t mode);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetShuffleMode(uint32_t mode);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetSeekTime(uint32_t seekTimeMs);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetSegment(int32_t segmentStartMs, int32_t fromMs, int32_t toMs);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetALCMode(uint32_t mode);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetEQMode(uint32_t mode);
PRX_INTERFACE int32_t sceMusicPlayerServiceGetInternalError(SceMusicInternalError *errors);
PRX_INTERFACE int32_t sceMusicPlayerServiceForbid(uint32_t param);
PRX_INTERFACE int32_t sceMusicPlayerServicePermit();
PRX_INTERFACE int32_t sceMusicPlayerServiceGetForbidStatus(uint32_t *status);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetDbQueryExt(const char *query, uint16_t extArg);
PRX_INTERFACE int32_t sceMusicPlayerServiceSetUriExt(const char *path, SceMusicOpt *optParams, uint16_t extArg);
PRX_INTERFACE int32_t sceMusicPlayerServiceGetPlayStatus(SceMusicPlayerServicePlayStatus *status);
PRX_INTERFACE int32_t sceMusicPlayerServiceGetTrackInfo(SceMusicPlayerServiceTrackInfo *infoBuffer);

PRX_INTERFACE int32_t sceMusicPlayerServiceLock();
PRX_INTERFACE int32_t sceMusicPlayerServiceUnlock();

#ifdef __cplusplus
}
#endif