% True solution [3;2;1];

% System matrix and vector
A = [2, 3, 1;
     1, 1, 1;
    3, 2,  2];

y = [13;6;15];

% Check for compatibility
[mA, nA] = size(A);
[my, ny] = size(y);

if my ~= mA || ny ~= 1
    error('Matrix A and vector y are incompatible');
end

% *Floating-Point 

% Normalize A and y
Anorm_fp = A/norm(A);
ynorm_fp = y/norm(y);

x_fp = zeros(nA,1); % Initial solution
M = 5; % Outer loop iterations
x_0_fp = 0;
r_fp = ynorm_fp;

AtA_fp = Anorm_fp' * Anorm_fp;
chi = 1; % Safety margin
tau_fp = (2 - chi) / norm(AtA_fp); % Step size tau
err = 1e-3; % Error tolerance

dx_fp = zeros(nA,1);
dx_prev_fp = zeros(nA,1);
b_fp = zeros(nA,1);

dxk_fp = zeros(nA,1);     
dxk_prev_fp = zeros(nA,1);
k_fp = 1; % Inner-loop counter

for i = 1:M % Outer loop
    b_fp = tau_fp * Anorm_fp' * r_fp;  
    
    while ~(norm(dxk_fp - dxk_prev_fp) < err && ~(k_fp == 1)) % Inner loop condition
        if k_fp == 1
            dxk_prev_fp = zeros(nA, 1);
            dxk_fp = b_fp;
        else        
            dxk_prev_fp = dxk_fp;
            dxk_fp = dxk_fp - Anorm_fp' * (Anorm_fp * dxk_prev_fp) + b_fp;
        end
        k_fp = k_fp + 1; % Increment inner loop
    end

    dx_fp = dxk_fp;
    
    if i == 1
        x_fp = x_0_fp + dx_fp;
    else
        x_fp = x_fp + dx_fp;
    end
    k_fp = 1; % Reset inner loop counter
    r_fp = ynorm_fp - Anorm_fp * x_fp;
end

x_fp = x_fp * norm(y) / norm(A); % Rescale

disp('Floating-Point Solution:');
disp(x_fp);

% Fixed-Point 
word_length = 8;  % Total number of bits
frac_length = 5;   % Number of fractional bits
fix_format = @(x) fi(x, 1, word_length, frac_length); % Fixed-point function

% Convert A & y to fixed-point
Afi = fix_format(A);
yfi = fix_format(y);

% Normalize A and y in fixed-point
Anorm_fi = fix_format(times(Afi, 1 / norm(double(Afi))));
ynorm_fi = fix_format(times(yfi, 1 / norm(double(yfi))));

x_fi = fix_format(zeros(nA,1));

M = 5; % Outer-loop iterations
x_0_fi = fix_format(0);
r_fi = ynorm_fi;

AtA_fi = fix_format(Anorm_fi' * Anorm_fi);
tau_fi = fix_format((2 - chi) / norm(double(AtA_fi))); % Step size tau
err_fi = fi(1e-6, 1, word_length, frac_length); % Error tolerance

dx_fi = fix_format(zeros(nA,1));
dx_prev_fi = fix_format(zeros(nA,1));
b_fi = fix_format(zeros(nA,1));

dxk_fi = fix_format(zeros(nA,1));
dxk_prev_fi = fix_format(zeros(nA,1));
k_fi = 1; % Inner-loop counter



for i = 1:M % Outer loop
    b_fi = fix_format(tau_fi * Anorm_fi' * r_fi);  

    while ~(norm(double(dxk_fi - dxk_prev_fi)) < double(err_fi) && ~(k_fi == 1)) % Inner loop
        if k_fi == 1
            dxk_prev_fi = fix_format(zeros(nA, 1));
            dxk_fi = b_fi;
        else        
            dxk_prev_fi = fix_format(dxk_fi);
            dxk_fi = fix_format(dxk_fi - fix_format(Anorm_fi' * (Anorm_fi * dxk_prev_fi)) + b_fi);
        end
        k_fi = k_fi + 1;
    end

    dx_fi = fix_format(dxk_fi);
    
    if i == 1
        x_fi = fix_format(x_0_fi + dx_fi);
    else
        x_fi = fix_format(x_fi + dx_fi);
    end
    
    k_fi = 1; % Reset inner loop counter
    r_fi = fix_format(ynorm_fi - Anorm_fi * x_fi);
end

x_fi = fix_format(x_fi * norm(y) / norm(A)); % Rescale
disp('Fixed-Point Solution:');
disp(double(x_fi));



