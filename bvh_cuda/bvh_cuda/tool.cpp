#include "tool.h"


void CompactMatrix(MatrixXf& M, float* target)
{
	for (int i = 0; i < M.rows(); i++)
	{
		for (int j = 0; j < M.cols(); j++)
		{
			target[i*M.cols() + j] = M(i, j);

		}
	}
}

void UnfoldMatrix(MatrixXf&M, float* source)
{
	for (int i = 0; i < M.rows(); i++)
	{
		for (int j = 0; j < M.cols(); j++)
		{
			M(i, j) = source[i*M.cols() + j];
		}
	}
}

void createVBO(GLuint* vbo, int N, struct cudaGraphicsResource*& cuda_vbo_resource)
{
	if (vbo)
	{
		glGenBuffers(1, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, *vbo);
		
		unsigned int size = N * sizeof(float) * 3;
		glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		cudaGraphicsGLRegisterBuffer(&cuda_vbo_resource, *vbo, cudaGraphicsMapFlagsWriteDiscard);	
		printf("Create VBO Success! \n");
	}
	else printf("Create VBO failed! \n");
}