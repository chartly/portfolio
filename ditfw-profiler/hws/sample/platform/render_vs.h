#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /T vs_4_0 /E ParticleVS /Fh render_vs.h render.hlsl
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// POSITION                 0   xy          0     NONE  float   xy  
// COLOR                    0   xyzw        1     NONE  float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// POSITION                 0   xy          0     NONE  float   xy  
// COLOR                    0   xyzw        1     NONE  float   xyzw
//
vs_4_0
dcl_input v0.xy
dcl_input v1.xyzw
dcl_output o0.xy
dcl_output o1.xyzw
mov o0.xy, v0.xyxx
mov o1.xyzw, v1.xyzw
ret 
// Approximately 3 instruction slots used
#endif

const BYTE g_ParticleVS[] =
{
     68,  88,  66,  67, 130, 179, 
     11, 185, 151, 183,  69,  18, 
    123, 184, 118, 250, 133, 221, 
    170, 105,   1,   0,   0,   0, 
     20,   2,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    140,   0,   0,   0, 220,   0, 
      0,   0,  44,   1,   0,   0, 
    152,   1,   0,   0,  82,  68, 
     69,  70,  80,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,   0,   1,   0,   0, 
     28,   0,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  57,  46,  50, 
     57,  46,  57,  53,  50,  46, 
     51,  49,  49,  49,   0, 171, 
    171, 171,  73,  83,  71,  78, 
     72,   0,   0,   0,   2,   0, 
      0,   0,   8,   0,   0,   0, 
     56,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   3,   0,   0, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,  15,   0,   0, 
     80,  79,  83,  73,  84,  73, 
     79,  78,   0,  67,  79,  76, 
     79,  82,   0, 171,  79,  83, 
     71,  78,  72,   0,   0,   0, 
      2,   0,   0,   0,   8,   0, 
      0,   0,  56,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,  12, 
      0,   0,  65,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,   0, 
      0,   0,  80,  79,  83,  73, 
     84,  73,  79,  78,   0,  67, 
     79,  76,  79,  82,   0, 171, 
     83,  72,  68,  82, 100,   0, 
      0,   0,  64,   0,   1,   0, 
     25,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      0,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5,  50,  32,  16,   0, 
      0,   0,   0,   0,  70,  16, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};