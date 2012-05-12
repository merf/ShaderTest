#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ObjLoader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Fbo.h"

#include "cinder/camera.h"

#include "SimpleGUI.h"

#include <boost/lexical_cast.hpp>

using namespace ci;
using namespace ci::app;

float shininess = 20.0;
float fresnel_power = 0.5f;
float specular_intensity = 0.1f;
float fresnel_intensity = 0.1f;
float explode = 0.0f;
bool exploding = false;

ColorA diffuse_color = ColorA(0.7f, 0.3f, 0.4f, 1.0f);


class ShaderTestApp : public AppBasic 
{
 public: 	
	void setup();
	void keyDown( KeyEvent event );
	void resize(ResizeEvent event);
	
	void draw();
	void DrawParticles();
	void DrawSceneToFBO();
	void PostProcess();
	
	bool Explode(MouseEvent event) { exploding = true; return true; }
	bool ResetExplosion(MouseEvent event) { exploding = false; return true; }
	
	gl::Texture				m_Texture;
	gl::GlslProg			m_Shader;
	gl::GlslProg			m_ParticleShader;
	gl::GlslProg			m_PostProcessShader;
	
	TriMesh					m_TriMesh;
	gl::VboMesh				m_Mesh;
	
	gl::Fbo					m_FBO;
	
	CameraPersp				m_Cam;
	mowa::sgui::SimpleGUI*	mp_GUI;
	
	ci::Matrix44f			m_PrevModelView;
};


void ShaderTestApp::setup()
{
	std::string mesh = "monkey288k";
	std::string trimesh_file = mesh;
	trimesh_file.append(".trimesh");
	std::string obj_file = mesh;
	obj_file.append(".obj");
	
	try
	{
		// try to load the mesh from its binary file
        m_TriMesh.read(loadAsset(trimesh_file));
	}
	catch ( ... )
	{
		// load model resource into mesh (takes a while!)
		ObjLoader loader(loadAsset(obj_file));
		loader.load(&m_TriMesh);
		
		// one-time only: write mesh to binary file
		fs::path path = getSaveFilePath(trimesh_file);
		if( ! path.empty() ) {
			console() << "Saving to " << path;
			m_TriMesh.write( writeFile( path ) );
		}
		//OStreamFileRef stream = ci::writeFileStream(myMeshFile, true);
		//if( stream ) m_TriMesh.write(stream);
	}

	m_Mesh = gl::VboMesh(m_TriMesh);
	
	try 
	{
//		mTexture = gl::Texture(loadImage(loadAsset("image.png")));
		gl::Texture::Format fmt;
		fmt.enableMipmapping();
		fmt.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		fmt.setMagFilter(GL_LINEAR_MIPMAP_LINEAR);
		fmt.setWrap(GL_REPEAT, GL_REPEAT);
		
		m_Texture = gl::Texture(loadImage(loadAsset("rainbow.jpg")), fmt);
	}
	catch( ... ) 
	{
		std::cout << "unable to load the texture file!" << std::endl;
		quit();
	}
	
	try 
	{
		//mShader = gl::GlslProg(loadAsset("Diffuse.vs"), loadAsset("Diffuse.fs"));
		m_Shader = gl::GlslProg(loadAsset("Specular.vs"), loadAsset("Specular.fs"));
		m_ParticleShader = gl::GlslProg(loadAsset("Particle.vs"), loadAsset("Specular.fs"));
		m_PostProcessShader = gl::GlslProg(loadAsset("PassThrough.vs"), loadAsset("PostProcess.fs"));
	}
	catch( gl::GlslProgCompileExc &exc ) 
	{
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
		quit();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
	
	setWindowSize(1280, 720);

	gl::Fbo::Format fmt;
	fmt.enableColorBuffer(true, 2);
	m_FBO = gl::Fbo(getWindowWidth(), getWindowHeight(), fmt);
	
	m_Cam.lookAt(Vec3f(0,0,4), Vec3f::zero());
	
	mp_GUI = new mowa::sgui::SimpleGUI(this);
	mp_GUI->addLabel("Shader");
	mp_GUI->addParam("colour", &diffuse_color, diffuse_color);
	mp_GUI->addParam("shininess", &shininess, 1.0f, 100, shininess);
	mp_GUI->addParam("fresnel_power", &fresnel_power, 0.001, 5, fresnel_power);
	mp_GUI->addParam("specular_intensity", &specular_intensity, 0, 0.5f, specular_intensity);
	mp_GUI->addParam("fresnel_intensity", &fresnel_intensity, 0, 0.5f, fresnel_intensity);
	mp_GUI->addLabel("Particles");
	mp_GUI->addParam("explosion progress", &explode, 0.0f, 2.0f, explode);
	mp_GUI->addButton("explode")->registerClick(this, &ShaderTestApp::Explode);
	mp_GUI->addButton("reset")->registerClick(this, &ShaderTestApp::ResetExplosion);
	mp_GUI->addParam("preview", &m_FBO.getTexture(1));
	
	m_PrevModelView = ci::Matrix44f::identity();
}

void ShaderTestApp::resize(cinder::app::ResizeEvent event)
{
	m_Cam.setAspectRatio(event.getAspectRatio());
//	m_Cam.lookAt(Vec3f(0,-1,7), Vec3f::zero());
}

void ShaderTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == app::KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
	if( event.getCode() == app::KeyEvent::KEY_x ) {
		if(exploding)
		{
			ResetExplosion(MouseEvent());
		}
		else 
		{
			Explode(MouseEvent());
		}
	}
}

