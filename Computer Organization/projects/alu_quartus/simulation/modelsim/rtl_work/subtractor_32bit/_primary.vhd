library verilog;
use verilog.vl_types.all;
entity subtractor_32bit is
    port(
        a               : in     vl_logic_vector(31 downto 0);
        b               : in     vl_logic_vector(31 downto 0);
        diff            : out    vl_logic_vector(31 downto 0)
    );
end subtractor_32bit;
