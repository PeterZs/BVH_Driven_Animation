#pragma once
#ifndef JOINT_H
#define JOINT_H

#include <Eigen/Dense>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iostream>
#include <freeglut.h>

using namespace std;
using namespace Eigen;

enum ChannelEnum
{
	X_ROTATION, Y_ROTATION, Z_ROTATION,
	X_POSITION, Y_POSITION, Z_POSITION
};// Joint allocate vector for channels in order

typedef double channel;


class Joint
{
public:
	Joint(string _name, Vector3d _offset, int channel_num);
	Joint(Joint* _father, string _name, Vector3d _offset, int channel_num);
	~Joint();

	void addChild(Joint* child);
	Matrix4d getTransform();
	Matrix4d getL2W();
	Matrix4d getL2P();
	Vector3d getWorldPosition();

	void draw();

public:
	string name;
	Vector3d offset;
	vector<double> channels;

	Matrix4d transform;
	Matrix4d L2P;
	Matrix4d L2W;

	Joint* father;
	vector<Joint*> children;

	Vector3d position;
	Vector3d mesh_position;
	int index = 0;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


#endif