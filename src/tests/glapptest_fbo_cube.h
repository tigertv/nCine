#ifndef CLASS_MYEVENTHANDLER
#define CLASS_MYEVENTHANDLER

#include "IAppEventHandler.h"
#include "IInputEventHandler.h"
#include "Matrix4x4.h"

namespace ncine {

class AppConfiguration;
class GLShaderProgram;
class GLShaderUniforms;
class GLShaderAttributes;
class GLTexture;
class GLFramebufferObject;
class GLBufferObject;

}

namespace nc = ncine;

/// My nCine event handler
class MyEventHandler
	: public nc::IAppEventHandler,
	  public nc::IInputEventHandler
{
  public:
	virtual void onPreInit(nc::AppConfiguration &config);
	virtual void onInit();
	virtual void onFrameStart();
	virtual void onShutdown();

	virtual void onKeyReleased(const nc::KeyboardEvent &event);

  private:
	static const int FboSize = 256;

	float angle_;
	int width_, height_;
	nc::GLShaderProgram *colorProgram_;
	nc::GLShaderUniforms *colorUniforms_;
	nc::GLShaderAttributes *colorAttributes_;
	nc::GLShaderProgram *texProgram_;
	nc::GLShaderUniforms *texUniforms_;
	nc::GLShaderAttributes *texAttributes_;
	nc::GLTexture *texture_;
	nc::GLFramebufferObject *fbo_;
	nc::GLBufferObject *vboTri_;
	nc::GLBufferObject *vboCube_;
	nc::GLBufferObject *iboCube_;
	nc::Matrix4x4f projection_;
	nc::Matrix4x4f modelView_;
};

#endif
