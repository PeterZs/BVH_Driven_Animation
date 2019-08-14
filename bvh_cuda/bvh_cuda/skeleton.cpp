#include "skeleton.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "lbs.h"

Skeleton::Skeleton() {}

Skeleton::~Skeleton() {}

void Skeleton::readBVH(string filename)
{
	ifstream in(filename);

	string token = "";
	string name = "";

	Joint* new_joint = NULL;
	vector<Joint*> stack;
	vector<Joint*> temp_joints;
	int channel_num = 0;
	double x = 0, y = 0, z = 0;


	while (in >> token)
	{
		if ((token == "JOINT" || token == "End"))
		{
			if (name != "END!")
			{
				if (stack.size() != 0)
				{
					new_joint = new Joint(stack[stack.size() - 1], name, Vector3d(x, y, z), channel_num);
					temp_joints.push_back(new_joint);
					stack.push_back(new_joint);
					name = ""; x = y = z = channel_num = 0;
				}
				else
				{
					name = "ROOT";
					new_joint = new Joint(name, Vector3d(x, y, z), channel_num);
					temp_joints.push_back(new_joint);
					stack.push_back(new_joint);
					name = ""; x = y = z = channel_num = 0;
				}
			}

			in >> name;

			if (token == "End")
			{
				string temp;
				in >> temp; in >> temp; //{ OFFSET
				in >> x >> y >> z;
				channel_num = 0;
				new_joint = new Joint(stack[stack.size() - 1], name, Vector3d(x, y, z), channel_num);
				temp_joints.push_back(new_joint);
				stack.push_back(new_joint);
				name = "END!"; x = y = z = channel_num = 0;
			}
		}

		if (token == "OFFSET")
		{
			in >> x >> y >> z;
		}

		if (token == "CHANNELS")
		{
			in >> channel_num;
		}

		if (token == "}")
		{
			stack.pop_back();
		}

		if (token == "MOTION") break;
	}

	int channel_sum = 0;
	for (unsigned int i = 0; i < temp_joints.size(); i++)
	{
		channel_sum += temp_joints[i]->channels.size();
	}

	for (unsigned int i = 0; i < temp_joints.size(); i++)
	{
		if (temp_joints[i]->channels.size() > 0)
		{
			joints.push_back(temp_joints[i]);
		}
	}
	JOINT_COUNT = joints.size();

	handles.resize(joints.size(), 3);
	for (int i = 0; i < joints.size(); i++)
	{
		handles(i, 0) = joints[i]->mesh_position.x();
		handles(i, 1) = joints[i]->mesh_position.y();
		handles(i, 2) = joints[i]->mesh_position.z();
	}

	in >> token;
	if (token != "Frames:") cout << "The BVH File Format Wrong!" << endl;
	int frame_num; in >> frame_num;
	motion_block.resize(frame_num, channel_sum);

	in >> token; in >> token; in >> token;
	for (int i = 0; i < motion_block.rows(); i++)
	{
		for (int j = 0; j < motion_block.cols(); j++)
		{
			in >> motion_block(i, j);
		}
	}
	cout << "Read BVH file Success!" << endl;
}

void Skeleton::readInfo(string filename)
{
	ifstream in(filename);
	Matrix<double, Dynamic, Dynamic> C;
	Matrix<int, Dynamic, Dynamic> E;

	int rows, cols;

	in >> rows >> cols;
	cout << "Vertices: " << rows << " " << cols << endl;
	mesh.resize(rows, cols);
	vertices.resize(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			in >> mesh(i, j);
		}
	}
	vertices = mesh;

	in >> rows >> cols;
	cout << "Surfaces: " << rows << "  " << cols << endl;
	faces.resize(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			in >> faces(i, j);
		}
	}

	in >> rows >> cols;
	cout << "Handles: " << rows << " " << cols << endl;
	C.resize(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			in >> C(i, j);
		}
	}

	in >> rows >> cols;
	cout << "Edeges: " << rows << " " << cols << endl;
	E.resize(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			in >> E(i, j);
		}
	}

	in >> rows >> cols;
	cout << "WeightMap: " << rows << " " << cols << endl;
	weight_map.resize(rows, cols);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			in >> weight_map(i, j);
		}
	}

	cout << "Read Info Success! " << endl;
}

