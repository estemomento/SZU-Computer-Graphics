//  rotating cube with two texture objects
//  change textures with 1 and 2 keys

#include "Angel.h"
#include "mesh.h"
#include "FreeImage.h"
#include "Mesh_Painter.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;


std::vector<My_Mesh*> my_meshs;
Mesh_Painter*			mp_;
//  Texture objects and storage for texture image



//  Vertex data arrays

//  Array of rotation angles (in degrees) for each coordinate axis

/*
 ----------------------------------------------------------------------------
 ----------------------------------------------------------------------------
 */
void
init()
{
	glEnable(GL_DEPTH_TEST);
    glEnable( GL_DEPTH_TEST );
	glEnable(GL_LIGHTING);
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mp_->draw_meshes();
	glutSwapBuffers();
};

 //----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
    
}

 //----------------------------------------------------------------------------
void
idle(void)
{
	glutPostRedisplay();
}

 //----------------------------------------------------------------------------

void
keyboard( unsigned char key, int mousex, int mousey )
{
   
    glutPostRedisplay();
}

 //----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "2015160196_刘畅宇_实验4" );
	glewExperimental = GL_TRUE;
    glewInit();

    init();

	
	mp_ = new Mesh_Painter;

  	My_Mesh* my_mesh1 = new My_Mesh;
	my_mesh1->load_obj("texture/table.obj");
	my_mesh1->set_texture_file("texture/table.png");
  	my_mesh1->set_translate(0.5, 0, 0); //移动到头顶
  	my_mesh1->set_theta(90, 0., 0.);
  	my_mesh1->set_theta_step(0, 1, 0);
  	mp_->add_mesh(my_mesh1);
  	my_meshs.push_back(my_mesh1);
  
  	My_Mesh* my_mesh2 = new My_Mesh;
	my_mesh2->load_obj("texture/wawa.obj");
	my_mesh2->set_texture_file("texture/wawa.png");
  	my_mesh2->set_translate(-0.5, 0, 0);
  	my_mesh2->set_theta(90, 0., 0.);
  	my_mesh2->set_theta_step(0, 1, 0); 
  	my_meshs.push_back(my_mesh2);
  	mp_->add_mesh(my_mesh2);

	mp_->init_shaders("v_texture.glsl","f_texture.glsl");
	mp_->update_vertex_buffer();
	mp_->update_texture();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouse );
    //glutIdleFunc( idle );

    glutMainLoop();



	for (unsigned int i = 0; i < my_meshs.size(); i++)
	{
		delete my_meshs[i];
	}
	delete mp_;

    return 0;
}
