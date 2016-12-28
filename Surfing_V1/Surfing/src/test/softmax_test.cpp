#include <iostream>
#include <cudnn.h>
#include <vector>

#include "basic/blob.h"
#include "basic/common.h"
#include "basic/cudnn_api.h"

using namespace std;
using namespace surfing;


void Softmax_TEST()
{
	float A[6] = { 0.3, 0.2, 0.3, 0.1, 0.5, 0.6 };

	float C[6] = { 0.158860531865695, 0.143742953481174, -0.841139468134305, 0.130064002888769, 0.194032691783551, 0.214439288115116 };

	Blob<float>* bottom;

	bottom = new Blob<float>;
	bottom->Reshape(1, 6, 1, 1);
	bottom->set_cpu_data(A);
	bottom->set_cpu_diff(C);

	cudnnHandle_t handle_;
	cudnnTensorDescriptor_t bottom_desc_;

	cudnnCreate(&handle_);
	cudnnCreateTensorDescriptor(&bottom_desc_);

	vector<int> shape;

	shape = bottom->shape();
	cudnnSetTensor4dDescriptor(bottom_desc_, CUDNN_TENSOR_NCHW, cudnn::dataType<float>::type,
		shape[0], shape[1], shape[2], shape[3]);

	for (int i = 0; i < bottom->count(); i++)
	{
		LOG(INFO) << bottom->cpu_data()[i];
	}
	CUDNN_CHECK(cudnnSoftmaxForward(handle_, CUDNN_SOFTMAX_ACCURATE, CUDNN_SOFTMAX_MODE_INSTANCE,
		cudnn::dataType<float>::one, bottom_desc_, bottom->gpu_data(),
		cudnn::dataType<float>::zero, bottom_desc_, bottom->mutable_gpu_data()));

	for (int i = 0; i < bottom->count(); i++)
	{
		LOG(INFO) << bottom->cpu_data()[i];
	}

	CUDNN_CHECK(cudnnSoftmaxBackward(handle_, CUDNN_SOFTMAX_ACCURATE, CUDNN_SOFTMAX_MODE_INSTANCE,
		cudnn::dataType<float>::one, bottom_desc_, bottom->gpu_data(),
		bottom_desc_, bottom->gpu_diff(),
		cudnn::dataType<float>::zero, bottom_desc_, bottom->mutable_gpu_diff()));

	for (int i = 0; i < bottom->count(); i++)
	{
		LOG(INFO) << bottom->cpu_diff()[i];
	}

	cudnnDestroy(handle_);
	cudnnDestroyTensorDescriptor(bottom_desc_);
}