library verilog;
use verilog.vl_types.all;
entity RegisterFile is
    port(
        clk             : in     vl_logic;
        we              : in     vl_logic;
        rs              : in     vl_logic_vector(4 downto 0);
        rt              : in     vl_logic_vector(4 downto 0);
        rd              : in     vl_logic_vector(4 downto 0);
        write_data      : in     vl_logic_vector(31 downto 0);
        read_data1      : out    vl_logic_vector(31 downto 0);
        read_data2      : out    vl_logic_vector(31 downto 0)
    );
end RegisterFile;
