#include "WinD3DInterface.h"

#include <assert.h>
#include "Boy/Mouse.h"
#include "BoyLib/BoyUtil.h"
#include "d3dx9math.h"
#include "Environment.h"
#include <fstream>
#include "Game.h"
#include <iostream>
#include "Keyboard.h"
#include "PersistenceLayer.h"
#include "ResourceManager.h"
#include <SDL3/SDL.h>
#include "WinEnvironment.h"
#include "WinImage.h"
#include "WinTriStrip.h"

using namespace Boy;

#define PROJECTION_Z_NEAR 0
#define PROJECTION_Z_FAR 1

#define CAM_Z PROJECTION_Z_FAR

#define DEFAULT_D3DFORMAT D3DFMT_X8R8G8B8

#include "BoyLib/CrtDbgNew.h"

static int gAltDown = false;
static int gShiftDown = false;
static int gControlDown = false;

bool getPresentationParameters(D3DPRESENT_PARAMETERS &pp, int width, int height, bool windowed, unsigned int refreshRate, SDL_Window* window, IDirect3D9* d3dobject)
{
	pp.AutoDepthStencilFormat = D3DFMT_D16;
	pp.BackBufferCount = 3;
	pp.BackBufferFormat = (windowed ? D3DFMT_UNKNOWN : DEFAULT_D3DFORMAT);
	pp.BackBufferWidth = width;
	pp.BackBufferHeight = height;
	pp.EnableAutoDepthStencil = true;
	pp.Flags = 0;
	pp.FullScreen_RefreshRateInHz = refreshRate;
	pp.hDeviceWindow = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	pp.MultiSampleQuality = 0;
	pp.MultiSampleType = D3DMULTISAMPLE_NONE;
	pp.PresentationInterval = (windowed ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_DEFAULT);
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.Windowed = windowed;

	// find the appropriate refresh rate for the desired resolution:
	if (!windowed)
	{
		D3DDISPLAYMODE mode;
		D3DFORMAT format = pp.BackBufferFormat;
		int modeCount = d3dobject->GetAdapterModeCount(D3DADAPTER_DEFAULT,format);
		for (int i=0 ; i<modeCount ; i++)
		{
			d3dobject->EnumAdapterModes(D3DADAPTER_DEFAULT,format,i,&mode);
			if (mode.Width==pp.BackBufferWidth &&
				mode.Height==pp.BackBufferHeight)
			{
				if (d3dobject->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, format, format, FALSE) == D3D_OK)
				{
					pp.FullScreen_RefreshRateInHz = mode.RefreshRate;
				}
			}
		}
		if (pp.FullScreen_RefreshRateInHz == 0)
		{
			return false;
		}
	}

	return true;
}

Keyboard::Key getKey(UINT nChar)
{
	switch (nChar)
	{
	case VK_BACK:
		return Keyboard::KEY_BACKSPACE;
	case VK_TAB:
		return Keyboard::KEY_TAB;
	case VK_RETURN:
		return Keyboard::KEY_RETURN;
	case VK_SHIFT:
		return Keyboard::KEY_SHIFT;
	case VK_CONTROL:
		return Keyboard::KEY_CONTROL;
	case VK_PAUSE:
		return Keyboard::KEY_PAUSE;
	case VK_ESCAPE:
		return Keyboard::KEY_ESCAPE;
	case VK_END:
		return Keyboard::KEY_END;
	case VK_HOME:
		return Keyboard::KEY_HOME;
	case VK_LEFT:
		return Keyboard::KEY_LEFT;
	case VK_UP:
		return Keyboard::KEY_UP;
	case VK_RIGHT:
		return Keyboard::KEY_RIGHT;
	case VK_DOWN:
		return Keyboard::KEY_DOWN;
	case VK_INSERT:
		return Keyboard::KEY_INSERT;
	case VK_DELETE:
		return Keyboard::KEY_DELETE;
	case VK_F1:
		return Keyboard::KEY_F1;
	case VK_F2:
		return Keyboard::KEY_F2;
	case VK_F3:
		return Keyboard::KEY_F3;
	case VK_F4:
		return Keyboard::KEY_F4;
	case VK_F5:
		return Keyboard::KEY_F5;
	case VK_F6:
		return Keyboard::KEY_F6;
	case VK_F7:
		return Keyboard::KEY_F7;
	case VK_F8:
		return Keyboard::KEY_F8;
	case VK_F9:
		return Keyboard::KEY_F9;
	case VK_F10:
		return Keyboard::KEY_F10;
	case VK_F11:
		return Keyboard::KEY_F11;
	case VK_F12:
		return Keyboard::KEY_F12;
	}

	return Keyboard::KEY_UNKNOWN;
}

