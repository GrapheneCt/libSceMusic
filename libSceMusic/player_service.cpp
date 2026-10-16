#include <kernel.h> 
#include <shellsvc.h>
#include <ipmi.h>

#include "music_internal.h"

extern "C" {

extern bool g_isInitialized;
extern bool g_isReady;
extern SceMusicCoreServiceParams g_srvParam;

static bool s_serviceLocked = false;

int32_t sceMusicPlayerServiceInitialize(uint32_t initType)
{
	int32_t ret;

	if (g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_ALREADY_INITIALIZED;
	}

	g_srvParam.unk_00 = 0;
	g_srvParam.initType = SCE_MUSIC_INIT_TYPE_MUSIC_PLAYER;
	g_srvParam.unk_08 = 0;

	switch (initType) {
	case 0:
		g_srvParam.unk_08 = 0x8;
		break;
	case 1:
		g_srvParam.unk_08 = 0x10;
		break;
	case 2:
		g_srvParam.unk_08 = 0x20;
		break;
	case 3:
		g_srvParam.unk_08 = 0x40;
		break;
	case 4:
		g_srvParam.unk_08 = 0x80;
		break;
	default:
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	ret = _sceMusicCoreSendMsgEmpty(0x30000);
	if (ret != SCE_OK)
	{
		return ret;
	}

	g_isInitialized = true;

	return ret;
}

int32_t sceMusicPlayerServiceTerminate()
{
	int32_t ret;

	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	ret = _sceMusicCoreSendMsgEmpty(0x30001);
	if (ret != SCE_OK)
	{
		return ret;
	}

	g_srvParam.unk_00 = 0;
	g_srvParam.initType = 0;
	g_srvParam.unk_08 = 0;

	g_isInitialized = false;

	return ret;
}

int32_t sceMusicPlayerServiceSetDbQuery(const char* query)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (query == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data[2];
	data[0].data = &g_srvParam;
	data[0].dataSize = sizeof(SceMusicCoreServiceParams);
	data[1].data = (ScePVoid)query;
	data[1].dataSize = sceClibStrnlen(query, SCE_MUSIC_MAX_QUERY_LENGTH);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30002, data, 2, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceGetDbQuery(char *dst, uint32_t maxlength)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (dst == NULL)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	if (maxlength < SCE_MUSIC_MAX_QUERY_LENGTH)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	IPMI::BufferInfo rdata;
	rdata.data = dst;
	rdata.dataSize = maxlength;
	rdata.dataWriteSize = 0;

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30003, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceSetUri(const char* path, SceMusicOpt* optParams)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (path == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	SceMusicOpt opt;
	sceClibMemset(&opt, 0, sizeof(SceMusicOpt));
	if (optParams != NULL)
	{
		sceClibMemcpy(&opt, optParams, sizeof(SceMusicOpt));
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data[3];
	data[0].data = &g_srvParam;
	data[0].dataSize = sizeof(SceMusicCoreServiceParams);
	data[1].data = (ScePVoid)path;
	data[1].dataSize = sceClibStrnlen(path, SCE_MUSIC_MAX_STRING_LENGTH);
	data[2].data = &opt;
	data[2].dataSize = sizeof(SceMusicOpt);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30004, data, 3, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceGetUri(char *dst, uint32_t maxlength)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (dst == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	if (maxlength < SCE_MUSIC_MAX_STRING_LENGTH)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	IPMI::BufferInfo rdata;
	rdata.data = dst;
	rdata.dataSize = maxlength;
	rdata.dataWriteSize = 0;

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30005, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceSetPlaybackCommand(uint32_t command, uint32_t fadeinFactor)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (command > SCE_MUSIC_COMMAND_SEEK)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg64(0x30006, command, fadeinFactor);
}

int32_t sceMusicPlayerServiceGetPlayStatusExtension(SceMusicPlayerServicePlayStatusExtension* infoBuffer)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (infoBuffer == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	IPMI::BufferInfo rdata;
	rdata.data = infoBuffer;
	rdata.dataSize = sizeof(SceMusicPlayerServicePlayStatusExtension);
	rdata.dataWriteSize = 0;

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30007, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	switch (infoBuffer->shuffleMode)
	{
	case 1:
		infoBuffer->shuffleMode = SCE_MUSIC_SHUFFLE_DISABLE;
		break;
	case 2:
		infoBuffer->shuffleMode = SCE_MUSIC_SHUFFLE_ENABLE;
		break;
	case 0:
		infoBuffer->shuffleMode = SCE_MUSIC_SHUFFLE_UNK_2;
		break;
	}

	if (infoBuffer->repeatMode == 3)
	{
		infoBuffer->repeatMode = SCE_MUSIC_REPEAT_MODE_DISABLE;
	}

	return res;
}

int32_t sceMusicPlayerServiceSetVolume(uint32_t volume)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (volume > SCE_MUSIC_MAX_VOLUME)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg32(0x30008, volume);
}

int32_t sceMusicPlayerServiceSetRepeatMode(uint32_t mode)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (mode != SCE_MUSIC_REPEAT_MODE_ALL &&
		mode != SCE_MUSIC_REPEAT_MODE_ONE &&
		mode != SCE_MUSIC_REPEAT_MODE_DISABLE)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	if (mode == SCE_MUSIC_REPEAT_MODE_DISABLE)
	{
		mode = 3;
	}

	return _sceMusicCoreSendMsg8(0x30009, mode);
}

int32_t sceMusicPlayerServiceSetShuffleMode(uint32_t mode)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (mode != SCE_MUSIC_SHUFFLE_DISABLE &&
		mode != SCE_MUSIC_SHUFFLE_ENABLE &&
		mode != SCE_MUSIC_SHUFFLE_UNK_2)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	switch (mode)
	{
	case SCE_MUSIC_SHUFFLE_DISABLE:
		mode = 1;
		break;
	case SCE_MUSIC_SHUFFLE_ENABLE:
		mode = 2;
		break;
	case SCE_MUSIC_SHUFFLE_UNK_2:
		mode = 0;
		break;
	}

	return _sceMusicCoreSendMsg8(0x3000A, mode);
}

int32_t sceMusicPlayerServiceSetSeekTime(uint32_t seekTimeMs)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	return _sceMusicCoreSendMsg32(0x3000B, seekTimeMs);
}

int32_t sceMusicPlayerServiceSetSegment(int32_t segmentStartMs, int32_t fromMs, int32_t toMs)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	int32_t params[3];
	params[0] = fromMs;
	params[1] = segmentStartMs;
	params[2] = toMs;

	IPMI::DataInfo data[2];
	data[0].data = params;
	data[0].dataSize = sizeof(params);
	data[1].data = &g_srvParam;
	data[1].dataSize = sizeof(SceMusicCoreServiceParams);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x3000E, data, 2, &res, 0, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceSetALCMode(uint32_t mode)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (mode > SCE_MUSIC_EQ_UNIQUE)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg8(0x3000F, mode);
}

