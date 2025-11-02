library verilog;
use verilog.vl_types.all;
entity AluModule is
    port(
        src1            : in     vl_logic_vector(31 downto 0);
        src2            : in     vl_logic_vector(31 downto 0);
        alu_control     : in     vl_logic_vector(2 downto 0);
        result          : out    vl_logic_vector(31 downto 0);
        zero            : out    vl_logic
    );
end AluModule;
