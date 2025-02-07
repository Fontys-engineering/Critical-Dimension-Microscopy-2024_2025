----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 27.01.2025 15:39:25
-- Design Name: 
-- Module Name: matrix_pkg - Behavioral
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


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

package matrix_pkg is
    type t1_2d_array is array (0 to 2, 0 to 2) of signed(7 downto 0);
    type t2_2d_array is array (0 to 2, 0 to 2) of signed(15 downto 0);
end matrix_pkg;

