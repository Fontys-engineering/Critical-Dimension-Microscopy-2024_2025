%True solution = [3;2;1]
y = [13;6;15]; %output vector
A = [2, 3, 1; %system matrix 
     1, 1, 1;
     3, 2, 2];

[mA, nA] = size(A);
[my, ny] = size(y);

if my ~= mA || ny ~= 1
    error('Matrix A and vector y are incompatible.');
end

Anorm = A/norm(A); %Normalizing A
ynorm = y/norm(y); %Normalizing y

x = zeros(nA,1); %create solution vector

M = 5; %Outer loop #iterations
x_0 = 0; %Initial guess for solution x
r=ynorm; %Initial residual = ynorm- Anrom*x_0 -> x_0 is 0 initially

AtA = Anorm' * Anorm; %Anorm * Anorm_transpose

chi = 0.9; %Safety margin
tau = (2 - chi) / norm(AtA); %Step size tau
err = 1e-6; %Error tolerance

%create vectors dx, dx_prev & b
dx = zeros(nA,1);
dx_prev = zeros(nA,1);
b= zeros(nA,1); 

dxk = zeros(nA,1);     
dxk_prev = zeros(nA,1);

k = 1; %Number of inner-loop iterations

for i=1:M %Outer-loop M times
    b = tau* Anorm' * r;  %Adjustment vector (b) calculation with residual error
    
     while(  ~((norm(dxk- dxk_prev) < err)   &&  ~(k==1)) ) %inner loop condition 

        if((k==1)) %first time in the inner loop
            dxk_prev = zeros(nA, 1);
            dxk = b;
        else        
            dxk_prev = dxk;
            dxk = dxk - Anorm' * (Anorm * dxk_prev) + b; %update inner loop refin
        end %end if

        k = k+1; %Inner loop counter increase

    end %end while

    dx = dxk; %update correction vector with

    if(i==1) %very first time in the outer loop
        x = x_0 + dx; %update solution
    else
        x = x + dx;   %update solution
    end
        k = 1; %reset k to 1
    
    r = ynorm- Anorm*x; %update residual 
end

x = x*norm(y)/norm(A); %scale x back
disp(x);