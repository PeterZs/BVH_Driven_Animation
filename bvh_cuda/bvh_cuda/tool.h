#pragma once

#ifndef TOOL_H
#define TOOL_H

#include <glew.h>
#include <freeglut.h>
#include <Eigen/Dense>
#include <stdio.h>
#include <stdlib.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_gl_interop.h"

using namespace std;
using namespace Eigen;

void CompactMatrix(MatrixXf& M, float* target);
void UnfoldMatrix(MatrixXf&M, float* source);
void createVBO(GLuint* vbo, int N, struct cudaGraphicsResource*& cuda_vbo_resource);

#endif