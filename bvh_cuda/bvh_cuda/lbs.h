#pragma once
#ifndef LBS_H
#define LBS_H

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda.h"
#include <freeglut.h>
#include "cuda_gl_interop.h"
#include <stdlib.h>
#include <stdio.h>


extern class Skeleton;

__global__ void blendVertex(float* mesh, float3* vertices, float* handles, float* transformation, float* weight_map, int N, int M);

void LBS(float* mesh_dev, float* handles_dev, float* transformation_dev, float* weight_map_dev, float* transformation_host, int N, int M,struct cudaGraphicsResource*& cuda_vbo_resource);

void mallocSpaceInCuda(float*& mesh_host, float*& handles_host, float*& transformation_host, float*& weight_map_host,
	float*& mesh_dev, float*& handles_dev, float*& transformation_dev, float*& weight_map_dev,
	int N, int M);

void freeSpaceInCuda(float* mesh_host, float* handles_host, float* transformation_host, float* weight_map_host,
	float* mesh_dev, float* handles_dev, float* transformation_dev, float* weight_map_dev,
	int N, int M);

#endif