int getKeyMods()
{
	int mods = Keyboard::KEYMOD_NONE;
	if (gAltDown) mods |= Keyboard::KEYMOD_ALT;
	if (gShiftDown) mods |= Keyboard::KEYMOD_SHIFT;
	if (gControlDown) mods |= Keyboard::KEYMOD_CTRL;
	return mods;
}

WinD3DInterface::WinD3DInterface(Game *game, int width, int height, const char *title, bool windowed, unsigned int refreshRate)
{
	// reset some members:
	mRendering = false;

	// remember the game:
	mGame = game;
	
	// initialize:
	SDL_Init(SDL_INIT_VIDEO);

	// create window:
	mTitle = title;
	mWindow = SDL_CreateWindow(mTitle.c_str(), width, height, windowed ? SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY : SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY|SDL_WINDOW_FULLSCREEN);

	// initialize d3d:
	//mRenderer = SDL_CreateRenderer(mWindow, "direct3d");
	mD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS pp;
	SDL_DisplayMode DM;
	getPresentationParameters(pp, SDL_GetDesktopDisplayMode(1)->w, SDL_GetDesktopDisplayMode(1)->h, windowed, refreshRate, mWindow, mD3D9);
	
	mD3D9->CreateDevice(
		D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, 
		(HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(mWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL), 
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&pp,
		&mD3D9Device);
	initD3D();

	// create the vertex buffer to be used 
	// for drawing subrects of images:
	mVertexBuffer = createVertexBuffer(4);

	// clearing params:
	mClearZ = PROJECTION_Z_FAR;
	mClearColor = 0x00000000;
}

WinD3DInterface::~WinD3DInterface()
{
	mD3D9Device->Release();
	mD3D9->Release();
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

int WinD3DInterface::getWidth()
{
	int w;
	SDL_GetWindowSizeInPixels(mWindow, &w, NULL);
	return w;
}

int WinD3DInterface::getHeight()
{
	int h;
	SDL_GetWindowSizeInPixels(mWindow, NULL, &h);
	return h;
}

bool WinD3DInterface::isWindowed()
{
	return !(SDL_GetWindowFlags(mWindow) & SDL_WINDOW_FULLSCREEN);
}
/*
void WinD3DInterface::toggleFullScreen(bool toggle)
{
	// release the subrect vb:
	mVertexBuffer->Release();

	DXUTToggleFullScreen();

	// initialize D3D:
//	initD3D();

	// recreate the subrect vb:
	mVertexBuffer = createVertexBuffer(4);
}
*/
bool WinD3DInterface::beginScene()
{
	HRESULT hr = mD3D9Device->TestCooperativeLevel();
	if (hr!=S_OK)
	{
		handleError(hr);
		return false; // can't draw...
	}

	// set the rendering flag:
	mRendering = true;

	hr = mD3D9Device->Clear(
		0, // num rects to clear
		NULL, // rects
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		mClearColor, // clear color
		mClearZ,  // z buffer clear value 
		0 );   // stencil clear value
	if(FAILED(hr)) return false;

	// let the rendering begin...
	hr = mD3D9Device->BeginScene();
	if(FAILED(hr)) return false;

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	mD3D9Device->SetTransform(D3DTS_WORLD,&identity);

    D3DXMATRIX matView;    // the view transform matrix

	float w = (float)getWidth();
	float h = (float)getHeight();
	float w2 = w/2;
	float h2 = h/2;
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3 (w2, h2, CAM_Z),	// the camera position
		&D3DXVECTOR3 (w2, h2, 0),		// the look-at position
		&D3DXVECTOR3 (0,  -1,  0));		// the up direction

    mD3D9Device->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

    D3DXMATRIX matProjection;     // the projection transform matrix

	D3DXMatrixOrthoLH(&matProjection,
		(FLOAT)w,
		(FLOAT)h,
		(FLOAT)PROJECTION_Z_NEAR,
		(FLOAT)PROJECTION_Z_FAR);

    mD3D9Device->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection

	// set some default state stuff:
	mD3D9Device->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	mD3D9Device->SetRenderState(D3DRS_ALPHAREF, 1);
	mD3D9Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	mD3D9Device->SetRenderState(D3DRS_COLORVERTEX, true);
	mD3D9Device->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);	
	mD3D9Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	mD3D9Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	return true;
}

