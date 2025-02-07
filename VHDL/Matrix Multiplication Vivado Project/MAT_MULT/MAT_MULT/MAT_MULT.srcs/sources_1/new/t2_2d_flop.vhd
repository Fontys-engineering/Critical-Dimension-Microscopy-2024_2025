----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 27.01.2025 21:25:44
-- Design Name: 
-- Module Name: t2_2d_flop - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity t2_2d_flop is
    port(
            clk : in std_logic;
            rst : in std_logic;
            D   : in t2_2d_array;
            Q   : out t2_2d_array
     );
end entity;

architecture Behavioral of t2_2d_flop is
begin
    process (clk, rst)
    begin
        if (rst = '1') then
            Q <= (
                    (B"0000_0000_0000_0000", B"0000_0000_0000_0000", B"0000_0000_0000_0000"), 
                    (B"0000_0000_0000_0000", B"0000_0000_0000_0000", B"0000_0000_0000_0000"), 
                    (B"0000_0000_0000_0000", B"0000_0000_0000_0000", B"0000_0000_0000_0000")
            );
        elsif (rising_edge(clk)) then
            Q <= D;
        end if;
    end process;
end Behavioral;
