----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 27.01.2025 15:40:23
-- Design Name: 
-- Module Name: matrix_mult_tb - Behavioral
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
use work.fixedpoint.all;
use work.matrix_pkg.all;


entity tb_matrix_multiplication is
end entity;



architecture testbench of tb_matrix_multiplication is
    -- Component declaration
    component matrix_multiplication
        port(
            A : in t1_2d_array;
            B : in t1_2d_array;
            S : out t2_2d_array
        );
    end component;
    
    component t2_2d_flop
        port(
            d : in t2_2d_array;
            q : out t2_2d_array;
            clk : in std_logic;
            rst : in std_logic
        );
    end component;
    -- Signals for testing
    signal A_tb : t1_2d_array;
    signal B_tb : t1_2d_array;
    signal S_tb : t2_2d_array;
    signal clk  : std_logic;
    signal rst  : std_logic;
    signal flop_out : t2_2d_array;
    
    constant clk_period : time := 100ns;

begin
    -- Instantiate the matrix multiplication unit
    uut2: matrix_multiplication
        port map(
            A => A_tb,
            B => B_tb,
            S => S_tb
        );
     flop: t2_2d_flop
        port map(
            d => S_tb,
            q => flop_out,
            clk => clk,
            rst => rst
        );

    -- Test process
    clk_process: process
    begin
        clk <= '0';
        wait for clk_period/2;
        clk <= '1';
        wait for clk_period/2;
    end process;
    

    stimulus: process
    begin
        wait for 10ns;
        rst <= '0';
        
        wait until rising_edge(clk);
        
     -- Test case 1
        report "Test Case 1: Start" severity note;
        A_tb <= (
            ("00001000", "11111000", "00000100"), -- 8 (0.5), -8 (-0.5), 4 (0.25)
            ("11111110", "00010100", "11111000"), -- -2 (-0.125), 20 (1.25), -8 (-0.5)
            ("00000000", "00001000", "11110100")  -- 0, 8 (0.5), -12 (-0.75)
        );

        B_tb <= (
            ("00000100", "11111100", "00000110"), -- 4 (0.25), -4 (-0.25), 6 (0.375)
            ("00010000", "00000000", "11111100"), -- 16 (1.0), 0, -4 (-0.25)
            ("11111110", "00000010", "00010100")  -- -2 (-0.125), 2 (0.125), 20 (1.25)
        );

        wait until rising_edge(clk);


        -- Test case 2
        A_tb <= (
            ("00001111", "11110101", "00001010"), -- 15 (0.9375), -11 (-0.6875), 10 (0.625)
            ("00000101", "00010001", "11111101"), -- 5 (0.3125), 17 (1.0625), -3 (-0.1875)
            ("00000001", "00000000", "00001000")  -- 1 (0.0625), 0, 8 (0.5)
        );

        B_tb <= (
            ("00000010", "00000100", "00000001"), -- 2 (0.125), 4 (0.25), 1 (0.0625)
            ("11111111", "00010100", "00000011"), -- -1 (-0.0625), 20 (1.25), 3 (0.1875)
            ("00000011", "11111100", "00001000")  -- 3 (0.1875), -4 (-0.25), 8 (0.5)
        );

        wait until rising_edge(clk);

        -- End simulation
        wait;
    end process stimulus;    
end testbench;
