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

//#include "Palette/Palette.h"

using namespace ci;
using namespace ci::app;

Vec4f light_pos_0;
Vec4f light_pos_1;

Vec3f extinction_coefficient = Vec3f(0.8f, 0.5f, 0.5f);
float light_dist = 1.5f;
float light_angle = 1.0f;
float thickness = 0.1f;
float shininess = 1.0;
float fresnel_power = 0.5f;
float specular_intensity = 0.05f;
float fresnel_intensity = 1.1f;
float explode = 0.0f;
bool exploding = false;

float curr_time;
Vec2f resolution;

ColorA diffuse_color = ColorA(0.7f, 0.3f, 0.4f, 1.0f);

ColorA light_color_0 = ColorA(1.0f, 0.8f, 0.2f, 1.0f);
ColorA light_color_1 = ColorA(0.2f, 0.1f, 1.0f, 1.0f);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
class ShaderTestApp : public AppBasic 
{
 public: 	
	void setup();
	void keyDown( KeyEvent event );
	void resize(ResizeEvent event);
	
	void draw();
	void DrawParticles();
	void DrawSceneToFBO();

	void SetupLightsAndMaterials();
	void PostProcess();
	
	bool Explode(MouseEvent event) { exploding = true; return true; }
	bool ResetExplosion(MouseEvent event) { exploding = false; return true; }
	
	void BindShaderAndSetUniforms(gl::GlslProg& shader);
	
	gl::Texture				m_Texture;
	gl::GlslProg			m_Shader;
	gl::GlslProg			m_PostProcessShader;
	
	TriMesh					m_TriMesh;
	gl::VboMesh				m_Mesh;
	
	gl::Fbo					m_FBO;
	bool					m_RecreateFBO;
	
	CameraPersp				m_Cam;
	mowa::sgui::SimpleGUI*	mp_GUI;
	
	ci::Matrix44f			m_PrevModelView;
	
