library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity FixedPointSolver is
    Generic (
        word_length : integer := 8;
        frac_length : integer := 5;
        M : integer := 10;
        N : integer := 100;
        err_tol : integer := 32 -- Error tolerance squared (scaled)
    );
    Port (
        clk : in STD_LOGIC;
        rst : in STD_LOGIC;
        start : in STD_LOGIC;
        done : out STD_LOGIC;
        y : in signed(word_length-1 downto 0);
        A : in signed(word_length-1 downto 0);
        x_out : out signed(word_length-1 downto 0)
    );
end FixedPointSolver;

architecture Behavioral of FixedPointSolver is

    type fixed_array is array (0 to M) of signed(word_length-1 downto 0);
    signal x : fixed_array := (others => (others => '0'));
    signal r : fixed_array := (others => (others => '0'));
    signal dx : signed(word_length-1 downto 0) := (others => '0');
    signal dx_prev : signed(word_length-1 downto 0) := (others => '0');
    signal b : signed(word_length-1 downto 0) := (others => '0');
    signal norm_diff_squared : integer := 0;
    signal iteration_outer : integer range 0 to M := 0;
    signal iteration_inner : integer range 0 to N := 0;
    signal state : integer range 0 to 3 := 0;

begin

    process(clk, rst)
    begin
        if rst = '1' then
            x <= (others => (others => '0'));
            r <= (others => (others => '0'));
            dx <= (others => '0');
            dx_prev <= (others => '0');
            b <= (others => '0');
            norm_diff_squared <= 0;
            iteration_outer <= 0;
            iteration_inner <= 0;
            state <= 0;
            done <= '0';
        elsif rising_edge(clk) then
            case state is
                when 0 => -- Initialization
                    if start = '1' then
                        r(0) <= y;
                        x(0) <= (others => '0');
                        iteration_outer <= 0;
                        state <= 1;
                    end if;

                when 1 => -- Outer loop
                    if iteration_outer < M then
                        -- Compute b = tau * A' * r
                        b <= (A * r(iteration_outer)) srl frac_length; -- Scaled multiplication
                        dx <= (others => '0');
                        dx_prev <= (others => '0');
                        iteration_inner <= 0;
                        state <= 2;
                    else
                        done <= '1';
                        state <= 0;
                    end if;

                when 2 => -- Inner loop
                    if iteration_inner < M then
                        -- Compute dxk = dxk_prev - A' * (A * dxk_prev) + b
                        dx_prev <= dx;
                        dx <= dx_prev - ((A * (A * dx_prev) srl frac_length) srl frac_length) + b;

                        -- Compute norm_diff_squared
                        norm_diff_squared <= to_integer(abs(dx - dx_prev) * abs(dx - dx_prev));

                        if norm_diff_squared < err_tol then
                            x(iteration_outer) <= x(iteration_outer) + dx;
                            iteration_outer <= iteration_outer + 1;
                            state <= 1;
                        else
                            iteration_inner <= iteration_inner + 1;
                        end if;
                    else
                        iteration_outer <= iteration_outer + 1;
                        state <= 1;
                    end if;

                when others =>
                    state <= 0;
            end case;
        end if;
    end process;

    x_out <= x(iteration_outer);

end Behavioral;