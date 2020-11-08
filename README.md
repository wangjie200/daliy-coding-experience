# daliy-coding-experience
this repository contains the thinking during my process of coding arithmetic programe that is short
#include<stdio.h>
#include<math.h>
int main(void)
{
    int n,i;
    double a[20][2],count=0;
    scanf("%d",&n);
    if(n==1)
    printf("0.00");
    else
    {
        for(i=0;i<n;i++)
        scanf("%lf%lf",&a[i][0],&a[i][1]);
        for(i=0;i<n-1;i++)
        count+=sqrt((a[i][0]-a[i+1][0])*(a[i][0]-a[i+1][0])+(a[i][1]-a[i+1][1])*(a[i][1]-a[i+1][1]));
        if(n>2)
        count+=sqrt((a[i][0]-a[0][0])*(a[i][0]-a[0][0])+(a[i][1]-a[0][1])*(a[i][1]-a[0][1]));
        printf("%.2lf",count);
    }
    return 0;
}
//对于特殊情况的考虑！
//1、考虑一个点时的情况，长度为0，循环无法进行
//2、考虑两个点时的情况，不构成多边形，用算法会导致长度为两倍
 