void WinD3DInterface::endScene()
{
	// reset the rendering flag:
	mRendering = false;

	// end of rendering:
	mD3D9Device->EndScene();

	//Show the results
	HRESULT hr = mD3D9Device->Present(NULL,NULL,NULL,NULL);
	handleError(hr);
}

void WinD3DInterface::handleError(HRESULT hr)
{
	switch (hr)
	{
	case D3D_OK: // same as S_OK
		// all clear!
		break;
	case D3DERR_DEVICELOST:
		SDL_Delay(100);
		break;
	case D3DERR_DEVICENOTRESET:
		// this is a hack, but it works:
		//DXUTToggleFullScreen();
		//DXUTToggleFullScreen();
		// end of hack
		SDL_Delay(100);
		break;
	default:
		assertSuccess(hr);
		break;
	}
}

void WinD3DInterface::drawImage(IDirect3DTexture9 *tex)
{
	// make sure beginScene was called:
	assert(mRendering);

	// bind to image's vertex buffer:
	HRESULT hr = mD3D9Device->SetStreamSource(0, mVertexBuffer, 0, sizeof(BoyVertex));
	if(FAILED(hr)) return;

	// set texture:
	hr = mD3D9Device->SetTexture(0,tex);
	if(FAILED(hr)) return;

	// render from vertex buffer:
	hr = mD3D9Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	if(FAILED(hr)) return;
}

void WinD3DInterface::drawImage(WinImage *image, DWORD color, float z)
{
	drawImage(image, color, z, 0, 0, image->getWidth(), image->getHeight());
}

void WinD3DInterface::drawImage(WinImage *image, DWORD color, float z, int x, int y, int w, int h)
{
	if (image->getTexture()==NULL)
	{
		envDebugLog("WARNING: trying to draw image with NULL texture (%s)\n",image->getPath().c_str());
		return;
	}

	// update the subrect vertex buffer values:
	D3DSURFACE_DESC textureInfo;
	HRESULT hr = image->getTexture()->GetLevelDesc(0,&textureInfo);
	assert(!FAILED(hr));

	float maxX = (float)w / 2.0f;
	float maxY = (float)h / 2.0f;
	float minX = -maxX;
	float minY = -maxY;

	float minU, minV, maxU, maxV;
	if (image->isTextureScaled())
	{
		float imgW = (float)image->getWidth();
		float imgH = (float)image->getHeight();
		minU = x / imgW;
		minV = y / imgH;
		maxU = minU + w / imgW;
		maxV = minV + h / imgH;
	}
	else
	{
		float texW = (float)textureInfo.Width;
		float texH = (float)textureInfo.Height;
		minU = x / texW;
		minV = y / texH;
		maxU = minU + w / texW;
		maxV = minV + h / texH;
	}
	int numVerts = 4;
	BoyVertex data[] = 
	{
		{minX, minY, z, color, minU, minV}, // top left
		{maxX, minY, z, color, maxU, minV}, // top right
		{minX, maxY, z, color, minU, maxV}, // bottom left
		{maxX, maxY, z, color, maxU, maxV}  // bottom right
	};

	// lock the vertex buffer:
	void *vbData = NULL;
	hr = mVertexBuffer->Lock(0, 0, &vbData, 0);
	assert(!FAILED(hr));

	// copy the data into the buffer:
	int byteCount = sizeof(BoyVertex) * numVerts;
	memcpy(vbData, data, byteCount);

	// unlock it:
	mVertexBuffer->Unlock();

	// draw the image:
	drawImage(image->getTexture());
}

void WinD3DInterface::drawRect(int x, int y, int w, int h, float z, DWORD color)
{
	float minX = (float)x;
	float minY = (float)y;
	float maxX = minX + w;
	float maxY = minY + h;

	int numVerts = 4;
	BoyVertex data[] = 
	{
		{minX, minY, z, color, 0, 0}, // top left
		{maxX, minY, z, color, 0, 0}, // top right
		{minX, maxY, z, color, 0, 0}, // bottom left
		{maxX, maxY, z, color, 0, 0}  // bottom right
	};

	// lock the vertex buffer:
	void *vbData = NULL;
	HRESULT hr = mVertexBuffer->Lock(0, 0, &vbData, 0);
	handleError(hr);

	// copy the data into the buffer:
	int byteCount = sizeof(BoyVertex) * numVerts;
	memcpy(vbData, data, byteCount);

	// unlock it:
	mVertexBuffer->Unlock();

	// draw the image:
	drawImage(NULL);
}

