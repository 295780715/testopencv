#include "myfunctions.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/*1.���������Ի���*/
void section1_setup() {
	Mat src = imread("D:/dehaze_image/haze0.jpg");
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}
	namedWindow("test opencv setup", CV_WINDOW_AUTOSIZE);
	imshow("test opencv setup", src);

	waitKey(0);
}


/*2.ͼ������룬�޸ģ�����*/
void section2_io() {
	/*
	ͼ�����룬��һ������Ϊͼ��·�����ڶ�������Ϊͼ������IMREAD_XXX
	Ĭ������£���IMREAD_COLOR���أ���ʾRGB��ɫͼ�񣬿���ͨ���ò����ƶ�����Ϊʲô���ͣ���Ҷ�ͼ��
	imread���Լ���jpg��png��tif�ȳ���ͼƬ��ʽ
	*/
	Mat src = imread("D:/dehaze_image/lena.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}

	// ԭͼ
	imshow("original", src);

	Mat output;
	/*
	cvtColor,convert color,ͼƬ��ɫ����ת�����������ڲ�ͬ��ɫ���͵�ת��
	���������ֱ�Ϊ��input��output����ת������CV_XXX2XXX
	*/
	cvtColor(src, output, CV_BGR2GRAY);

	// ת��֮���ͼ
	imshow("gray_img", output);

	/*
	����ת��֮���ͼƬ
	�����ֱ�Ϊ������·�������ͼ���array������vector
	��ָ������ʱ������ݱ���ĺ�׺���Զ����䱣������
	*/
	imwrite("D:/dehaze_image/lena_gray.jpg", output);
	// pause
	waitKey(0);
}

/*
3.ͼ����Ĥ������ͼ������ؼ�����
��Ĥ����mask������ģ�������
�˴����õ�ģ�壨���ӣ�����
|0 -1 0 |
|-1 5 -1|
|0 -1 0 |
������������:��߶Աȶȣ��м���Ȩֵ�ߣ�4����Ϊ��Ȩֵ
*/
void section3_1() {
	Mat src = imread("D:/dehaze_image/haze0.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("could't load image..\n");
		return;
	}

	// ��ʾԭͼ
	namedWindow("original img", CV_WINDOW_AUTOSIZE);
	imshow("original img", src);

	// ����Ĥ����
	// 1.�ó���������
	// ע��1������ͼ�����mat������ʱ��ÿһ�еĸ����صĸ�ͨ��ֵ���������е�
	// ��RGBͼ��ľ���ĳһ��������������:(i0_b,i0_g,i0_r,  i1_b,i1_g,i1_r,  i2_b,i2_g,i2_r)
	// ע��2��ģ�������ʱ����Ҫ����߽磬�˴����õĴ������ǣ��߽粻����ģ���������col��row��Ҫ��ȥ�߽�
	// ע��3��cols��rows�Ǵ�0��ʼ����ģ������������һ����
	int cols_end = (src.cols - 1)*src.channels();
	int rows_end = src.rows - 1;
	int col_offset = src.channels();

	// ��������ͼ��Ĵ�С�����ͣ���ʼ��һ�����ͼ��ľ���
	Mat out = Mat::zeros(src.size(), src.type());

	int64 t1 = getTickCount();
	for (int row = 1; row < rows_end; row++) {
		// ��ȡ��Ҫ���������������
		const uchar* currentRow = src.ptr<uchar>(row);
		const uchar* preRow = src.ptr<uchar>(row - 1);
		const uchar* nextRow = src.ptr<uchar>(row + 1);
		uchar * outrow = out.ptr<uchar>(row);
		for (int col = col_offset; col < cols_end; col++) {
			// ��Ĥ���������Ǿ����������ģ��ĳ˼Ӳ���
			outrow[col] = saturate_cast<uchar>(5 * currentRow[col] - preRow[col] - nextRow[col] - currentRow[col - col_offset] - currentRow[col + col_offset]);
			// �˴���Ҫ�ж�outrow[col]��ֵԽ���𣿳���255����Ȼ����Ҫ�ģ�opencv�ṩ�˸ú���,��ֵ��������[0,255]��
			// outrow[col] = saturate_cast<uchar>(outrow[col]); 
			// ���д����Ǵ���ģ�������ԭ����ֵ��saturate����
			// ԭ��outrow�����͸���src.type���ʴ洢ֵӦ����255֮�ڵġ�������ֵ�������������
			// Ĭ�ϵĴ�������Ĳ�����ȡ��8λ����͵�����ֵ�쳣���������Ĵ����ǳ���255��ȡ255��С��0��ȡ0.
			// �ʣ�outrow[col] = saturate_cast<uchar>(outrow[col])��������Ч�ģ�outrow�Ѿ���ȡ�������Ĭ�ϵ��������
		}
	}
	int64 t2 = getTickCount();
	printf("section3_1 time consume:%.2f\n", (t2 - t1) / getTickFrequency());

	// ���mask֮��ͼ��
	namedWindow("masked image", CV_WINDOW_AUTOSIZE);
	imshow("masked image", out);

	waitKey(0);
}

/*opencv���ṩ����Ĥ����*/
void section3_2() {
	Mat src = imread("D:/dehaze_image/haze0.jpg", IMREAD_COLOR);
	if (!src.data) {
		printf("could't load image..\n");
		return;
	}

	// ��ʾԭͼ
	namedWindow("original img2", CV_WINDOW_AUTOSIZE);
	imshow("original img2", src);

	// ���Ŀ��
	Mat dst = Mat::zeros(src.size(), src.type());
	// ģ������
	Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);

	int64 t1 = getTickCount();
	// ���ṩ��ģ��������������������Ϊ��ȣ��ڲ�֪����ʱ�����-1����ʾ������ͼ����ͬ�����
	filter2D(src, dst, src.depth(), kernel);
	int64 t2 = getTickCount();
	printf("section3_2 time consume:%.2f\n", (t2 - t1) / getTickFrequency());

	// ���mask֮��ͼ��
	namedWindow("masked image2", CV_WINDOW_AUTOSIZE);
	imshow("masked image2", dst);

	waitKey(0);
}

