#include <iostream>


double dt=0.002; //周期
double angle=0, bias_angle_speed=0; //角度，角速度零偏
double x[2]={angle,bias_angle_speed}; //状态变量
double A[2][2]={{1,-dt},{0,1}}; //状态方程中的A矩阵
double B[2]={dt,0};  //状态方程中的B矩阵

double Q[2][2]={{0.0025,0},{0,0.0025}}; //误差协方差中的过程噪声。可以计算获得，但太过复杂；所以采用给定默认值（默认值选自约定俗成的值）
double P[2][2]={{1,0},{0,1}}; //没有太多信息的时候，设置为单位阵是可以的

double K[2]={0,0}; //卡尔曼增益系数
double R[2][2]={{0.1,0},{0,0.1}}; //观测噪声
double H[2]={1,0}; 

void Kalman_filter(double angle_speed, double measure_angle_imu, double measure_angle_gps){
    //第一步 预测x
    x[0]=x[0]-dt*x[1]+angle_speed*dt;
    x[1]=x[1];

    //第二步 计算误差协方差
    double temp[2][2]={
        {A[0][0]*P[0][0]+A[0][1]*P[1][0],A[0][0]*P[0][1]+A[0][1]*P[1][1]},
        {A[1][0]*P[0][0]+A[1][1]*P[1][0],A[1][0]*P[0][1]+A[1][1]*P[1][1]}
    };
    P[0][0]=temp[0][0]*A[0][0]+temp[0][1]*A[0][1]+Q[0][0];
    P[0][1]=temp[0][0]*A[1][0]+temp[0][1]*A[1][1]+Q[0][1];
    P[1][0]=temp[1][0]*A[0][0]+temp[1][1]*A[0][1]+Q[1][0];
    P[1][1]=temp[1][0]*A[1][0]+temp[1][1]*A[1][1]+Q[1][1];

    //第三步 更新卡尔曼增益系数k，更新x预测值，更新误差协方差P
    K[0]=(P[0][0]*H[0]+P[0][1]*H[1]) / (((H[0]*P[0][0]+H[1]*P[1][0]) * H[0] + (H[0]*P[0][1]+H[1]*P[1][1]) * H[1]) + R[0][0]);
    K[1]=(P[1][0]*H[0]+P[1][1]*H[1]) / (((H[0]*P[0][0]+H[1]*P[1][0]) * H[0] + (H[0]*P[0][1]+H[1]*P[1][1]) * H[1]) + R[1][1]);

    
    //这里根据频率还可以采用gps数据中穿插imu数据的做法
    x[0]=x[0]+K[0]*(measure_angle_gps - (H[0]*x[0]+H[1]*x[1]));
    x[1]=x[1]+K[1]*(measure_angle_gps - (H[0]*x[0]+H[1]*x[1]));


    P[0][0]=P[0][0]-K[0]*P[0][0];
    P[0][1]=P[0][1]-K[0]*P[0][1];
    P[1][0]=P[1][0]-K[1]*P[0][0];
    P[1][1]=P[1][1]-K[1]*P[0][1];
}
