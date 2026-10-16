#include <kernel.h> 
#include <shellsvc.h>
#include <ipmi.h>

#include "music_internal.h"

extern "C" {

extern bool g_isInitialized;
extern bool g_isReady;
extern SceMusicCoreServiceParams g_srvParam;

extern int sceAppMgrSetBgmProxyApp(const char*);

int32_t sceMusicInternalAppInitialize()
{
	int32_t ret;
	uint32_t initType = SCE_MUSIC_INIT_TYPE_INTERNAL_APPLICATION;

	if (g_isInitialized)
	{
		return SCE_MUSIC_ERROR_ALREADY_INITIALIZED;
	}

	if (initType != 0 && initType != 1)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	if (initType == 0)
	{
		sceAppMgrSetBgmProxyApp("NPXS19999");
	}

	g_srvParam.unk_00 = 0;
	g_srvParam.initType = initType;
	g_srvParam.unk_08 = 0;

	ret = _sceMusicCoreSendMsgEmpty(0xD0000);
	if (ret != SCE_OK)
	{
		return ret;
	}

	g_isInitialized = true;
	g_isReady = false;

	return ret;
}

int32_t sceMusicInternalAppTerminate()
{
	int32_t ret;

	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	ret = _sceMusicCoreSendMsgEmpty(0xD0001);
	if (ret != SCE_OK)
	{
		return ret;
	}

	g_srvParam.unk_00 = 0;
	g_srvParam.initType = 0;
	g_srvParam.unk_08 = 0;

	g_isInitialized = false;
	g_isReady = false;

	return ret;
}

int32_t sceMusicInternalAppSetUri(const char* path, SceMusicOpt* optParams)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (path == NULL)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
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

	IPMI::DataInfo data[3];
	data[0].data = &g_srvParam;
	data[0].dataSize = sizeof(SceMusicCoreServiceParams);
	data[1].data = (ScePVoid)path;
	data[1].dataSize = sceClibStrnlen(path, SCE_MUSIC_MAX_STRING_LENGTH);
	data[2].data = &opt;
	data[2].dataSize = sizeof(SceMusicOpt);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0xD0002, data, 3, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	g_isReady = true;

	return res;
}

int32_t sceMusicInternalAppGetUri(char *dst, uint32_t maxlength)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (dst == NULL)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	if (maxlength < SCE_MUSIC_MAX_STRING_LENGTH)
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
	int32_t ret = shellService->invokeSyncMethod(0xD0003, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicInternalAppSetPlaybackCommand(uint32_t command, uint32_t fadeinFactor)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (!g_isReady)
	{
		return SCE_MUSIC_ERROR_NOT_READY;
	}

	if (command > SCE_MUSIC_COMMAND_SEEK)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg64(0xD0004, command, fadeinFactor);
}

int32_t sceMusicInternalAppGetStatus(SceMusicStatus *status)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (status == NULL)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	IPMI::BufferInfo rdata;
	rdata.data = status;
	rdata.dataSize = sizeof(SceMusicStatus);
	rdata.dataWriteSize = 0;

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(0xD0005, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicInternalAppSetVolume(uint32_t volume)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (volume > SCE_MUSIC_MAX_VOLUME)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg32(0xD0006, volume);
}

int32_t sceMusicInternalAppSetRepeatMode(uint32_t mode)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (mode != SCE_MUSIC_REPEAT_MODE_ALL &&
		mode != SCE_MUSIC_REPEAT_MODE_ONE &&
		mode != SCE_MUSIC_REPEAT_MODE_DISABLE)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg8(0xD0007, mode);
}

int32_t sceMusicInternalAppSetShuffleMode(uint32_t mode)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (mode != SCE_MUSIC_SHUFFLE_DISABLE &&
		mode != SCE_MUSIC_SHUFFLE_ENABLE)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg8(0xD0008, mode);
}

int32_t sceMusicInternalAppSetEQMode(uint32_t mode)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (mode > SCE_MUSIC_EQ_UNIQUE)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg8(0xD0009, mode);
}

int32_t sceMusicInternalAppSetALCMode(uint32_t mode)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (mode != SCE_MUSIC_ALC_DISABLE &&
		mode != SCE_MUSIC_ALC_ENABLE)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
	}

	return _sceMusicCoreSendMsg8(0xD000A, mode);
}

int32_t sceMusicInternalAppSetSegment(int32_t segmentStartMs, int32_t fromMs, int32_t toMs)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
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
	int32_t ret = shellService->invokeSyncMethod(0xD000B, data, 2, &res, 0, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicInternalAppGetInternalError(SceMusicInternalError *errors)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
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
	int32_t ret = shellService->invokeSyncMethod(0xD000C, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t sceMusicInternalAppSetUriExt(const char* path, SceMusicOpt* optParams, uint16_t extArg)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
	}

	if (path == NULL)
	{
		return SCE_MUSIC_ERROR_INVALID_ARG;
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
	int32_t ret = shellService->invokeSyncMethod(0xD000D, data, 4, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	g_isReady = true;

	return res;
}

int32_t sceMusicInternalAppGetInternalError2(SceMusicInternalError *errors)
{
	if (!g_isInitialized)
	{
		return SCE_MUSIC_ERROR_NOT_INITIALIZED;
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
	int32_t ret = shellService->invokeSyncMethod(0xD000E, &data, 1, &res, &rdata, 1);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

}