#include "lbs.h"

__global__ void blendVertex(float* mesh, float3* vertices, float* handles, float* transformation, float* weight_map, int N, int M)
{
	int index = threadIdx.x + blockDim.x*blockIdx.x; //Calculate vertex index
	//printf("%d ", index);
	if (index >= N) return;

	float sum_weight = 0;
	for (int i = 0; i < M; i++)
	{
		sum_weight += weight_map[index*M + i];
	}
	float sum[4] = { 0 };

	for (int i = 0; i < M; i++)
	{
		float offset[4], coordinate[4] = { 0 };
		float L2W[4][4];

		offset[0] = mesh[index * 3 + 0] - handles[i * 3 + 0];
		offset[1] = mesh[index * 3 + 1] - handles[i * 3 + 1];
		offset[2] = mesh[index * 3 + 2] - handles[i * 3 + 2];
		offset[3] = 1;
		//transformation 4m*4

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				L2W[j][k] = transformation[(4 * i + j) * 4 + k];
			}
		}

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				coordinate[j] += L2W[j][k] * offset[k];
			}
		}

		sum[0] += coordinate[0] * weight_map[index*M + i] / sum_weight;
		sum[1] += coordinate[1] * weight_map[index*M + i] / sum_weight;
		sum[2] += coordinate[2] * weight_map[index*M + i] / sum_weight;
		sum[3] += coordinate[3] * weight_map[index*M + i] / sum_weight;
	}

	//vertices[index * 3 + 0] = sum[0];
	//vertices[index * 3 + 1] = sum[1];
	//vertices[index * 3 + 2] = sum[2];
	vertices[index] = { sum[0],sum[1],sum[2] };
}


void LBS(float* mesh_dev, float* handles_dev, float* transformation_dev, float* weight_map_dev,
	float* transformation_host, int N, int M, struct cudaGraphicsResource*& cuda_vbo_resource)
{

	cudaMemcpy(transformation_dev, transformation_host, 4 * M * 4 * sizeof(float), cudaMemcpyHostToDevice);

	float3* dptr = NULL;
	size_t num_bytes;

	cudaGraphicsMapResources(1, &cuda_vbo_resource, 0);
	cudaGraphicsResourceGetMappedPointer((void**)&dptr, &num_bytes, cuda_vbo_resource);

	dim3 block(128);
	dim3 grid((N - 1) / 128 + 1);

	blendVertex << <grid, block >> > (mesh_dev, dptr, handles_dev, transformation_dev, weight_map_dev, N, M);


	cudaDeviceSynchronize();

	cudaGraphicsUnmapResources(1, &cuda_vbo_resource,0);

}

void mallocSpaceInCuda(float*& mesh_host,  float*& handles_host, float*& transformation_host, float*& weight_map_host,
	float*& mesh_dev, float*& handles_dev, float*& transformation_dev, float*& weight_map_dev,
	int N, int M)
{

	mesh_dev = NULL;
	handles_dev = NULL;
	transformation_dev = NULL;
	weight_map_dev = NULL;

	cudaMalloc((void**)&mesh_dev, N * 3 * sizeof(float));
	cudaMalloc((void**)&handles_dev, M * 3 * sizeof(float));
	cudaMalloc((void**)&transformation_dev, 4 * M * 4 * sizeof(float));
	cudaMalloc((void**)&weight_map_dev, N*M * sizeof(float));
	cudaMemcpy(mesh_dev, mesh_host, N * 3 * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(handles_dev, handles_host, M * 3 * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(weight_map_dev, weight_map_host, N*M * sizeof(float), cudaMemcpyHostToDevice);
}

void freeSpaceInCuda(float* mesh_host, float* handles_host, float* transformation_host, float* weight_map_host,
	float* mesh_dev, float* handles_dev, float* transformation_dev, float* weight_map_dev,
	int N, int M)
{
	cudaFree(mesh_dev);
	cudaFree(handles_dev);
	cudaFree(transformation_dev);
	cudaFree(weight_map_dev);

	free(mesh_host);
	free(handles_host);
	free(transformation_host);
	free(weight_map_host);

	cudaDeviceReset();
}
