#pragma once
#ifndef SKELETON_H
#define SKELETON_H

#include <glew.h>
#include <freeglut.h>
#include "joint.h"
#include "tool.h"
#include "lbs.h"
#include "cuda_runtime.h"
#include "cuda_gl_interop.h"
#include "device_launch_parameters.h"

class Skeleton
{
public:
	Skeleton();
	~Skeleton();

	void readBVH(string filename);
	void readInfo(string filename);

	void updateJoint(int _frame_index);
	void updateJoint();
	void updateMesh();
	void updateMeshCPU();

	void mallocSpace();
	void freeSpace();

	void drawMeshWire();
	void drawMeshFace();


public:
	int JOINT_COUNT;
	vector<Joint*> joints;

	MatrixXf motion_block;

	MatrixXf mesh;
	MatrixXf vertices;
	MatrixXf handles;
	MatrixXf weight_map;
	MatrixXf transformation;
	MatrixXi faces;

	GLuint VBO = NULL;
	GLuint VAO[2] = { NULL };
	GLuint EBO[2] = { NULL };

	struct cudaGraphicsResource* cuda_vbo_resource;

	float* mesh_dev, *handles_dev, *weight_map_dev, *transformation_dev;
	float* mesh_host, *handles_host, *weight_map_host, *transformation_host;
	unsigned int* face_index, *wire_index;


	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};


#endif