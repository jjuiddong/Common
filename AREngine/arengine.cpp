
#include "stdafx.h"
#include "arengine.h"

#define CPARA_NAME "Data/camera_para.dat"
#define VPARA_NAME "Data/cameraSetting-%08x%08x.dat"

using namespace ar;

cAREngine::cAREngine()
	: m_CparamLT(NULL)
	, m_xsize(640)
	, m_ysize(480)
{

}

cAREngine::~cAREngine()
{
	arVideoCapStop();
 	argCleanup();
 	arPattDetach(m_arHandle);
	arPattDeleteHandle(m_arPattHandle);
	ar3DDeleteHandle(&m_ar3DHandle);
	arDeleteHandle(m_arHandle);
	arParamLTFree(&m_CparamLT);
	arVideoClose();
}


// ARToolkit 초기화
bool cAREngine::Init(int argc, char **argv)
{
	//glutInit(&argc, argv);

	char vconf[512];
	if (argc == 1)
	{
		vconf[0] = '\0';
	}
	else
	{
		strcpy_s(vconf, argv[1]);
		for (int i = 2; i < argc; i++)
		{
			strcat_s(vconf, " ");
			strcat_s(vconf, argv[i]);
		}
	}

	// open the video path
	ARLOGi("Using video configuration '%s'.\n", vconf);
	if (arVideoOpen(vconf) < 0)
		return false;

// 	if (arVideoGetSize(&xsize, &ysize) < 0) 
// 		return false;

	ARLOGi("Image size (x,y) = (%d,%d)\n", m_xsize, m_ysize);
	AR_PIXEL_FORMAT pixFormat;
	if ((pixFormat = arVideoGetPixelFormat()) < 0) 
		return false;

	ARUint32 id0, id1;
	if (arVideoGetId(&id0, &id1) == 0) {
		ARLOGi("Camera ID = (%08x, %08x)\n", id1, id0);
		sprintf_s(vconf, VPARA_NAME, id1, id0);
		if (arVideoLoadParam(vconf) < 0) {
			ARLOGe("No camera setting data!!\n");
		}
	}

	// set the initial camera parameters
	ARParam cparam;
	if (arParamLoad(CPARA_NAME, 1, &cparam) < 0) 
	{
		ARLOGe("Camera parameter load error !!\n");
		return false;
	}

	arParamChangeSize(&cparam, m_xsize, m_ysize, &cparam);
	ARLOG("*** Camera Parameter ***\n");
	arParamDisp(&cparam);
	if ((m_CparamLT = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL) 
	{
		ARLOGe("Error: arParamLTCreate.\n");
		return false;
	}

	if ((m_arHandle = arCreateHandle(m_CparamLT)) == NULL) 
	{
		ARLOGe("Error: arCreateHandle.\n");
		return false;
	}
	
	if (arSetPixelFormat(m_arHandle, pixFormat) < 0) 
	{
		ARLOGe("Error: arSetPixelFormat.\n");
		return false;
	}

	if ((m_ar3DHandle = ar3DCreateHandle(&cparam)) == NULL) 
	{
		ARLOGe("Error: ar3DCreateHandle.\n");
		return false;
	}

	if ((m_arPattHandle = arPattCreateHandle()) == NULL) 
	{
		ARLOGe("Error: arPattCreateHandle.\n");
		return false;
	}

	ARGViewport viewport;
	viewport.sx = 0;
	viewport.sy = 0;
	viewport.xsize = m_xsize;
	viewport.ysize = m_ysize;
	if ((m_vp = argCreateViewport(&viewport)) == NULL) 
		return false; 

	argViewportSetCparam(m_vp, &cparam);
	argViewportSetPixFormat(m_vp, pixFormat);
	//argViewportSetDispMethod( vp, AR_GL_DISP_METHOD_GL_DRAW_PIXELS );
	argViewportSetDistortionMode(m_vp, AR_GL_DISTORTION_COMPENSATE_DISABLE);

	arSetPatternDetectionMode(m_arHandle, AR_MATRIX_CODE_DETECTION);
	arSetMatrixCodeType(m_arHandle, AR_MATRIX_CODE_3x3);

	if (arVideoCapStart() != 0)
	{
		ARLOGe("video capture start error !!\n");
		return false;
	}

	return true;
}

