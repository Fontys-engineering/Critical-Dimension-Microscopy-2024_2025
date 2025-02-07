----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 27.01.2025 15:38:36
-- Design Name: 
-- Module Name: functions - Behavioral
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
use ieee.numeric_std.all;

--use ieee.numeric_std.all;

package fixedpoint is
    function fixedpoint_prod(
        a:      in signed(7 downto 0);       -- 1.3.4 format
        b:      in signed(7 downto 0)       -- 1.7.8 format
    ) return signed;
    function fixedpoint_sum(
        
        c:      in signed(15 downto 0);       -- 1.7.8 format
        d:      in signed(15 downto 0)       -- 1.7.8 format
    ) return signed;
end fixedpoint;

package body fixedpoint is
    function fixedpoint_prod(
        a:  signed(7 downto 0);
        b:  signed(7 downto 0)
    )return signed is
        variable prod_ab: signed(15 downto 0);
    begin
        prod_ab := a * b;
        return prod_ab;
    end function;
    
    function fixedpoint_sum(
        c:  signed(15 downto 0);
        d:  signed(15 downto 0)
    )return signed is
    
        variable sum_ab: signed(16 downto 0);
        variable result: signed(15 downto 0); -- Final result
    begin
        sum_ab := (c(15) & c) + (d(15) & d);
        -- Check for overflow and saturate
          if (c(15) = '1' and d(15) = '1' and sum_ab(16) = '0') then
                result := b"1000_0000_0000_0000"; 
          elsif (c(15) = '0' and d(15) = '0' and sum_ab(16) = '1') then
                result := b"0111_1111_1111_1111";
          else
                result := sum_ab(15 downto 0); 
          end if;
        return result;
    end function;
end package body;