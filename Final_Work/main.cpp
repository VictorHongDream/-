// rotating cube with two texture objects
// change textures with 1 and 2 keys

#include "Angel.h"
#include "mesh.h"
#include "Mesh_Painter.h"
#include <string>

// #pragma comment(lib, "glew32.lib")
// #pragma comment(lib, "FreeImage.lib")

// 窗口大小
struct Window
{
	int width = 500;
	int height = 500;
};
Window window;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// 存储要贴纹理的物体
std::vector<My_Mesh*>	my_meshs;
// 存储纹理图片
Mesh_Painter*			mp_;

void init()
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void display(void)
{
#ifdef __APPLE__ // 解决 macOS 10.15 显示画面缩小问题
	glViewport(0, 0, window.width * 2, window.height * 2);
#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mp_->draw_meshes();
	glutSwapBuffers();
};

void mouse( int button, int state, int x, int y )
{
}

void idle(void)
{
	glutPostRedisplay();
}

void keyboard( unsigned char key, int mousex, int mousey )
{
	switch (key)
	{
	case 033: // ESC键退出
		exit(EXIT_SUCCESS);
		break;
	}
    glutPostRedisplay();
}

int main( int argc, char **argv )
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window.width, window.height);
	glutCreateWindow("2017153040_吴嘉鸿_作业四");

	#ifdef WIN32
		glutInitContextVersion(3, 2);
		glutInitContextProfile(GLUT_CORE_PROFILE);
		glewExperimental = GL_TRUE;
		glewInit();
	#endif

	init();

	mp_ = new Mesh_Painter;
	
	// 读取wawa.obj和table.obj模型并贴图、设置绕y轴旋转
	My_Mesh* my_mesh1 = new My_Mesh;
	my_mesh1->load_obj("texture/wawa.obj",5);			// 读取娃娃模型
	my_mesh1->set_texture_file("texture/wawa.png"); 	// 读取娃娃纹理图像
	my_mesh1->set_translate(-0.5, -0.40, 0);			// 平移物体到合适的位置
	my_mesh1->set_theta(90, 0.0, 0.0);					// 设置旋转轴为y轴
	my_mesh1->set_theta_step(0, 0.9, 0);				// 设置旋转速度
	my_meshs.push_back(my_mesh1);
	mp_->add_mesh(my_mesh1);

	My_Mesh* my_mesh2 = new My_Mesh;
	my_mesh2->load_obj("texture/table.obj",0.35);		// 读取桌子模型
	my_mesh2->set_texture_file("texture/table.png");	// 读取桌子纹理图像
	my_mesh2->set_translate(0.4, -0.05, 0);				// 平移物体
	my_mesh2->set_theta(90.0, 0.0, 0.0);				// 旋转轴为y轴
	my_mesh2->set_theta_step(0, 0.8, 0);				// 设置旋转速度
	my_meshs.push_back(my_mesh2);
	mp_->add_mesh(my_mesh2);							//添加模型类

	std::string vshader, fshader;
	#ifdef __APPLE__
		vshader = "shaders/vshader_mac.glsl";
		fshader = "shaders/fshader_mac.glsl";
	#else
		vshader = "shaders/vshader_win.glsl";
		fshader = "shaders/fshader_win.glsl";
	#endif
	mp_->init_shaders(vshader.c_str(), fshader.c_str());
	mp_->update_vertex_buffer();
	mp_->update_texture();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	glutMainLoop();

	for (unsigned int i = 0; i < my_meshs.size(); i++)
	{
		delete my_meshs[i];
	}
	delete mp_;

	return 0;
}