void WinD3DInterface::drawTriStrip(WinTriStrip *strip)
{
	// make sure beginScene was called:
	assert(mRendering);

	// create a vertex buffer:
	IDirect3DVertexBuffer9 *vb = createVertexBuffer(strip->mVertexCount);

	// lock the vertex buffer:
	void *vbData = NULL;
	HRESULT hr = vb->Lock(0, 0, &vbData, 0);
	handleError(hr);

	// copy the data into the buffer:
	int byteCount = sizeof(BoyVertex) * strip->mVertexCount;
	memcpy(vbData, strip->mVerts, byteCount);

	// unlock it:
	vb->Unlock();

	// bind to tristrip's vertex buffer:
	hr = mD3D9Device->SetStreamSource(0, vb, 0, sizeof(BoyVertex));
	if(FAILED(hr)) return;

	// set texture to NULL:
	hr = mD3D9Device->SetTexture(0,NULL);
	if(FAILED(hr)) return;

	// render from vertex buffer:
	hr = mD3D9Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, strip->mVertexCount-2);
	if(FAILED(hr)) return;

	// release the vertex buffer:
	vb->Release();
}

void WinD3DInterface::drawLine(int x0, int y0, int x1, int y1, Color color)
{
	BoyVertex verts[2];
	verts[0].x = (float)x0;
	verts[0].y = (float)y0;
	verts[0].z = 0;
	verts[0].u = 0;
	verts[0].v = 0;
	verts[0].color = color;
	verts[1].x = (float)x1;
	verts[1].y = (float)y1;
	verts[1].z = 0;
	verts[1].u = 0;
	verts[1].v = 0;
	verts[1].color = color;

	// make sure beginScene was called:
	assert(mRendering);

	// create a vertex buffer:
	IDirect3DVertexBuffer9 *vb = createVertexBuffer(2);

	// lock the vertex buffer:
	void *vbData = NULL;
	HRESULT hr = vb->Lock(0, 0, &vbData, 0);
	handleError(hr);

	// copy the data into the buffer:
	int byteCount = sizeof(BoyVertex) * 2;
	memcpy(vbData, verts, byteCount);

	// unlock it:
	vb->Unlock();

	// bind to tristrip's vertex buffer:
	hr = mD3D9Device->SetStreamSource(0, vb, 0, sizeof(BoyVertex));
	if(FAILED(hr)) return;

	// set texture to NULL:
	hr = mD3D9Device->SetTexture(0,NULL);
	if(FAILED(hr)) return;

	// render from vertex buffer:
	hr = mD3D9Device->DrawPrimitive(D3DPT_LINELIST, 0, 1);
	if(FAILED(hr)) return;

	// release the vertex buffer:
	vb->Release();
}

#include <wchar.h>
void WinD3DInterface::assertSuccess(HRESULT hr)
{
//#ifdef _DEBUG
	if (hr<0)
	{
		//const WCHAR *err = DXGetError(hr);
		//const WCHAR *desc = DXGetErrorDescription(hr);
		//wprintf(err);
		//printf("\n");
		//wprintf(desc);
		//printf("\n");
	}
//#endif
}