/*Mat��ʹ��*/
void section4_mat()
{
	Mat src = imread("D:/dehaze_image/haze1.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}
	
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	// Mat dst = Mat(src.size(), src.type()); // ����һ���յ�Mat����
	// dst = Scalar(255, 0, 0); // ����ֵ��scalar���������ֱ�ΪBGR

	// Mat dst = src.clone();   // Clone������ȫ����һ��Mat����
	
	Mat dst;
	// src.copyTo(dst);
	cvtColor(src, dst, CV_BGR2GRAY);

	namedWindow("output", CV_WINDOW_AUTOSIZE);
	imshow("output", dst);

	// ���ù��죺 Mat(rols,cols,type,&scaler)
	Mat m(3, 3, CV_8UC3, Scalar(0, 255, 0));
	cout << "m = " << endl;
	cout << m << endl;

	// ��һ�֣�create��ʽ
	Mat mat;
	mat.create(src.size(), src.type());
	mat = Scalar(0, 255, 0);
	imshow("mat", mat);
	
	// ����ģ�塢kernel,С�Ķ�ά����
	Mat k = (Mat_<int>(3, 3) << 0, 0, 0, 1, 1, 1, 0, 0, 0);
	cout << k << endl;

	// ��λ����eye�������Mat::zeros(rows,cols,type)
	Mat e = Mat::eye(3, 3, CV_8UC1);
	cout << e << endl;


	cout << "src channel:" << src.channels() << ",size:" << src.size() << endl;
	cout << "dst channel:" << dst.channels() << ",size:" << dst.size() << endl;
	waitKey(0);
}

/*5.���ز�������ͨ��*/
void section5_pixel_1c()
{
	Mat src, gray_src;
	src = imread("D:/dehaze_image/haze2.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}

	// mat.at<type>(row_index,col_index)��ʽ���ʵ�ͨ������
	// �����ǵ�ͨ����������Ҫ��ת��Ϊ�Ҷ�ͼ
	cvtColor(src, gray_src, CV_BGR2GRAY);
	// ���ԭ�Ҷ�ͼ
	imshow("original gray", gray_src);
	for(int i = 0; i < gray_src.rows; i++) {
		for (int j = 0; j < gray_src.cols; j++) {
			uchar value = gray_src.at<uchar>(i, j);
			// mat.at��������ԭ�ظ�ֵ����
			gray_src.at<uchar>(i, j) = 255 - value; // ��ɫ����
		}
	}
	// ��ɫ֮��ĻҶ�ͼ
	// opencvĬ����û��namedwindowʱ���Զ�������ʾ���ڣ���Ȼ������ȥѰ��ͬ���Ĵ��ڣ�����һ��
	imshow("processed gray", gray_src);

	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);

	waitKey(0);
}

/*5.���ز���,��ͨ��*/
void section5_pixel_mc()
{
	Mat src, dst;
	src = imread("D:/dehaze_image/haze2.jpg", IMREAD_COLOR);
	if (src.empty()) {
		printf("could't load image..\n");
		return;
	}
	dst.create(src.size(), src.type());
	dst = Scalar(0, 0, 255);

	// ���ԭͼ
	imshow("original", src);

	// mat.at<��������>(row_index,col_index)��ʽ���ʶ�ͨ������
	int channels = src.channels();
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			// typedef Vec<uchar, 3> Vec3b; 
			// ע����ߵ�����Ӧ������������&������ԭ�ز���
			// ������벢����ͨ�ø���ͨ������ΪVec3bָ����ͨ����Ϊ3
			Vec3b & values = src.at<Vec3b>(i, j);
			Vec3b & dst_values = dst.at<Vec3b>(i, j);
			// ����ÿ��ͨ��
			for (int c = 0; c < channels; c++) {
				// ��ɫ����,ͬ������ԭ�ز���,��mat.at���ص�&���ã��ʿ���ԭ�ز���
				dst_values[c] = 255 - values[c];
			}
		}
	}
	imshow("processed", dst);

	// opencv���õ�ͬ��Ч���ĺ���,bitwise_not,�ڲ�ͨ��λ����ʵ��
	Mat cvdst;
	bitwise_not(src, cvdst);
	imshow("cv processed", cvdst);

	waitKey(0);
}

