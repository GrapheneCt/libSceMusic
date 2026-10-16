#include <kernel.h> 
#include <shellsvc.h>
#include <ipmi.h>

#include "music_internal.h"

extern "C" {

bool g_isInitialized = false;
bool g_isReady = false;
SceMusicCoreServiceParams g_srvParam = { 0, 0, 0 };

int32_t _sceMusicCoreSendMsgEmpty(uint32_t eventId)
{
	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data;
	data.data = &g_srvParam;
	data.dataSize = sizeof(SceMusicCoreServiceParams);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(eventId, &data, 1, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t _sceMusicCoreSendMsg64(uint32_t eventId, uint32_t param1, uint32_t param2)
{
	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	SceMusicCoreMessage64 msg;
	msg.param1 = param1;
	msg.param2 = param2;

	IPMI::DataInfo data[2];
	data[0].data = &msg;
	data[0].dataSize = sizeof(SceMusicCoreMessage64);
	data[1].data = &g_srvParam;
	data[1].dataSize = sizeof(SceMusicCoreServiceParams);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(eventId, data, 2, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t _sceMusicCoreSendMsg32(uint32_t eventId, uint32_t param)
{
	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data[2];
	data[0].data = &param;
	data[0].dataSize = sizeof(param);
	data[1].data = &g_srvParam;
	data[1].dataSize = sizeof(SceMusicCoreServiceParams);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(eventId, data, 2, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int32_t _sceMusicCoreSendMsg8(uint32_t eventId, uint8_t param)
{
	IPMI::Client* shellService = sceShellSvcGetSvcClient();

	IPMI::DataInfo data[2];
	data[0].data = &param;
	data[0].dataSize = sizeof(param);
	data[1].data = &g_srvParam;
	data[1].dataSize = sizeof(SceMusicCoreServiceParams);

	int32_t res = SCE_OK;
	int32_t ret = shellService->invokeSyncMethod(eventId, data, 2, &res, NULL, 0);
	if (ret != SCE_OK)
	{
		res = ret;
	}

	return res;
}

int _sceLdTlsRegisterModuleInfo()
{
	return 0;
}

int _sceLdTlsUnregisterModuleInfo()
{
	return 0;
}

int __at_quick_exit(void(*func)(void), void *arg, void *dso_handle)
{
	return 0;
}

int __cxa_atexit(void(*func)(void), void *arg, void *dso_handle)
{
	return 0;
}

void __cxa_finalize(void * d)
{

}

}