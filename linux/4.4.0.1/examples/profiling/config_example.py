
LLVM_TOOLS_PATH = "../../tools/HEXAGON_Tools/8.4.11/Tools/bin/"
#ver="V68"      #For running Hexagon Trace Analyzer on Lahaina target
elfList=["../../tools/debug/hexagon-trace-analyzer/binaries/image/cdsp.mdt",
        "../../tools/debug/hexagon-trace-analyzer/binaries/fastrpc_shell_3",
        "hexagon_ReleaseG_toolv84_v66/libprofiling_skel.so"
         ]
elfOffsets=[0x00000000,
            0xe1700000,
            0xe4ebb000
            ]