#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"

#include "cinder/camera.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class ImageFileTestApp : public AppBasic {
 public: 	
	void setup();
	void keyDown( KeyEvent event );
	
	void update();
	void draw();
	
	gl::Texture		mTexture;	
	gl::GlslProg	mShader;
	float			mAngle;
	
	CameraPersp		mCam;
};


void ImageFileTestApp::setup()
{
	try 
	{
		//mTexture = gl::Texture( loadImage( loadResource( RES_IMAGE_JPG ) ) );
		
//		mTexture = gl::Texture(loadImage(loadAsset("image.png")));
		gl::Texture::Format fmt;
		fmt.enableMipmapping();
		fmt.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		fmt.setMagFilter(GL_LINEAR_MIPMAP_LINEAR);
		fmt.setWrap(GL_REPEAT, GL_REPEAT);
		
		mTexture = gl::Texture(loadImage(loadAsset("rainbow.jpg")), fmt);
	}
	catch( ... ) 
	{
		std::cout << "unable to load the texture file!" << std::endl;
		quit();
	}
	
	try 
	{
		//mShader = gl::GlslProg(loadAsset("Diffuse.vs"), loadAsset("Diffuse.fs"));
		mShader = gl::GlslProg(loadAsset("Specular.vs"), loadAsset("Specular.fs"));
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
		quit();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
	
	mAngle = 0.0f;
	
	setWindowSize(800, 600);
	
	mCam.lookAt(Vec3f(0,3,5), Vec3f::zero());
}

void ImageFileTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == app::KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
}

void ImageFileTestApp::update()
{
	mAngle += 0.05f;
}

void ImageFileTestApp::draw()
{
	float time = getElapsedSeconds();
	
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	
	//GLfloat light_position0[] = { -sin(time)*5, 3, cos(time)*5, 1 };
	GLfloat light_position0[] = { 3, 1, 5, 1 };
	glLightfv( GL_LIGHT0, GL_POSITION, light_position0 );
	
	GLfloat light_color0[] = { 1,1,1,1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color0);

	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	gl::clear();
	
	gl::setMatrices(mCam);
	
	gl::pushModelView();
	
//	/gl::rotate(Vec3f(0, getElapsedSeconds() * 10, 0));

	Vec2f resolution = getWindowSize();
	
	mTexture.enableAndBind();
	
	mShader.bind();
	mShader.uniform("time", time);	
	mShader.uniform("resolution", resolution);
	mShader.uniform("variation", 2);

	static float rx = 0;
	static float ry = 0;
	static float rz = 0;

	rx = 500 *  time / 7.7f;
	ry = 500 *  time / 15.0f;
	rz = 500 *  time / 27.3f;
	
	const int num_spheres = 10;
	for(int i = 0; i<num_spheres; ++i)
	{
		float f = i/(float)num_spheres;

		float angle = (f + time * 0.01f) * M_PI * 2.0f;

		gl::pushModelView();
		gl::rotate(Vec3f(ry, 2*rx, 3.0f*rz));
		gl::translate(Vec3f(1 * cos(angle), 0, -1 * sin(angle)));
		gl::drawSphere(Vec3f(0, 0, 0), 0.25f, 64);
		gl::popModelView();
	}

	gl::rotate(Vec3f(rx, ry, rz));
	gl::drawTorus(0.5f, 0.25f, 64, 64);
	//gl::drawSphere(Vec3f(0, 0, 0), 0.5f, 64);
	//gl::drawCylinder(0.5f, 0.5f, 1.0f);
	
	mTexture.unbind();
	
	gl::popModelView();

}


CINDER_APP_BASIC( ImageFileTestApp, RendererGl )