#include "mesh.h"
#include <sstream>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <math.h>
#include <array>
#include <vector>
using namespace std;

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <gl/GL.h>
#endif

My_Mesh::My_Mesh()
{
	vTranslation[0] = Theta[0] = 0;
	vTranslation[1] = Theta[1] = 0;
	vTranslation[2] = Theta[2] = 0;
	Theta[0] = 45;
}

My_Mesh::~My_Mesh()
{
}
int My_Mesh::findKey(string str, char key) {
    int value = 0;
    for (int i = 0; i < str.length(); i++)
        if (str[i] == key)
            value++;
    return value;
}

void My_Mesh::load_obj(std::string obj_File,float scale)
{
	// 实现 obj文件的读取
	if(obj_File.empty()) return;	//假如文件名为空则不执行

	std::ifstream in;	//使用文件流读取
	in.open(obj_File.c_str(), std::ios::in);

	string sign;
	float x,y,z;
	unsigned int ix,iy,iz;
	char c;
	
	/*******************清除数据并且设置中心和边框大小**********************/
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-scale, -scale,-scale);
	this->m_max_box_ = point3f(scale, scale, scale);

	string line;
	string str;
	/*********************开始读取数据**************************/
	while(!in.eof())
	{
		//获取每行数据
		getline(in,line);
		if(line.size() == 0) continue;
		if(line[0] == '#') 	 continue;
		else if(line[0] == 'v')
		{
			if(line[1] == 't')	//vt坐标 //存储点的贴图坐标
				{
					istringstream IL(line);
					IL >> str >> x >> y >> z;
					m_vt_list_.push_back(x);
					m_vt_list_.push_back(y);
				}
			else if(line[1] == 'n')	//vn坐标 //存储点的法线数据
				{
					istringstream IL(line);
					IL >> str >> x >> y >> z;
					m_normals_.push_back(x);
					m_normals_.push_back(y);
					m_normals_.push_back(z);
					float r;
					float g;
					float b;
					My_Mesh::normal_to_color(x, y, z, r, g, b);	//存储顶点颜色数据
					m_color_list_.push_back(r);
					m_color_list_.push_back(g);
					m_color_list_.push_back(b);
				}
			else	//v定点坐标 //存储顶点坐标
				{
					istringstream IL(line);
					IL >> str >> x >> y >> z;
					m_vertices_.push_back(x);
					m_vertices_.push_back(y);
					m_vertices_.push_back(z);
				}
			
		}
		else if(line[0] == 'f'){	//存储片元信息
			cout<<"sdd"<<endl;
			istringstream IL(line);
			 if (My_Mesh::findKey(line,'/') == 6) {      //  格式为V1/VT1/VN1 V2/VT2/VN2 V3/VT3/VN3
			 	
                IL >> str;
				for(int i = 0;i < 3;i++)
				{	
					IL  >> ix >> c>> iy >> c >>iz;
					m_faces_.push_back(ix-1);
					m_faces_.push_back(iy-1);
					m_faces_.push_back(iz-1); //"f 顶点索引/uv点索引/法线索引"。
				}
            }
            else if (findKey(line, '/') == 3) {      //格式为 V1/T1 V2/T2 V3/T3
                ;
            }
            else if (findKey(line, '/') == 0) {      //格式为 V1 V2 V3
               ;
            }
            else if (findKey(line, '//') == 3) {     //格式为 V1//N1 V2//N2 V3//N3
                ;
            }
			
		}
	}
	cout << m_faces_.size()<<endl;
	cout << m_vertices_.size()<<endl;
	in.close();	
	/*********************读取数据完毕**************************/

};

void My_Mesh::normal_to_color(float nx, float ny, float nz, float& r, float& g, float& b)
{
	r = float(std::min(std::max(0.5 * (nx + 1.0), 0.0), 1.0));
	g = float(std::min(std::max(0.5 * (ny + 1.0), 0.0), 1.0));
	b = float(std::min(std::max(0.5 * (nz + 1.0), 0.0), 1.0));
};

const VtList&  My_Mesh::get_vts()
{
	return this->m_vt_list_;
};

void My_Mesh::clear_data()
{
	m_vertices_.clear();
	m_normals_.clear();
	m_faces_.clear();
	m_color_list_.clear();
	m_vt_list_.clear();
};

void My_Mesh::get_boundingbox(point3f& min_p, point3f& max_p) const
{
	min_p = this->m_min_box_;
	max_p = this->m_max_box_;
};

const STLVectorf&  My_Mesh::get_colors()
{
	return this->m_color_list_;
};

const VertexList& My_Mesh::get_vertices()
{
	return this->m_vertices_;
};

const NormalList& My_Mesh::get_normals()
{
	return this->m_normals_;
};

const FaceList&   My_Mesh::get_faces()
{
	return this->m_faces_;
};

int My_Mesh::num_faces()
{
	return this->m_faces_.size()/9;
};

int My_Mesh::num_vertices()
{
	return this->m_vertices_.size()/3;
};

const point3f& My_Mesh::get_center()
{
	return this->m_center_;
};

void My_Mesh::generate_cylinder(int num_division, float height)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, -height);
	this->m_max_box_ = point3f(1, 1, height);

	int num_samples = num_division;
	float z = -height;
	float pi = 3.14159265;
	float step = 1.0 * 360 / num_samples;
	float rr = pi / 180;
	//圆柱体Z轴向上，按cos和sin生成x，y坐标
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}

	z = height;
	//圆柱体Z轴向上，按cos和sin生成x，y坐标，
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
		//这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
	}

	for (int i = 0; i < num_samples; i++)
	{
		m_faces_.push_back(i);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples) % (num_samples)+num_samples);

		m_faces_.push_back((i + num_samples) % (num_samples)+num_samples);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples + 1) % (num_samples)+num_samples);
		//生成三角面片

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标
	}

};

void My_Mesh::set_texture_file(std::string s)
{
	this->texture_file_name = s;
};

std::string My_Mesh::get_texture_file()
{
	return this->texture_file_name;
};

void My_Mesh::set_translate(float x, float y, float z)
{
	vTranslation[0] = x;
	vTranslation[1] = y;
	vTranslation[2] = z;

};
void My_Mesh::get_translate(float& x, float& y, float& z)
{
	x = vTranslation[0];
	y = vTranslation[1];
	z = vTranslation[2];
};

void My_Mesh::set_theta(float x, float y, float z)
{
	Theta[0] = x;
	Theta[1] = y;
	Theta[2] = z;
};
void My_Mesh::get_theta(float& x, float& y, float& z)
{
	x = Theta[0];
	y = Theta[1];
	z = Theta[2];
};

void My_Mesh::set_theta_step(float x, float y, float z)
{
	Theta_step[0] = x;
	Theta_step[1] = y;
	Theta_step[2] = z;
};

void My_Mesh::add_theta_step()
{
	Theta[0] = Theta[0] + Theta_step[0];
	Theta[1] = Theta[1] + Theta_step[1];
	Theta[2] = Theta[2] + Theta_step[2];
};