void ShaderTestApp::DrawParticles()
{
	std::vector<Vec3f> pts = m_TriMesh.getVertices();
	std::vector<Vec3f> normals = m_TriMesh.getNormals();
	
	float size = 0.002f * (float)getWindowSize().length();
	glPointSize(size);
	glBegin(GL_POINTS);
	int num_pts = pts.size();
	for(int i=0; i<num_pts; ++i)
	{
		glVertex3f(pts[i]);
		glNormal3f(normals[i]);
	}
	glEnd();
}

void ShaderTestApp::draw()
{
	DrawSceneToFBO();
	PostProcess();
	
	bool write_frames = false;
	if( write_frames )
	{
		std::string path = "/Users/Neil/vis_frames/frame" + boost::lexical_cast<std::string>(getElapsedFrames()) + ".png";
		writeImage(path, copyWindowSurface());
		//		writeImage( getHomeDirectory() + "image_" + toString( getElapsedFrames() ) + ".png",
		//				   copyWindowSurface() );
	}

	mp_GUI->draw();
}

void ShaderTestApp::DrawSceneToFBO()
{
	// this will restore the old framebuffer binding when we leave this function
	// on non-OpenGL ES platforms, you can just call mFbo.unbindFramebuffer() at the end of the function
	// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
	gl::SaveFramebufferBinding bindingSaver;
	
	// bind the framebuffer - now everything we draw will go there
	m_FBO.bindFramebuffer();
	
	// setup the viewport to match the dimensions of the FBO
	gl::setViewport(m_FBO.getBounds());
	
	if(exploding)
	{
		explode += 0.005f;
	}
	else 
	{
		if(explode > 0)
		{
			explode -= 0.005f;
		}
		explode = math<float>::clamp(explode, 0.0, 10.0f);
	}
	
	float dist = 5.0f - explode * 3.0f;
	float rot = M_PI * 2.0f * pow(explode, 2.0f);
	
	
	m_Cam.lookAt(Vec3f(-dist*sin(rot), 0, dist*cos(rot)), Vec3f::zero(), Vec3f(0,-1,0));
	
	float time = getElapsedSeconds();
	
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	
	GLfloat light_position0[] = { 3, 1, 5, 1 };
	glLightfv( GL_LIGHT0, GL_POSITION, light_position0 );
	
	GLfloat light_color0[] = { 1,1,1,1 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color0);
	
	glLightfv(GL_LIGHT0, GL_SPECULAR, ColorA(1.0, 1.0, 0.0, 1.0));
	
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color);
	
	ColorA ambient_color = (diffuse_color * ColorA(0.5f, 0.5f, 1.0f, 1.0f)) * 0.1f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_color);
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, ColorA(1,1,1,specular_intensity));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	gl::clear();
	
	gl::setMatrices(m_Cam);
	
	Matrix44f cam_mat =	m_Cam.getModelViewMatrix();
	
	gl::pushModelView();

	//ci::Matrix44f model_view = ci::Matrix44f::createRotation(Vec3f(0.5f * sin(time), 1.0f, 0.0f).normalized(), sin(0.5f *  time) * M_PI);
	ci::Matrix44f model_view = ci::Matrix44f::createRotation(Vec3f(5.0, 1.0f, 0.0f).normalized(), powf(sin(2.5f *  time), 3.0f) * M_PI * 0.5f);
	gl::multModelView(model_view);
	
	Vec2f resolution = getWindowSize();
	
	m_Shader.bind();
	
	
	m_Shader.uniform("time", time);	
	m_ParticleShader.uniform("explode", explode);

	m_Shader.uniform("resolution", resolution);

	m_Shader.uniform("fresnel_power", fresnel_power);	
	m_Shader.uniform("fresnel_intensity", fresnel_intensity);
	
	m_Shader.uniform("previous_model_view", cam_mat * m_PrevModelView);
	m_Shader.uniform("model_view", cam_mat * model_view);

	gl::draw(m_Mesh);
	m_Shader.unbind();
	
	m_ParticleShader.bind();
	m_ParticleShader.uniform("explode", explode);
	//DrawParticles();
	m_ParticleShader.unbind();
	
	gl::popModelView();

	glDisable(GL_LIGHTING);
	
	m_PrevModelView = model_view;
}

void ShaderTestApp::PostProcess()
{	
	m_PostProcessShader.bind();
	m_PostProcessShader.uniform("velocity_tex", 1);
	
	gl::clear();
//	gl::color(1, 1, 1, 1);
	
	gl::setMatricesWindow(getWindowWidth(), getWindowHeight());

	glEnable(GL_TEXTURE_2D);
	m_FBO.getTexture(0).bind(0);
	m_FBO.getTexture(1).bind(1);
	//m_FBO.bindTexture(0,0);
	//m_FBO.bindTexture(0,1);
	gl::draw(m_FBO.getTexture(0));
	m_FBO.unbindTexture();
		
	m_PostProcessShader.unbind();
}


CINDER_APP_BASIC( ShaderTestApp, RendererGl )