IDirect3DTexture9 *WinD3DInterface::loadTexture(const char *filenameUtf8, D3DXIMAGE_INFO *imageInfo, bool *scaled, bool warn)
{
	// declare some variables:
	IDirect3DTexture9 *tex = NULL;

	// get image info:
	UString filename(filenameUtf8);
	HRESULT result = D3DXGetImageInfoFromFile(filename.wc_str(), imageInfo);
	if (result != D3D_OK && warn)
	{
		printf("WARNING: D3DXGetImageInfoFromFile failed for '%s'\n",filename.toUtf8());
		assertSuccess(result);
		return NULL;
	}

	// calcualte texture width/height
	UINT width = D3DX_DEFAULT;
	UINT height = D3DX_DEFAULT;
	DWORD filter = D3DX_FILTER_NONE;
	if (imageInfo->Width > mMaxTextureWidth)
	{
		width = mMaxTextureWidth;
		if (mPow2TextureSize)
		{
			width = findHighestPowerOf2SmallerThan(width);
		}
		filter = D3DX_FILTER_LINEAR;
	}
	if (imageInfo->Height > mMaxTextureHeight)
	{
		height = mMaxTextureHeight;
		if (mPow2TextureSize)
		{
			width = findHighestPowerOf2SmallerThan(width);
		}
		filter = D3DX_FILTER_LINEAR;
	}

	// update the scaled flag:
	*scaled = (filter != D3DX_FILTER_NONE);

	// no color keying by default:
	D3DCOLOR colorKey = 0;

	// if this is a 32-bit ARGB image
	if (imageInfo->Format == D3DFMT_A8R8G8B8)
	{
		// map transparent white pixels to black:
		colorKey = 0x00ffffff;
		// (this fixes that white fuzz at the edges of images with alpha)
	}

	// load the texture:
	HRESULT hr = D3DXCreateTextureFromFileEx(
		mD3D9Device, // device
		filename.wc_str(), // file to load from
		width, // width
		height, // height
		D3DX_DEFAULT, // mipmap level count
		0, // usage
		D3DFMT_UNKNOWN, // pixel format
		D3DPOOL_MANAGED, // manged pool
		filter, // filtering function
		D3DX_DEFAULT, // default mipmap filter
		colorKey, // color key
		NULL, // optional structure to hold the source file info, but we get it above
		NULL, // no palette
		&tex); // the texture object to load the image into

	if (hr!=D3D_OK && warn)
	{
		printf("WARNING: D3DXCreateTextureFromFileEx failed for '%s'\n",filename.toUtf8());
		printf("w=%d h=%d filter=0x%x colorKey=0x%x\n",width,height,filter,colorKey);
		assertSuccess(hr);
	}

	if(FAILED(hr)) return NULL;

	// return it:
	return tex;
}

IDirect3DVertexBuffer9 *WinD3DInterface::createVertexBuffer(int numVerts)
{
	// create the vertex buffer:
	IDirect3DVertexBuffer9 *vb;
	int byteCount = sizeof(BoyVertex) * numVerts;
	HRESULT hr = mD3D9Device->CreateVertexBuffer(
		byteCount,
		D3DUSAGE_WRITEONLY,
		BOYFVF,
		D3DPOOL_MANAGED,
		&vb, 
		NULL);
	if(FAILED(hr)) return NULL;

	// return it:
	return vb;
}

void WinD3DInterface::initD3D()
{
	// get some device capabilities:
	D3DCAPS9 caps;
	mD3D9Device->GetDeviceCaps(&caps);
	mMaxTextureWidth = caps.MaxTextureWidth;
	mMaxTextureHeight = caps.MaxTextureHeight;
	mPow2TextureSize = (caps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0;

	// set up an orthogonal projection matrix:
	D3DXMATRIX projection;
	D3DXMatrixOrthoRH(
		&projection, 
		(FLOAT)mPresentationParameters.BackBufferWidth,
		(FLOAT)mPresentationParameters.BackBufferHeight,
		(FLOAT)-1000,
		(FLOAT)+1000);
	HRESULT hr = mD3D9Device->SetTransform(D3DTS_PROJECTION,&projection);
	assert(!FAILED(hr));

	// set world / view matrices to identity:
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	mD3D9Device->SetTransform(D3DTS_WORLD, &identity);
	mD3D9Device->SetTransform(D3DTS_VIEW, &identity);

	// turn off lighting:
    mD3D9Device->SetRenderState(D3DRS_LIGHTING, false);

	// set the flexible vertex format:
	hr = mD3D9Device->SetFVF(BOYFVF);
	if(FAILED(hr)) return;

	// set up texture blending parameters:
	hr = mD3D9Device->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
	if(FAILED(hr)) return;
	hr = mD3D9Device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
	if(FAILED(hr)) return;
}

void WinD3DInterface::setTransform(D3DXMATRIX &xform)
{
	// set the new transform:
	mD3D9Device->SetTransform(D3DTS_WORLD, &xform);
}

void WinD3DInterface::setSamplerState(D3DSAMPLERSTATETYPE state, DWORD value)
{
	mD3D9Device->SetSamplerState(0,state,value);
}

void WinD3DInterface::setRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
	HRESULT hr = mD3D9Device->SetRenderState(state,value);
	assertSuccess(hr);
}

DWORD WinD3DInterface::getRenderState(D3DRENDERSTATETYPE state)
{
	DWORD value;
	mD3D9Device->GetRenderState(state,&value);
	return value;
}

void WinD3DInterface::setClipRect(int x, int y, int width, int height)
{
	RECT rect;
	rect.left = x;
	rect.right = x+width;
	rect.top = y;
	rect.bottom = y+height;
	mD3D9Device->SetScissorRect(&rect);
}