/*6.ͼ���ں�*/
void section6_blend()
{
	Mat src1, src2, dst;
	src1 = imread("D:/dehaze_image/haze2.jpg", IMREAD_COLOR);
	src2 = imread("D:/dehaze_image/hazefree2.jpg", IMREAD_COLOR);
	imshow("src1", src1);
	imshow("src2", src2);

	// opencv���� Ȩ�ػ�Ϻ���
	if (src1.size == src2.size && src1.type() == src2.type()) {
		//addWeighted(src1, 0.5, src2, 0.5, 1, dst);
		//imshow("blend", dst);
	}

	// ����ʵ��,����ָ�����
	if (src1.size == src2.size && src1.type() == src2.type()) {
		dst = Mat::zeros(src1.size(), src1.type());
		for (int i = 0; i < src1.rows; i++) {
			uchar * row_src1_ptr = src1.ptr<uchar>(i);
			uchar * row_src2_ptr = src2.ptr<uchar>(i);
			uchar * row_dst_ptr = dst.ptr<uchar>(i);
			for (int j = 0; j < src1.cols*src1.channels(); j++)
			{
				row_dst_ptr[j] = row_src1_ptr[j] * 0.5 + row_src2_ptr[j] * 0.5;
			}
		}
		imshow("blend", dst);
	}
	else {
		// ��С���Ͳ�ͬ����Ҫת��
	}
	waitKey(0);
}
