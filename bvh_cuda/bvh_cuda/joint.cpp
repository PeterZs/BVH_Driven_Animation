#include "joint.h"

Joint::Joint(string _name, Vector3d _offset, int channel_num)
{
	name = _name;
	offset = _offset;
	channels.resize(channel_num);

	transform = Matrix4d::Identity();
	L2P = Matrix4d::Identity();
	L2W = Matrix4d::Identity();


	father = nullptr;
	position = _offset;
	mesh_position = _offset; //root
}


Joint::Joint(Joint* _father, string _name, Vector3d _offset, int channel_num)
{
	name = _name;
	offset = _offset;
	channels.resize(channel_num);

	transform = Matrix4d::Identity();
	L2P = Matrix4d::Identity();
	L2W = Matrix4d::Identity();


	father = _father;
	father->addChild(this);
	position = father->position + offset;
	mesh_position = father->mesh_position + offset;
}

Joint::~Joint() {}

void Joint::addChild(Joint* child)
{
	children.push_back(child);
}

Matrix4d Joint::getTransform()
{
	if (channels.size() == 0)
	{
		transform = Matrix4d::Identity();
	}
	else
	{
		Matrix4d RotX, RotY, RotZ;
		double x = channels[X_ROTATION];
		double y = channels[Y_ROTATION];
		double z = channels[Z_ROTATION];

		RotX << 1, 0, 0, 0,
			0, cos(x), -sin(x), 0,
			0, sin(x), cos(x), 0,
			0, 0, 0, 1;
		RotY << cos(y), 0, sin(y), 0,
			0, 1, 0, 0,
			-sin(y), 0, cos(y), 0,
			0, 0, 0, 1;
		RotZ << cos(z), -sin(z), 0, 0,
			sin(z), cos(z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		transform = RotZ * RotX * RotY;
	}
	transform = transform;
	return transform;
}

Matrix4d Joint::getL2P()
{
	getTransform();
	Matrix4d M;
	if (father != nullptr)
	{
		M << transform(0, 0), transform(0, 1), transform(0, 2), offset.x(),
			transform(1, 0), transform(1, 1), transform(1, 2), offset.y(),
			transform(2, 0), transform(2, 1), transform(2, 2), offset.z(),
			0, 0, 0, 1;
	}
	else
	{
		M << transform(0, 0), transform(0, 1), transform(0, 2), offset.x() + channels[X_POSITION],
			transform(1, 0), transform(1, 1), transform(1, 2), offset.y() + channels[Y_POSITION],
			transform(2, 0), transform(2, 1), transform(2, 2), offset.z() + channels[Z_POSITION],
			0, 0, 0, 1;
	}
	L2P = M;

	return L2P;
}

Matrix4d Joint::getL2W()
{
	L2W = Matrix4d::Identity();
	Joint* current = this;
	vector<Joint*> stack;
	while (current != nullptr)
	{
		stack.push_back(current);
		current = current->father;
	}
	for (int i = stack.size() - 1; i >= 0; i--)
	{
		L2W = L2W * stack[i]->getL2P();
	}
	return L2W;
}

Vector3d Joint::getWorldPosition()
{
	getL2W();
	Vector4d coordinate(0, 0, 0, 1);
	coordinate = L2W * coordinate;

	position = Vector3d(coordinate(0), coordinate(1), coordinate(2));
	return position;
}


void Joint::draw()
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i]->channels.size() > 0)
		{
			glColor3d(1, 0, 0);
			glBegin(GL_LINES);
			glVertex3d(position.x(), position.y(), position.z());
			glVertex3d(children[i]->position.x(), children[i]->position.y(), children[i]->position.z());
			glEnd();

			children[i]->draw();
		}
	}
}