	//CPalette*				mp_Palette;
};


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void ShaderTestApp::setup()
{
	std::string mesh = "monkey72k";
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
		//m_Shader = gl::GlslProg(loadAsset("Diffuse.vs"), loadAsset("Diffuse.fs"));
		m_Shader = gl::GlslProg(loadAsset("SubSurface.vs"), loadAsset("SubSurface.fs"));
		//m_Shader = gl::GlslProg(loadAsset("Specular.vs"), loadAsset("Specular.fs"));
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
	
	setWindowSize(800, 600);

	gl::Fbo::Format fmt;
	fmt.enableColorBuffer(true, 2);
	fmt.setSamples(4);
	m_FBO = gl::Fbo(1280, 720, fmt);

	m_RecreateFBO = false;
	
	m_Cam.lookAt(Vec3f(0,0,10), Vec3f::zero());
	
//	mp_Palette = new CPalette("candy.aco");
	
	mp_GUI = new mowa::sgui::SimpleGUI(this);
	mp_GUI->bgColor = ColorA(0.2f, 0.2f, 0.2f, 0.8f);
	mp_GUI->darkColor = ColorA(0.4f, 0.4f, 0.4f, 0.8f);
	mp_GUI->lightColor = ColorA(0.6f, 0.6f, 0.6f, 0.8f);
	mp_GUI->textColor = ColorA(0.8f, 0.8f, 0.8f, 0.8f);
	//mp_GUI->bgColor = mp_Palette->getColor(3);
	//mp_GUI->darkColor = mp_Palette->getColor(2);
	//mp_GUI->lightColor = mp_Palette->getColor(3);
	//mp_GUI->textColor = mp_Palette->getColor(1) + ColorA(0.2f, 0.2f, 0.2f, 0.0f);
//	mp_GUI->bgColor = ColorA(1.0f, 0.0f, 0.0f, 1.0f);
	
	mp_GUI->addColumn();
	mp_GUI->addLabel("Shader");
	mp_GUI->addParam("colour", &diffuse_color, diffuse_color);
	mp_GUI->addParam("shininess", &shininess, 1.0f, 100, shininess);
	mp_GUI->addParam("specular_intensity", &specular_intensity, 0, 0.5f, specular_intensity);
	mp_GUI->addParam("fresnel_power", &fresnel_power, 0.001f, 5, fresnel_power);
	mp_GUI->addParam("fresnel_intensity", &fresnel_intensity, 0, 2.0f, fresnel_intensity);
	
	mp_GUI->addColumn();
	mp_GUI->addLabel("Lighting");
	mp_GUI->addParam("thickness", &thickness, 0, 1.0f, thickness);
	mp_GUI->addParam("extinction red", &extinction_coefficient.x, 0, 1, extinction_coefficient.x);
	mp_GUI->addParam("extinction green", &extinction_coefficient.y, 0, 1, extinction_coefficient.y);
	mp_GUI->addParam("extinction blue", &extinction_coefficient.z, 0, 1, extinction_coefficient.z);
	mp_GUI->addParam("light_dist", &light_dist, 0, 10, light_dist);
	mp_GUI->addParam("light_angle", &light_angle, -M_PI, M_PI, light_angle);
	

	mp_GUI->addColumn();
	mp_GUI->addLabel("Particles");
	mp_GUI->addParam("explosion progress", &explode, 0.0f, 2.0f, explode);
	mp_GUI->addButton("explode")->registerClick(this, &ShaderTestApp::Explode);
	mp_GUI->addButton("reset")->registerClick(this, &ShaderTestApp::ResetExplosion);
	mp_GUI->addParam("preview", &m_FBO.getTexture(1));
	
	m_PrevModelView = ci::Matrix44f::identity();
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void ShaderTestApp::resize(cinder::app::ResizeEvent event)
{
	m_Cam.setAspectRatio(event.getAspectRatio());
//	m_Cam.lookAt(Vec3f(0,-1,7), Vec3f::zero());
	
	//m_RecreateFBO = true;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void ShaderTestApp::draw()
{
	if(m_RecreateFBO)
	{
		gl::Fbo::Format fmt;
		fmt.enableColorBuffer(true, 2);
		m_FBO = gl::Fbo(getWindowWidth(), getWindowHeight(), fmt);
	}

	curr_time = (float)getElapsedSeconds();

	//light_pos = Vec4f(-light_dist * sin(light_angle), 0.0f, light_dist * cos(light_angle), 1.0f);

	light_pos_0 = Vec4f(	-light_dist * sin(curr_time), 
		light_dist * sin(curr_time * 1.43f), 
		light_dist * cos(curr_time), 
		1.0f);

	light_pos_1 = Vec4f(	-light_dist * sin(curr_time * 1.34f), 
		light_dist * sin(curr_time * 0.43f), 
		light_dist * cos(curr_time * 1.34f), 
		1.0f);

	//light_pos = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		
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

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void ShaderTestApp::BindShaderAndSetUniforms(gl::GlslProg& shader)
{
	shader.bind();
	
	shader.uniform("time", curr_time);	
	shader.uniform("explode", explode);
	
	shader.uniform("resolution", resolution);
	
	shader.uniform("fresnel_power", fresnel_power);	
	shader.uniform("fresnel_intensity", fresnel_intensity);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
	
	float dist = 6.0f - explode * 3.0f;
	float rot = (float)M_PI * 2.0f * pow(explode, 2.0f);
	
	resolution = getWindowSize();
	
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHT1 );
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	gl::clear();

	gl::pushMatrices();

	SetupLightsAndMaterials();


	//m_Cam.lookAt(Vec3f(-dist*sin(rot), 1, dist*cos(rot)), Vec3f::zero(), Vec3f(0,-1,0));
	m_Cam.lookAt(Vec3f(0, 0, dist), Vec3f::zero(), Vec3f(0,1,0));

	gl::setMatrices(m_Cam);

	Matrix44f cam_mat =	m_Cam.getModelViewMatrix();

	gl::pushModelView();


	//ci::Matrix44f model_view = ci::Matrix44f::createRotation(Vec3f(0,1,0), 0.0f);
	//m_PrevModelView = ci::Matrix44f::createRotation(Vec3f(0,1,0), -0.5f);
	Matrix44f model_matrix = ci::Matrix44f::createRotation(Vec3f(0.0f, 1.0f, 0.0f).normalized(), 0.1f *  curr_time * (float)M_PI);
	//Matrix44f model_view = ci::Matrix44f::createRotation(Vec3f(0.0, 1.0f, 0.0f).normalized(), powf(sin(1.5f *  curr_time), 3.0f) * M_PI * 0.5f);
	model_matrix = Matrix44f::identity();
	model_matrix = ci::Matrix44f::createRotation(Vec3f(0.0f, 0.0f, 1.0f), (float)M_PI) * ci::Matrix44f::createRotation(Vec3f(0.0f, 1.0f, 0.0f), 0.73f * curr_time);

	gl::multModelView(model_matrix);

	
	//SetupLightsAndMaterials();


	///////////////////////////////////////////////////////////
	//Set unifroms and draw mesh
	BindShaderAndSetUniforms(m_Shader);

	m_Shader.uniform("previous_model_view", cam_mat * m_PrevModelView);
	m_Shader.uniform("model_view", cam_mat * model_matrix);

	m_Shader.uniform("ModelMatrix", model_matrix);

	m_Shader.uniform("ExtinctionCoefficient", extinction_coefficient);
	m_Shader.uniform("MaterialThickness", thickness);
	m_Shader.uniform("RimScalar", fresnel_intensity);
	m_Shader.uniform("RimPower", fresnel_power);

	m_Shader.uniform("SpecularIntensity", specular_intensity);
	m_Shader.uniform("SpecularPower", shininess);
	//m_Shader.uniform("SpecularIntensity", fresnel_intensity);
	
	m_Shader.uniform("particle", false);
	
	gl::draw(m_Mesh);

	///////////////////////////////////////////////////////////
	//Draw particles...
	
	m_Shader.uniform("particle", true);

	//DrawParticles();
	
	m_Shader.unbind();

	glDisable(GL_LIGHTING);

	gl::color(light_color_0);
	gl::drawSphere(light_pos_0.xyz(), 0.1f);

	gl::color(light_color_1);
	gl::drawSphere(light_pos_1.xyz(), 0.1f);

	gl::popModelView();
	
	m_PrevModelView = model_matrix;

	gl::popMatrices();
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void ShaderTestApp::PostProcess()
{	
	m_PostProcessShader.bind();
	m_PostProcessShader.uniform("velocity_tex", 1);
	//m_PostProcessShader.uniform("time", time);	

	gl::clear();
	gl::color(1, 1, 1, 1);
	
	gl::setMatricesWindow(getWindowWidth(), getWindowHeight());

	glEnable(GL_TEXTURE_2D);
	m_FBO.getTexture(0).bind(0);
	m_FBO.getTexture(1).bind(1);
	//m_FBO.bindTexture(0,0);
	//m_FBO.bindTexture(0,1);
	gl::draw(m_FBO.getTexture(0), Rectf(0, 0, (float)getWindowWidth(), (float)getWindowHeight()));
	//gl::draw(m_FBO.getTexture(0));
	m_FBO.unbindTexture();
		
	m_PostProcessShader.unbind();
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void ShaderTestApp::SetupLightsAndMaterials()
{
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos_0.ptr() );
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color_0.ptr());

	glLightfv(GL_LIGHT0, GL_SPECULAR, ColorA(1.0, 1.0, 1.0, 1.0));

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos_1.ptr() );
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_color_1.ptr());

	glLightfv(GL_LIGHT1, GL_SPECULAR, ColorA(1.0, 1.0, 1.0, 1.0));

	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color);

	ColorA ambient_color = (diffuse_color * ColorA(0.5f, 0.5f, 1.0f, 1.0f)) * 0.1f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_color);

	glMaterialfv(GL_FRONT, GL_SPECULAR, ColorA(1, 0.7, 0.9,specular_intensity));
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

CINDER_APP_BASIC( ShaderTestApp, RendererGl )