void WinD3DInterface::dumpInfo(std::ofstream &file)
{
	D3DCAPS9 caps;
	mD3D9Device->GetDeviceCaps(&caps);

	file << "type: ";
	switch (caps.DeviceType)
	{
	case D3DDEVTYPE_HAL:
		file << "D3DDEVTYPE_HAL";
		break;
	case D3DDEVTYPE_REF:
		file << "D3DDEVTYPE_REF";
		break;
	case D3DDEVTYPE_SW:
		file << "D3DDEVTYPE_SW";
		break;
	}
	file << "\n";

	file << "\nCaps3:\n";
	file << "\t D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD = " << ((caps.Caps3 & D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD) ? "true\n" : "false\n");

	file << "\nPresentationIntervals:\n";
	file << "\t D3DPRESENT_INTERVAL_DEFAULT = " << ((caps.PresentationIntervals & D3DPRESENT_INTERVAL_DEFAULT) ? "true\n" : "false\n");
	file << "\t D3DPRESENT_INTERVAL_ONE = " << ((caps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE) ? "true\n" : "false\n");
	file << "\t D3DPRESENT_INTERVAL_TWO = " << ((caps.PresentationIntervals & D3DPRESENT_INTERVAL_TWO) ? "true\n" : "false\n");
	file << "\t D3DPRESENT_INTERVAL_THREE = " << ((caps.PresentationIntervals & D3DPRESENT_INTERVAL_THREE) ? "true\n" : "false\n");
	file << "\t D3DPRESENT_INTERVAL_FOUR = " << ((caps.PresentationIntervals & D3DPRESENT_INTERVAL_FOUR) ? "true\n" : "false\n");
	file << "\t D3DPRESENT_INTERVAL_IMMEDIATE = " << ((caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) ? "true\n" : "false\n");

	file << "\nDevCaps (D3D):\n";
	file << "\t D3DDEVCAPS_CANRENDERAFTERFLIP = "<<((caps.DevCaps & D3DDEVCAPS_CANRENDERAFTERFLIP)?"true\n":"false\n");
	file << "\t D3DDEVCAPS_DRAWPRIMTLVERTEX = "<<((caps.DevCaps & D3DDEVCAPS_DRAWPRIMTLVERTEX)?"true\n":"false\n");
	file << "\t D3DDEVCAPS_HWRASTERIZATION = "<<((caps.DevCaps & D3DDEVCAPS_HWRASTERIZATION)?"true\n":"false\n");
	file << "\t D3DDEVCAPS_PUREDEVICE = "<<((caps.DevCaps & D3DDEVCAPS_PUREDEVICE)?"true\n":"false\n");
	file << "\t D3DDEVCAPS_SEPARATETEXTUREMEMORIES = "<<((caps.DevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES)?"true\n":"false\n");
	file << "\t D3DDEVCAPS_TEXTURENONLOCALVIDMEM = "<<((caps.DevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM)?"true\n":"false\n");
	file << "\t D3DDEVCAPS_TEXTUREVIDEOMEMORY = "<<((caps.DevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY)?"true\n":"false\n");
	file << "\t D3DDEVCAPS_TLVERTEXSYSTEMMEMORY = "<<((caps.DevCaps & D3DDEVCAPS_TLVERTEXSYSTEMMEMORY)?"true\n":"false\n");
	file << "\t D3DDEVCAPS_TLVERTEXVIDEOMEMORY = "<<((caps.DevCaps & D3DDEVCAPS_TLVERTEXVIDEOMEMORY)?"true\n":"false\n");

	file << "\nPrimitiveMiscCaps:\n";
	file << "\t D3DPMISCCAPS_MASKZ = "<<((caps.PrimitiveMiscCaps & D3DPMISCCAPS_MASKZ)?"true\n":"false\n");
	file << "\t D3DPMISCCAPS_BLENDOP = "<<((caps.PrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP)?"true\n":"false\n");
	file << "\t D3DPMISCCAPS_SEPARATEALPHABLEND = "<<((caps.PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND)?"true\n":"false\n");

	file << "\nRasterCaps:\n";
	file << "\t D3DPRASTERCAPS_SCISSORTEST = "<<((caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST)?"true\n":"false\n");
	file << "\t D3DPRASTERCAPS_ZBUFFERLESSHSR = "<<((caps.RasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR)?"true\n":"false\n");
	file << "\t D3DPRASTERCAPS_ZTEST = "<<((caps.RasterCaps & D3DPRASTERCAPS_ZTEST)?"true\n":"false\n");

	file << "\nZCmpCaps:\n";
	file << "\t D3DPCMPCAPS_GREATER = "<<((caps.ZCmpCaps & D3DPCMPCAPS_GREATER)?"true\n":"false\n");
	file << "\t D3DPCMPCAPS_GREATEREQUAL = "<<((caps.ZCmpCaps & D3DPCMPCAPS_GREATEREQUAL)?"true\n":"false\n");
	file << "\t D3DPCMPCAPS_LESS = "<<((caps.ZCmpCaps & D3DPCMPCAPS_LESS)?"true\n":"false\n");
	file << "\t D3DPCMPCAPS_LESSEQUAL = "<<((caps.ZCmpCaps & D3DPCMPCAPS_LESSEQUAL)?"true\n":"false\n");

	file << "\nSrcBlendCaps:\n";
	file << "\t D3DPBLENDCAPS_BOTHINVSRCALPHA = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_BOTHINVSRCALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_BOTHSRCALPHA = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_BOTHSRCALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_DESTALPHA = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_DESTALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_DESTCOLOR = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_DESTCOLOR)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_INVDESTALPHA = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_INVDESTALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_INVDESTCOLOR = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_INVDESTCOLOR)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_INVSRCALPHA = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_INVSRCALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_INVSRCCOLOR = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_ONE = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_ONE)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_SRCALPHA = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_SRCALPHASAT = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHASAT)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_SRCCOLOR = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_SRCCOLOR)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_ZERO = "<<((caps.SrcBlendCaps & D3DPBLENDCAPS_ZERO)?"true\n":"false\n");

	file << "\nDestBlendCaps:\n";
	file << "\t D3DPBLENDCAPS_BOTHINVSRCALPHA = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_BOTHINVSRCALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_BOTHSRCALPHA = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_BOTHSRCALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_DESTALPHA = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_DESTALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_DESTCOLOR = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_DESTCOLOR)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_INVDESTALPHA = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_INVDESTALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_INVDESTCOLOR = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_INVDESTCOLOR)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_INVSRCALPHA = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_INVSRCCOLOR = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_ONE = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_ONE)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_SRCALPHA = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_SRCALPHA)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_SRCALPHASAT = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_SRCALPHASAT)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_SRCCOLOR = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_SRCCOLOR)?"true\n":"false\n");
	file << "\t D3DPBLENDCAPS_ZERO = "<<((caps.DestBlendCaps & D3DPBLENDCAPS_ZERO)?"true\n":"false\n");

	file << "\nAlphaCmpCaps:\n";
	file << "\t D3DPCMPCAPS_GREATER = "<<((caps.AlphaCmpCaps & D3DPCMPCAPS_GREATER)?"true\n":"false\n");
	file << "\t D3DPCMPCAPS_GREATEREQUAL = "<<((caps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL)?"true\n":"false\n");
	file << "\t D3DPCMPCAPS_LESS = "<<((caps.AlphaCmpCaps & D3DPCMPCAPS_LESS)?"true\n":"false\n");
	file << "\t D3DPCMPCAPS_LESSEQUAL = "<<((caps.AlphaCmpCaps & D3DPCMPCAPS_LESSEQUAL)?"true\n":"false\n");

	file << "\nTextureCaps:\n";
	file << "\t D3DPTEXTURECAPS_ALPHA = "<<((caps.TextureCaps & D3DPTEXTURECAPS_ALPHA)?"true\n":"false\n");
	file << "\t D3DPTEXTURECAPS_MIPMAP = "<<((caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP)?"true\n":"false\n");
	file << "\t D3DPTEXTURECAPS_NONPOW2CONDITIONAL = "<<((caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)?"true\n":"false\n");
	file << "\t D3DPTEXTURECAPS_POW2 = "<<((caps.TextureCaps & D3DPTEXTURECAPS_POW2)?"true\n":"false\n");
	file << "\t D3DPTEXTURECAPS_SQUAREONLY = "<<((caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)?"true\n":"false\n");

	file << "\nTextureFilterCaps:\n";
	file << "\t D3DPTFILTERCAPS_MAGFLINEAR = "<<((caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR)?"true\n":"false\n");
	file << "\t D3DPTFILTERCAPS_MINFLINEAR = "<<((caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)?"true\n":"false\n");
	file << "\t D3DPTFILTERCAPS_MIPFLINEAR = "<<((caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR)?"true\n":"false\n");

	file << "\nTextureAddressCaps:\n";
	file << "\t D3DPTADDRESSCAPS_BORDER = "<<((caps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)?"true\n":"false\n");
	file << "\t D3DPTADDRESSCAPS_CLAMP = "<<((caps.TextureAddressCaps & D3DPTADDRESSCAPS_CLAMP)?"true\n":"false\n");
	file << "\t D3DPTADDRESSCAPS_INDEPENDENTUV = "<<((caps.TextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV)?"true\n":"false\n");
	file << "\t D3DPTADDRESSCAPS_MIRROR = "<<((caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR)?"true\n":"false\n");
	file << "\t D3DPTADDRESSCAPS_MIRRORONCE = "<<((caps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRRORONCE)?"true\n":"false\n");
	file << "\t D3DPTADDRESSCAPS_WRAP = "<<((caps.TextureAddressCaps & D3DPTADDRESSCAPS_WRAP)?"true\n":"false\n");
	
	file << "\nLineCaps:\n";
	file << "\t D3DLINECAPS_ALPHACMP = "<<((caps.LineCaps & D3DLINECAPS_ALPHACMP)?"true\n":"false\n");
	file << "\t D3DLINECAPS_ANTIALIAS = "<<((caps.LineCaps & D3DLINECAPS_ANTIALIAS)?"true\n":"false\n");
	file << "\t D3DLINECAPS_BLEND = "<<((caps.LineCaps & D3DLINECAPS_BLEND)?"true\n":"false\n");
	file << "\t D3DLINECAPS_ZTEST = "<<((caps.LineCaps & D3DLINECAPS_ZTEST)?"true\n":"false\n");

	file << "\nMaxTextureWidth = " << caps.MaxTextureWidth << "\n";
	file << "\nMaxTextureHeight = " << caps.MaxTextureHeight << "\n";
	file << "\nMaxTextureAspectRatio = " << caps.MaxTextureAspectRatio << "\n";

	file << "\nTextureOpCaps:\n";
	file << "\t D3DTEXOPCAPS_ADD = "<<((caps.TextureOpCaps & D3DTEXOPCAPS_ADD)?"true\n":"false\n");
	file << "\t D3DTEXOPCAPS_ADDSIGNED = "<<((caps.TextureOpCaps & D3DTEXOPCAPS_ADDSIGNED)?"true\n":"false\n");
	file << "\t D3DTEXOPCAPS_MODULATE = "<<((caps.TextureOpCaps & D3DTEXOPCAPS_MODULATE)?"true\n":"false\n");
	file << "\t D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR = "<<((caps.TextureOpCaps & D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR)?"true\n":"false\n");
	file << "\t D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA = "<<((caps.TextureOpCaps & D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA)?"true\n":"false\n");
	file << "\t D3DTEXOPCAPS_SELECTARG1 = "<<((caps.TextureOpCaps & D3DTEXOPCAPS_SELECTARG1)?"true\n":"false\n");
	file << "\t D3DTEXOPCAPS_SELECTARG2 = "<<((caps.TextureOpCaps & D3DTEXOPCAPS_SELECTARG2)?"true\n":"false\n");
	file << "\t D3DTEXOPCAPS_SUBTRACT = "<<((caps.TextureOpCaps & D3DTEXOPCAPS_SUBTRACT)?"true\n":"false\n");
}

void WinD3DInterface::handleLostDevice()
{
	mVertexBuffer->Release();
	ResourceManager *rm = Environment::instance()->getResourceManager();
	if (rm!=NULL)
	{
		printf("handleDeviceLost(): calling rm->destroyResources()... ");
		rm->destroyResources(false);
		printf("done\n");
	}
}

void WinD3DInterface::handleResetDevice()
{
	// persist the fullscreen setting:
	Boy::Environment *env = Boy::Environment::instance();
	Boy::PersistenceLayer *pl = env->getPersistenceLayer();
	bool fullscreen = env->isFullScreen();
	pl->putString("fullscreen",fullscreen?"true":"false",true);

	mVertexBuffer = createVertexBuffer(4);
	ResourceManager *rm = Environment::instance()->getResourceManager();
	if (rm!=NULL)
	{
		printf("handleResetDevice(): calling rm->initResources()... ");
		rm->initResources(false);
		printf("done\n");
	}
	initD3D();

	// notify the game of the device reset:
	mGame->windowResized(0,0,getWidth(),getHeight());
}

IDirect3DDevice9* WinD3DInterface::GetD3DDevice()
{
	return mD3D9Device;
}

SDL_Window* WinD3DInterface::GetSDLWindow()
{
	return mWindow;
}