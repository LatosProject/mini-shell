# Mini-Shell

一个用 C 语言实现的迷你 Shell 解释器，支持变量、数组、算术运算、控制流语句和管道。

## 编译

```bash
make
```

## 运行

交互模式：
```bash
./mini-shell
```

执行脚本文件：
```bash
./mini-shell script.msh
```

## 功能特性

### 1. 变量

```bash
a=10;
b=a;          # 变量赋值
echo a;       # 输出: 10
```

### 2. 算术运算

支持 `+` `-` `*` `/` `%` 运算符：

```bash
a=10+5;       # a=15
b=a*2;        # b=30
c=100%7;      # c=2
echo c;
```

### 3. 数组

```bash
arr[5]={1,2,3,4,5};    # 声明并初始化数组
echo arr[0];            # 输出: 1
arr[2]=100;             # 修改元素
tmp=arr[2];             # 读取到变量
```

### 4. 条件语句 (if)

支持 `==` `!=` `>` `<` `>=` `<=` 比较运算符：

```bash
a=10;
if a>5 {echo a;};
if a==10 {b=1;};
```

### 5. 循环语句 (while)

```bash
i=0;
while i<10 {echo i;i=i+1;};
```

### 6. 管道

```bash
ls | grep main
cat file.txt | wc -l
ls | grep c | wc -l
```

## 示例算法

### GCD (最大公约数)

```bash
a=48;b=18;while b!=0{t=b;b=a%b;a=t;};echo a;
# 输出: 6
```

### 阶乘

```bash
n=5;r=1;while n>0{r=r*n;n=n-1;};echo r;
# 输出: 120
```

### 斐波那契数列

```bash
n=25;a=0;b=1;i=0;while i<n{t=a+b;a=b;b=t;i=i+1;};echo a;
# 输出: 75025
```

### 素数判断

```bash
n=17;i=2;flag=1;while i<n{z=n%i;if z==0{flag=0;};i=i+1;};echo flag;
# 输出: 1 (是素数)
```

### 统计素数个数

```bash
c=0;n=2;while n<=100{i=2;f=1;while i<n{z=n%i;if z==0{f=0;};i=i+1;};if f==1{c=c+1;};n=n+1;};echo c;
# 输出: 25
```

### 冒泡排序

```bash
arr[5]={5,2,8,1,9};n=5;i=0;while i<n{j=0;m=n-1;while j<m{tmp=arr[j];k=j+1;tmp2=arr[k];if tmp>tmp2{arr[j]=tmp2;arr[k]=tmp;};j=j+1;};i=i+1;};echo arr[0];echo arr[1];echo arr[2];echo arr[3];echo arr[4];
# 输出: 1 2 5 8 9
```

### 计算 π (莱布尼茨公式)

```bash
s=0;i=0;while i<1000{d=i*4+1;t=40000/d;s=s+t;d=i*4+3;t=40000/d;s=s-t;i=i+1;};echo s;
# 输出: 31404 (π×10000 ≈ 31415)
```

### Collatz 猜想

```bash
n=27;c=0;while n!=1{z=n%2;if z==0{n=n/2;};if z!=0{n=n*3+1;};c=c+1;};echo c;
# 输出: 111
```

## 语法规则

- 每条语句以 `;` 结尾
- `if` 和 `while` 的循环体用 `{}` 包裹
- 变量名由字母组成，区分大小写
- 仅支持整数运算

## 限制

| 项目 | 限制 |
|------|------|
| 最大变量数 | 32 |
| 最大数组大小 | 64 |
| 单行最大长度 | 256 字符 |
| 最大 token 数 | 64 |

## 退出

```bash
exit
```