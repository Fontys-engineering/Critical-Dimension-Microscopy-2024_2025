----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 27.01.2025 15:37:28
-- Design Name: 
-- Module Name: matrix_multiplication - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;
--use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;
use ieee.numeric_std.all;
use work.fixedpoint.all;
use work.matrix_pkg.all;




entity matrix_multiplication is
    port( 
           A : in t1_2d_array;
           B : in t1_2d_array;
           S : out t2_2d_array
     );
end entity;


architecture Behavioral of matrix_multiplication is
    signal mat : t2_2d_array;

begin
    process (A, B)
    begin    
       for i in 0 to 2 loop
           for j in 0 to 2 loop
                mat(i,j) <= fixedpoint_sum(fixedpoint_prod(A(i,0), B(0,j)),
                            fixedpoint_sum(fixedpoint_prod(A(i,1), B(1,j)),
                                           fixedpoint_prod(A(i,2), B(2,j))));
           end loop;
       end loop;
    end process;
    S <= mat;
end Behavioral;