int32_t sceMusicPlayerServiceSetEQMode(uint32_t mode)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (mode > SCE_MUSIC_EQ_UNIQUE)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg8(0x30010, mode);
}

int32_t sceMusicPlayerServiceGetInternalError(SceMusicInternalError *errors)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	IPMI::BufferInfo rdata;
	rdata.data = errors;
	rdata.dataSize = sizeof(SceMusicInternalError);
	rdata.dataWriteSize = 0;

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30011, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceForbid(uint32_t param)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	return _sceMusicCoreSendMsg8(0x30013, param);
}

int32_t sceMusicPlayerServicePermit()
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	return _sceMusicCoreSendMsgEmpty(0x30015);
}

int32_t sceMusicPlayerServiceGetForbidStatus(uint32_t *status)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (status == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	IPMI::BufferInfo rdata;
	rdata.data = status;
	rdata.dataSize = sizeof(uint32_t);
	rdata.dataWriteSize = 0;

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30017, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceSetDbQueryExt(const char* query, uint16_t extArg)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (query == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data[3];
	data[0].data = &extArg;
	data[0].dataSize = sizeof(uint16_t);
	data[1].data = &g_srvParam;
	data[1].dataSize = sizeof(SceMusicCoreServiceParams);
	data[2].data = (ScePVoid)query;
	data[2].dataSize = sceClibStrnlen(query, SCE_MUSIC_MAX_QUERY_LENGTH);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30018, data, 3, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceSetUriExt(const char* path, SceMusicOpt* optParams, uint16_t extArg)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (path == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	SceMusicOpt opt;
	sceClibMemset(&opt, 0, sizeof(SceMusicOpt));
	opt.unk_00 = -1;
	if (optParams != NULL)
	{
		if ((static_cast<uint8_t>(optParams->unk_04) != 0) &&
			(static_cast<uint8_t>(optParams->unk_04) != 1))
		{
			return SCE_MUSIC_ERROR_INVALID_ARG;
		}

		sceClibMemcpy(&opt, optParams, sizeof(SceMusicOpt));
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data[4];
	data[0].data = &extArg;
	data[0].dataSize = sizeof(uint16_t);
	data[1].data = &g_srvParam;
	data[1].dataSize = sizeof(SceMusicCoreServiceParams);
	data[2].data = (ScePVoid)path;
	data[2].dataSize = sceClibStrnlen(path, SCE_MUSIC_MAX_STRING_LENGTH);
	data[3].data = &opt;
	data[3].dataSize = sizeof(SceMusicOpt);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x30019, data, 4, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceGetPlayStatus(SceMusicPlayerServicePlayStatus *status)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (status == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	IPMI::BufferInfo rdata;
	rdata.data = status;
	rdata.dataSize = sizeof(SceMusicPlayerServicePlayStatus);
	rdata.dataWriteSize = 0;

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x3001A, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceGetTrackInfo(SceMusicPlayerServiceTrackInfo *infoBuffer)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_NOT_INITIALIZED;
	}

	if (infoBuffer == NULL)
	{
		return SCE_MUSIC_PLAYER_SERVICE_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	IPMI::BufferInfo rdata;
	rdata.data = infoBuffer;
	rdata.dataSize = sizeof(SceMusicPlayerServiceTrackInfo);
	rdata.dataWriteSize = 0;

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0x3001B, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicPlayerServiceLock()
{
	int32_t ret;
	uint32_t attempt = 0;

	if (!s_serviceLocked)
	{
		ret = sceMusicPlayerServiceForbid(1);
		if (ret == SCE_OK)
		{
			s_serviceLocked = true;
			do
			{
				uint32_t status = 0;
				ret = sceMusicPlayerServiceGetForbidStatus(&status);
				if ((ret != SCE_OK) || (status == 2)) break;
				sceKernelDelayThread(200000);
				attempt++;
			} while (attempt < 0x32);
		}
		else
		{
			return ret;
		}
	}
	
	return SCE_OK;
}

int32_t sceMusicPlayerServiceUnlock()
{
	int32_t ret;

	if (s_serviceLocked)
	{
		ret = sceMusicPlayerServicePermit();
		if (ret == SCE_OK)
		{
			s_serviceLocked = false;
		}
		else
		{
			return ret;
		}
	}

	return SCE_OK;
}

}