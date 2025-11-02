library verilog;
use verilog.vl_types.all;
entity mux_8to1 is
    port(
        a               : in     vl_logic_vector(31 downto 0);
        b               : in     vl_logic_vector(31 downto 0);
        c               : in     vl_logic_vector(31 downto 0);
        d               : in     vl_logic_vector(31 downto 0);
        e               : in     vl_logic_vector(31 downto 0);
        f               : in     vl_logic_vector(31 downto 0);
        g               : in     vl_logic_vector(31 downto 0);
        h               : in     vl_logic_vector(31 downto 0);
        sel             : in     vl_logic_vector(2 downto 0);
        \out\           : out    vl_logic_vector(31 downto 0)
    );
end mux_8to1;