void Skeleton::updateJoint(int _frame_index)
{
	int count = 0;
	int frame_index = _frame_index;
	for (unsigned int i = 0; i < joints.size(); i++)
	{
		if (joints[i]->channels.size() == 0) continue;

		if (i == 0)
		{
			joints[i]->channels[X_POSITION] = motion_block(frame_index, count); count++;
			joints[i]->channels[Y_POSITION] = motion_block(frame_index, count); count++;
			joints[i]->channels[Z_POSITION] = motion_block(frame_index, count); count++;

			joints[i]->channels[Z_ROTATION] = motion_block(frame_index, count++)*3.1415926535 / 600;
			joints[i]->channels[Y_ROTATION] = motion_block(frame_index, count++)*3.1415926535 / 600;
			joints[i]->channels[X_ROTATION] = motion_block(frame_index, count++)*3.1415926535 / 600;

		}
		else if (i < 4)
		{
			joints[i]->channels[Z_ROTATION] = motion_block(frame_index, count)*3.1415926535 / 600; count++;
			joints[i]->channels[X_ROTATION] = motion_block(frame_index, count)*3.1415926535 / 600; count++;
			joints[i]->channels[Y_ROTATION] = motion_block(frame_index, count)*3.1415926535 / 600; count++;
		}
		else
		{
			joints[i]->channels[Z_ROTATION] = motion_block(frame_index, count)*3.1415926535 / 180; count++;
			joints[i]->channels[X_ROTATION] = motion_block(frame_index, count)*3.1415926535 / 180; count++;
			joints[i]->channels[Y_ROTATION] = motion_block(frame_index, count)*3.1415926535 / 180; count++;
		}
	}
	for (unsigned int i = 0; i < joints.size(); i++)
	{
		joints[i]->getWorldPosition();
	}

	transformation.resize(joints.size() * 4, 4);
	for (int i = 0; i < joints.size(); i++)
	{
		Matrix4d L2W = joints[i]->L2W;
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				transformation(4 * i + j, k) = L2W(j, k);
			}
		}
	}
}

void Skeleton::updateJoint()
{
	for (unsigned int i = 0; i < joints.size(); i++)
	{
		joints[i]->getWorldPosition();
	}


	transformation.resize(joints.size() * 4, 4);
	for (int i = 0; i < joints.size(); i++)
	{
		Matrix4d L2W = joints[i]->L2W;
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				transformation(4 * i + j, k) = L2W(j, k);
			}
		}
	}
}

void Skeleton::drawMeshWire()
{
	GLfloat mat[] = { 1,1,1,1 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
	glLineWidth(1.5f);
	glBindVertexArray(VAO[0]);
	glDrawElements(GL_LINES, faces.rows()*6, GL_UNSIGNED_INT, 0);
}

void Skeleton::drawMeshFace()
{
	GLfloat mat[] = { 0,0,0,1 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
	glBindVertexArray(VAO[1]);
	glDrawElements(GL_TRIANGLES, faces.rows()*faces.cols(), GL_UNSIGNED_INT, 0);

}

void Skeleton::mallocSpace()
{
	int N = vertices.rows();
	int M = handles.rows();

	cudaSetDevice(0);

	mesh_host = (float*)malloc(sizeof(float)*N * 3);
	handles_host = (float*)malloc(sizeof(float)*M * 3);
	transformation_host = (float*)malloc(4 * M * 4 * sizeof(float));
	weight_map_host = (float*)malloc(sizeof(float)*M*N);
	face_index = (unsigned int*)malloc(sizeof(unsigned int)*faces.rows()*faces.cols());
	wire_index=(unsigned int*)malloc(sizeof(unsigned int)*faces.rows()*6);

	CompactMatrix(mesh, mesh_host);						// N*3
	CompactMatrix(handles, handles_host);				// M*3 
	CompactMatrix(weight_map, weight_map_host);			// N*M
	
	for (int i = 0; i < faces.rows(); i++)
	{
		for (int j = 0; j < faces.cols(); j++)
		{
			face_index[i*faces.cols() + j] = faces(i, j);
		}
	}

	for (int i = 0; i < faces.rows(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			wire_index[6 * i + 0] = faces(i, 0);
			wire_index[6 * i + 1] = faces(i, 1);
			wire_index[6 * i + 2] = faces(i, 0);
			wire_index[6 * i + 3] = faces(i, 2);
			wire_index[6 * i + 4] = faces(i, 1);
			wire_index[6 * i + 5] = faces(i, 2);
		}
	}

	glGenVertexArrays(1, &VAO[0]);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO[0]);
	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, N * sizeof(float) * 3, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*faces.rows()*6, wire_index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO[1]);
	glGenBuffers(1, &EBO[1]);
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, N * sizeof(float) * 3, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*faces.rows() * faces.cols(), face_index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cudaGraphicsGLRegisterBuffer(&cuda_vbo_resource, VBO, cudaGraphicsMapFlagsWriteDiscard);

	mallocSpaceInCuda(mesh_host, handles_host, transformation_host, weight_map_host,
		mesh_dev, handles_dev, transformation_dev, weight_map_dev, N, M);

}

void Skeleton::freeSpace()
{
	int N = vertices.rows();
	int M = handles.rows();

	freeSpaceInCuda(mesh_host, handles_host, transformation_host, weight_map_host,
		mesh_dev, handles_dev, transformation_dev, weight_map_dev, N, M);
}

void Skeleton::updateMesh()
{
	int N = vertices.rows();
	int M = handles.rows();

	CompactMatrix(transformation, transformation_host);
	LBS(mesh_dev, handles_dev, transformation_dev, weight_map_dev, transformation_host, N, M, cuda_vbo_resource);

}

