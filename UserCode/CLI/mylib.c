#include "mylib.h"

double myatof(const char * s)
{
    double small=1,sum=0;

    //去掉遇到数字前的其他字符
    while(!((*s >= '0' && *s <= '9') || *s == '+' || *s == '-' || *s == '.') && *s != '\0') s++;

    //无数字字符，sum为0；
    if(*s=='\0') 
	    sum=0;
	else{
	    if(*s=='-'){             //如果读到负号用falg标记
	        small*=-1;
	        s++;
	    }
	    if(*s=='+'){              //读到正号则跳过
	        s++;
	    }
	    //计算小数点前的数字
	    while(*s>='0'&&*s<='9')  
	        {
	            sum=sum*10+(*s-'0');
	            s++;
	        }
	    //计算小数点后数字，并统计位数，用small记录，最后除以small变小数
	    if(*s=='.')
	    {
	    	s++;
	    	while(*s>='0'&&*s<='9')
	    	{
	    		sum=sum*10+(*s-'0');
	    		s++;
	    		small*=10;
			}
		}
        if(sum==0){     //如果不进行sum=0时直接进行else操作sum也会等于0但无法过测试点无任何有效数字字符时输出0.000000
            sum=0;
        }
        else{           
		    sum=sum/small;
        }
	}
    return sum;
}

// double myatof(const char * s)
// {
//     double small=1,sum=0,falg=1;
//     int i=0;
//     //去掉遇到数字前的其他字符
//     while(!(s[i]>='0'&&s[i]<='9'||s[i]=='+'||s[i]=='-'||s[i]=='.')&&s[i]!='\0'){
//         i++;
//     }
//     //无数字字符，sum为0；
//     if(s[i]=='\0') 
// 	    sum=0;
// 	else{
// 	    if(s[i]=='-'){             //如果读到负号用falg标记
// 	        falg=-1;
// 	        i++;
// 	    }
// 	    if(s[i]=='+'){              //读到正号则跳过
// 	        i++;
// 	    }
// 	    //计算小数点前的数字
// 	    while(s[i]>='0'&&s[i]<='9')  
// 	        {
// 	            sum=sum*10+(s[i]-'0');
// 	            i++;
// 	        }
// 	    //计算小数点后数字，并统计位数，用small记录，最后除以small变小数
// 	    if(s[i]=='.')
// 	    {
// 	    	i++;
// 	    	while(s[i]>='0'&&s[i]<='9')
// 	    	{
// 	    		sum=sum*10+(s[i]-'0');
// 	    		i++;
// 	    		small*=10;
// 			}
// 		}
//         if(sum==0){     //如果不进行sum=0时直接进行else操作sum也会等于0但无法过测试点无任何有效数字字符时输出0.000000
//             sum=0;
//         }
//         else{           
// 		    sum=falg*sum/small;
//         }
// 	}
//     return sum;
// }