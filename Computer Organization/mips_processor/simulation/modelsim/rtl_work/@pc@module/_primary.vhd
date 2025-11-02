library verilog;
use verilog.vl_types.all;
entity PcModule is
    port(
        clk             : in     vl_logic;
        rst             : in     vl_logic;
        next_pc         : in     vl_logic_vector(31 downto 0);
        pc              : out    vl_logic_vector(31 downto 0)
    );
end PcModule;
